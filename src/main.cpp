#include "GameState.h"
#include "SaveGame.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <cstdio>
#include <cstring>

// ── Camada de plataforma ─────────────────────────────────────────────────────
// Win32: LoadLibrary / FILETIME / CopyFile / Sleep
// POSIX: dlopen   / stat.st_mtime / fread+fwrite / usleep
// ─────────────────────────────────────────────────────────────────────────────

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

typedef HMODULE  PlataformaModulo;
typedef FILETIME PlataformaMtime;

#else  // POSIX
#include <dlfcn.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

typedef void*  PlataformaModulo;
typedef time_t PlataformaMtime;
#endif  // _WIN32

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

// ── Extensão da lib compartilhada por plataforma ─────────────────────────────
#if defined(_WIN32)
#define LIB_EXT "dll"
#elif defined(__APPLE__)
#define LIB_EXT "dylib"
#else
#define LIB_EXT "so"
#endif

static constexpr int CAMINHO_MAX = PATH_MAX;

struct GameCode
{
    PlataformaModulo modulo;
    PlataformaMtime  ultimaEscrita;
    GameInitFn       init;
    GameFrameFn      frame;
    bool             valido;
};

static char GAME_LIB_PATH[CAMINHO_MAX]         = {};
static char GAME_LIB_CARREGADO[CAMINHO_MAX]    = {};

// ── Helpers de plataforma ────────────────────────────────────────────────────

static void resolverCaminhos()
{
    char *base = SDL_GetBasePath();
    const char *prefixo = base ? base : "";
    snprintf(GAME_LIB_PATH,      sizeof(GAME_LIB_PATH),
             "%sgame." LIB_EXT, prefixo);
    snprintf(GAME_LIB_CARREGADO, sizeof(GAME_LIB_CARREGADO),
             "%sgame_loaded." LIB_EXT, prefixo);
    if (base) SDL_free(base);
}

static PlataformaMtime obterMtime(const char *caminho)
{
#ifdef _WIN32
    WIN32_FILE_ATTRIBUTE_DATA dados;
    if (GetFileAttributesExA(caminho, GetFileExInfoStandard, &dados))
        return dados.ftLastWriteTime;
    FILETIME zero = {};
    return zero;
#else
    struct stat st;
    if (stat(caminho, &st) == 0) return st.st_mtime;
    return 0;
#endif
}

static bool mtimeDiferente(PlataformaMtime a, PlataformaMtime b)
{
#ifdef _WIN32
    return CompareFileTime(&a, &b) != 0;
#else
    return a != b;
#endif
}

static bool copiarArquivo(const char *origem, const char *destino)
{
#ifdef _WIN32
    return CopyFileA(origem, destino, FALSE) != 0;
#else
    FILE *src = fopen(origem, "rb");
    if (!src) return false;
    FILE *dst = fopen(destino, "wb");
    if (!dst) { fclose(src); return false; }
    char buf[65536];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), src)) > 0)
        fwrite(buf, 1, n, dst);
    fclose(src);
    fclose(dst);
    return true;
#endif
}

static void dormirMs(unsigned ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000u);
#endif
}

// ── Gerenciamento do game code dinâmico ──────────────────────────────────────

static void descarregarGameCode(GameCode *gc)
{
    if (gc->modulo)
    {
#ifdef _WIN32
        FreeLibrary(gc->modulo);
#else
        dlclose(gc->modulo);
#endif
        gc->modulo = nullptr;
    }
    gc->init   = nullptr;
    gc->frame  = nullptr;
    gc->valido = false;
}

