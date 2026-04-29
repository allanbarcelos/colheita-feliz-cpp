#include "GameState.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <cstdio>
#include <cstring>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

struct GameCode
{
    HMODULE module;
    FILETIME ultimaEscrita;
    GameInitFn init;
    GameFrameFn frame;
    bool valido;
};

static char GAME_DLL_PATH[MAX_PATH] = {};
static char GAME_DLL_CARREGADO[MAX_PATH] = {};

static void resolverCaminhosDll()
{
    char *base = SDL_GetBasePath();
    if (!base)
    {
        snprintf(GAME_DLL_PATH, sizeof(GAME_DLL_PATH), "game.dll");
        snprintf(GAME_DLL_CARREGADO, sizeof(GAME_DLL_CARREGADO), "game_loaded.dll");
        return;
    }
    snprintf(GAME_DLL_PATH, sizeof(GAME_DLL_PATH), "%sgame.dll", base);
    snprintf(GAME_DLL_CARREGADO, sizeof(GAME_DLL_CARREGADO), "%sgame_loaded.dll", base);
    SDL_free(base);
}

static FILETIME obterMtime(const char *caminho)
{
    WIN32_FILE_ATTRIBUTE_DATA dados;
    FILETIME zero = {};
    if (GetFileAttributesExA(caminho, GetFileExInfoStandard, &dados))
    {
        return dados.ftLastWriteTime;
    }
    return zero;
}

static bool mtimeDiferente(FILETIME a, FILETIME b)
{
    return CompareFileTime(&a, &b) != 0;
}

static void descarregarGameCode(GameCode *gc)
{
    if (gc->module)
    {
        FreeLibrary(gc->module);
        gc->module = nullptr;
    }
    gc->init = nullptr;
    gc->frame = nullptr;
    gc->valido = false;
}

static bool carregarGameCode(GameCode *gc)
{
    for (int tentativa = 0; tentativa < 10; tentativa++)
    {
        if (CopyFileA(GAME_DLL_PATH, GAME_DLL_CARREGADO, FALSE))
            break;
        Sleep(50);
    }

    gc->module = LoadLibraryA(GAME_DLL_CARREGADO);
    if (!gc->module)
    {
        printf("[platform] LoadLibrary falhou: %lu\n", GetLastError());
        gc->valido = false;
        return false;
    }

    gc->init = reinterpret_cast<GameInitFn>(GetProcAddress(gc->module, "game_init"));
    gc->frame = reinterpret_cast<GameFrameFn>(GetProcAddress(gc->module, "game_frame"));
    gc->ultimaEscrita = obterMtime(GAME_DLL_PATH);
    gc->valido = (gc->init != nullptr) && (gc->frame != nullptr);

    if (!gc->valido)
    {
        printf("[platform] símbolos não encontrados no DLL\n");
        FreeLibrary(gc->module);
        gc->module = nullptr;
    }

    return gc->valido;
}

static bool dllMudou(const GameCode *gc)
{
    FILETIME atual = obterMtime(GAME_DLL_PATH);
    return mtimeDiferente(atual, gc->ultimaEscrita);
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    SDL_SetHint("SDL_WINDOWS_DPI_AWARENESS", "permonitorv2");
    SDL_SetHint("SDL_WINDOWS_DPI_SCALING", "0");
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        printf("[audio] Mix_OpenAudio falhou: %s\n", Mix_GetError());
    Mix_AllocateChannels(16);

    resolverCaminhosDll();
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    SDL_Window *janela = SDL_CreateWindow(
        "Colheita Feliz",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        LARGURA_JANELA,
        ALTURA_JANELA,
        0);

    SDL_Renderer *renderer = SDL_CreateRenderer(
        janela, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        printf("[platform] erro ao criar renderer: %s\n", SDL_GetError());
        return 1;
    }

    GameState estado = {};
    GameCode gc = {};

    if (!carregarGameCode(&gc))
    {
        printf("[platform] falha ao carregar game.dll inicial\n");
        return 1;
    }

    gc.init(&estado, renderer);
    printf("[platform] game.dll carregado — hot-reload ativo\n");

    Uint32 tempoAnterior = SDL_GetTicks();
    Uint32 ultimaChecagemDll = tempoAnterior;

    while (!estado.solicitouSair)
    {
        Uint32 tempoAtual = SDL_GetTicks();
        float dt = (tempoAtual - tempoAnterior) / 1000.0f;
        tempoAnterior = tempoAtual;

        if (tempoAtual - ultimaChecagemDll > 250)
        {
            ultimaChecagemDll = tempoAtual;
            if (dllMudou(&gc))
            {
                Sleep(100);
                descarregarGameCode(&gc);
                if (carregarGameCode(&gc))
                {
                    gc.init(&estado, renderer);
                    printf("[platform] game.dll recarregado\n");
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

    descarregarGameCode(&gc);

    if (estado.fonteTooltip) TTF_CloseFont(estado.fonteTooltip);
    if (estado.fontePequena) TTF_CloseFont(estado.fontePequena);
    if (estado.fonte) TTF_CloseFont(estado.fonte);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(janela);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    printf("Jogo encerrado\n");
    return 0;
}
