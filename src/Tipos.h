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
    Uint32 timestampPlantio = 0;
    int temporadaAtual = 0;

    int saude = 100;
    bool seca = false;
    int praga = 0;
    Uint32 ultimoSorteioEventoMs = 0;
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

    SDL_Texture *cursorApontando = nullptr;
    SDL_Texture *cursorPegando = nullptr;

    int sementeSelecionada = -1;
    bool painelAberto = false;

    float selecaoAnimacao[7] = {};
    float painelSelAnimacao[15] = {};
    float painelAberturaAnimacao = 0.0f;
};

inline const char *nomeFerramenta(Ferramenta f)
{
    switch (f)
    {
    case CURSOR:
        return "Cursor";
    case ENXADA:
        return "Enxada";
    case SACOLA:
        return "Sementes";
    case REGADOR:
        return "Regador";
    case REMOVEDOR:
        return "Removedor de Pragas";
    case PESTICIDA:
        return "Pesticida";
    case MAO:
        return "Colher";
    }
    return "";
}

inline const char *legendaCanteiro(const Canteiro &c)
{
    if (c.estado == VAZIO && c.seca)
        return "Terra Seca - Use o Regador";
    if ((c.estado == PLANTADO || c.estado == MADURO) && c.praga == 1)
        return "Erva Daninha - Use o Removedor";
    if ((c.estado == PLANTADO || c.estado == MADURO) && c.praga == 2)
        return "Minhoca - Use o Pesticida";

    switch (c.estado)
    {
    case BLOQUEADO:
        return "Terra Bloqueada - Expandir Fazenda";
    case VAZIO:
        return "Terra Molhada - Apropriada para Plantação";
    case PLANTADO:
        return "Plantação em Crescimento";
    case MADURO:
        return "Pronto para Colher";
    case RESTOS:
        return "Terra com Restos - Use a Enxada";
    }
    return "";
}

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
