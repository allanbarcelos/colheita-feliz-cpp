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

    Assets assets;
    CropAssets cropAssets;
    HudAssets hudAssets;

    TTF_Font *fonte;
    TTF_Font *fontePequena;
    TTF_Font *fonteTooltip;
    TTF_Font *fonteHud;

    bool solicitouSair;
};

#ifdef _WIN32
#define GAME_API extern "C" __declspec(dllexport)
#else
#define GAME_API extern "C"
#endif

typedef void (*GameInitFn)(GameState *, SDL_Renderer *);
typedef void (*GameFrameFn)(GameState *, SDL_Renderer *, float);