static bool carregarGameCode(GameCode *gc)
{
    for (int tentativa = 0; tentativa < 10; tentativa++)
    {
        if (copiarArquivo(GAME_LIB_PATH, GAME_LIB_CARREGADO)) break;
        dormirMs(50);
    }

#ifdef _WIN32
    gc->modulo = LoadLibraryA(GAME_LIB_CARREGADO);
#else
    gc->modulo = dlopen(GAME_LIB_CARREGADO, RTLD_NOW | RTLD_LOCAL);
#endif

    if (!gc->modulo)
    {
#ifdef _WIN32
        printf("[platform] LoadLibrary falhou: %lu\n", GetLastError());
#else
        printf("[platform] dlopen falhou: %s\n", dlerror());
#endif
        gc->valido = false;
        return false;
    }

#ifdef _WIN32
    gc->init  = reinterpret_cast<GameInitFn> (GetProcAddress(gc->modulo, "game_init"));
    gc->frame = reinterpret_cast<GameFrameFn>(GetProcAddress(gc->modulo, "game_frame"));
#else
    gc->init  = reinterpret_cast<GameInitFn> (dlsym(gc->modulo, "game_init"));
    gc->frame = reinterpret_cast<GameFrameFn>(dlsym(gc->modulo, "game_frame"));
#endif

    gc->ultimaEscrita = obterMtime(GAME_LIB_PATH);
    gc->valido = (gc->init != nullptr) && (gc->frame != nullptr);

    if (!gc->valido)
    {
        printf("[platform] símbolos não encontrados na lib\n");
        descarregarGameCode(gc);
    }

    return gc->valido;
}

static bool libMudou(const GameCode *gc)
{
    return mtimeDiferente(obterMtime(GAME_LIB_PATH), gc->ultimaEscrita);
}

// ── Ponto de entrada ─────────────────────────────────────────────────────────

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

#ifdef _WIN32
    SDL_SetHint("SDL_WINDOWS_DPI_AWARENESS", "permonitorv2");
    SDL_SetHint("SDL_WINDOWS_DPI_SCALING", "0");
#endif

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        printf("[audio] Mix_OpenAudio falhou: %s\n", Mix_GetError());
    Mix_AllocateChannels(16);

    resolverCaminhos();
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    SDL_Window *janela = SDL_CreateWindow(
        "Fazenda dos Sonhos",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        LARGURA_JANELA, ALTURA_JANELA, 0);

    SDL_Surface *iconeJanela = IMG_Load("assets/sprites/ui/app_icon_mark.png");
    if (iconeJanela)
    {
        SDL_SetWindowIcon(janela, iconeJanela);
        SDL_FreeSurface(iconeJanela);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(
        janela, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        printf("[platform] erro ao criar renderer: %s\n", SDL_GetError());
        return 1;
    }

    GameState estado = {};
    GameCode  gc     = {};

    if (!carregarGameCode(&gc))
    {
        printf("[platform] falha ao carregar game lib\n");
        return 1;
    }

    gc.init(&estado, renderer);
    printf("[platform] game lib carregada — hot-reload ativo\n");

    Uint32 tempoAnterior     = SDL_GetTicks();
    Uint32 ultimaChecagemLib = tempoAnterior;

    while (!estado.solicitouSair)
    {
        Uint32 tempoAtual = SDL_GetTicks();
        float  dt         = (tempoAtual - tempoAnterior) / 1000.0f;
        tempoAnterior = tempoAtual;

        if (tempoAtual - ultimaChecagemLib > 250)
        {
            ultimaChecagemLib = tempoAtual;
            if (libMudou(&gc))
            {
                dormirMs(100);
                descarregarGameCode(&gc);
                if (carregarGameCode(&gc))
                {
                    gc.init(&estado, renderer);
                    printf("[platform] game lib recarregada\n");
                }
                else
                {
                    printf("[platform] reload falhou — saindo\n");
                    break;
                }
            }
        }

        gc.frame(&estado, renderer, dt);
    }

    if (estado.estadoJogo == JOGANDO)
    {
        salvarJogo(estado);
        printf("[platform] auto-save no shutdown\n");
    }

    descarregarGameCode(&gc);

    if (estado.fonteTooltip) TTF_CloseFont(estado.fonteTooltip);
    if (estado.fontePequena)  TTF_CloseFont(estado.fontePequena);
    if (estado.fonte)         TTF_CloseFont(estado.fonte);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(janela);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    printf("Jogo encerrado\n");
    return 0;
}
