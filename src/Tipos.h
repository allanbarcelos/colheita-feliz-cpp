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

enum TipoCrop
{
    NABO = 0,
    CENOURA = 1,
    TOMATE = 2,
    MILHO = 3,
    MORANGO = 4,
    BATATA = 5,
    MELANCIA = 6,
    ARROZ = 7,
    GIRASSOL = 8,
    REPOLHO = 9,
    ABOBORA = 10,
    BERINJELA = 11,
    UVA = 12,
    PEPINO = 13,
    PIMENTAO = 14,
};

struct Canteiro
{
    int coluna;
    int linha;
    EstadoCanteiro estado;

    int tipoCrop = -1;

    int estagioCrop = 0;
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
    SDL_Texture *icones[7] = {};

    int sementeSelecionada = -1;
    bool painelAberto = false;

    float selecaoAnimacao[7] = {};
    float painelSelAnimacao[15] = {};
    float painelAberturaAnimacao = 0.0f;
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