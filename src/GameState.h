#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Constantes.h"
#include "Tipos.h"
#include "Desenho.h"
#include "Assets.h"
#include "Toolbar.h"
#include "Crops.h"
#include "HudAssets.h"
#include "Missoes.h"
#include "Animal.h"
#include "AnimalAssets.h"
#include "Decoracao.h"
#include "Sons.h"

struct GameState
{
    bool inicializado;

    Canteiro canteiros[GRID_COLUNAS * GRID_LINHAS];
    Toolbar toolbar;

    Uint32 tempoJogoMs;
    float velocidadeTempo;

    int colheitas;
    int xp;
    int ouro;
    int moedasVerdes;
    int popularidade;
    int generoJogador;
    int canteirosComprados;

    int canteiroHover;
    int toolbarHover;
    int hudDireitoHover;
    int mouseX;
    int mouseY;

    bool lojaAberta;
    bool depositoAberto;
    bool modoCompraCanteiro;

    int inventarioSementes[TOTAL_CROPS];
    int inventarioColhidos[TOTAL_CROPS];
    int valorDeposito;

    char logMensagens[5][96];
    int logCount;

    Missao missoesDiarias[3];
    Uint32 timestampUltimoResetMissoes;
    bool painelMissoesAberto;
    float painelMissoesAbertura;
    float lojaAbertura;
    float depositoAbertura;

    int ultimoDiaRecompensa;
    bool recompensaDisponivel;

    Animal animais[3];
    Cachorro cachorro;
    int inventarioRacao;
    int inventarioProdutos[3];
    AnimalAssets animalAssets;
    int animalHover;

    int lojaTabAtiva;
    DecoracaoAssets decoracaoAssets;
    int inventarioDecoracoes[TOTAL_DECORACOES];
    DecoracaoColocada decoracoesColocadas[MAX_DECORACOES_COLOCADAS];
    int totalDecoracoesColocadas;
    int modoColocarItem;
    int rotacaoColocando;
    int decoracaoArrastando;

    Sons sons;
    bool painelSettingsAberto;
    float painelSettingsAbertura;
    int nivelAnteriorParaSom;

    int estadoJogo;
    bool saveExiste;
    int botaoTituloHover;
    int rodapeIconeHover;
    int tipAtualIndex;
    Uint32 tipUltimaTrocaMs;
    float passaroX1, passaroY1;
    float passaroX2, passaroY2;
    bool painelSettingsAbertoTitulo;

    SDL_Texture *tituloBackground;   // NOVO: bg dedicado MJ-quality (substitui reuso farm in-game)
    SDL_Texture *tituloLogo;
    SDL_Texture *tituloGlow;         // NOVO: radial atras do logo, additive blend + breathing alpha
    SDL_Texture *tituloSparkles;     // NOVO: sparkles 4 frames atras do logo, opcional
    SDL_Texture *tituloPassaros;
    SDL_Texture *tituloIconeGithub;
    SDL_Texture *tituloIconeLivepix;
    SDL_Texture *tituloIconeDiscord;

    Assets assets;
    CropAssets cropAssets;
    HudAssets hudAssets;

    TTF_Font *fonte;
    TTF_Font *fontePequena;
    TTF_Font *fonteTooltip;
    TTF_Font *fonteHud;

    bool solicitouSair;

    bool debugAtivo;
};

#ifdef _WIN32
#define GAME_API extern "C" __declspec(dllexport)
#else
#define GAME_API extern "C"
#endif

typedef void (*GameInitFn)(GameState *, SDL_Renderer *);
typedef void (*GameFrameFn)(GameState *, SDL_Renderer *, float);

inline void liberarGameState(GameState &s)
{
    if (!s.inicializado)
        return;

    liberarIconesToolbar(s.toolbar);
    liberarAssets(s.assets);
    liberarCropAssets(s.cropAssets);
    liberarHudAssets(s.hudAssets);
    liberarAnimalAssets(s.animalAssets);
    liberarDecoracaoAssets(s.decoracaoAssets);
    liberarSons(s.sons);

    auto destroi = [](SDL_Texture *&t) {
        if (t) { SDL_DestroyTexture(t); t = nullptr; }
    };
    destroi(s.tituloBackground);
    destroi(s.tituloLogo);
    destroi(s.tituloGlow);
    destroi(s.tituloSparkles);
    destroi(s.tituloPassaros);
    destroi(s.tituloIconeGithub);
    destroi(s.tituloIconeLivepix);
    destroi(s.tituloIconeDiscord);

    if (s.fonteTooltip) { TTF_CloseFont(s.fonteTooltip); s.fonteTooltip = nullptr; }
    if (s.fontePequena) { TTF_CloseFont(s.fontePequena); s.fontePequena = nullptr; }
    if (s.fonte) { TTF_CloseFont(s.fonte); s.fonte = nullptr; }
    if (s.fonteHud) { TTF_CloseFont(s.fonteHud); s.fonteHud = nullptr; }

    s.inicializado = false;
}
