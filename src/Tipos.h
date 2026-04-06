#pragma once
#include <SDL2/SDL.h>

enum EstadoCanteiro
{
    BLOQUEADO = 0,
    VAZIO = 1,
    PLANTADO = 2,
    MADURO = 3,
    RESTOS = 4
};

struct Canteiro
{
    int coluna;
    int linha;
    EstadoCanteiro estado;
};

enum Ferramenta
{
    CURSOR = 0,
    ENXADA = 1,
    SACOLA = 2,
    REGADOR = 3,
    REMOVEDOR = 4,
    PESTICIDA = 5,
    MAO = 6,
};

struct Toolbar 
{
    Ferramenta selecionada = CURSOR;
    SDL_Texture* icones[7] = {};
};

struct Assets
{
    SDL_Texture *background = nullptr;

    SDL_Texture *tileGrama = nullptr;
    SDL_Texture *tileGramaEscuro = nullptr;
    SDL_Texture *tileTerra = nullptr;
    SDL_Texture *tileTerraSeca = nullptr;
    SDL_Texture *tileTerraVermelha = nullptr;
    SDL_Texture *tileBloqueado = nullptr;
    SDL_Texture *tileRestos = nullptr;

    SDL_Texture *casa = nullptr;
    SDL_Texture *casaCachorro = nullptr;
    SDL_Texture *cerca = nullptr;
};