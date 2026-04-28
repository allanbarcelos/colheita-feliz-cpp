#pragma once

#include <SDL2/SDL.h>
#include "Constantes.h"

enum TipoAnimal
{
    GALINHA = 0,
    VACA = 1,
    OVELHA = 2
};

enum RacaCachorro
{
    GOLDEN = 0,
    DOGO = 1,
    KANGAL = 2
};

struct Animal
{
    int tipo;
    float x, y;
    int direcao;

    float targetX, targetY;
    Uint32 timestampProximoTarget;

    Uint32 timestampUltimaProducao;
    bool produtoPronto;
    bool comFome;
    Uint32 timestampUltimaRefeicao;
};

struct Cachorro
{
    int raca;
    float x, y;
    int direcao;
    float targetX, targetY;
    Uint32 timestampProximoTarget;
    bool emMovimento;
};

static constexpr Uint32 TEMPO_FOME_MS = 12u * 3600u * 1000u;

inline const char *nomeProduto(int tipoAnimal)
{
    switch (tipoAnimal)
    {
    case GALINHA: return "Ovo";
    case VACA:    return "Leite";
    case OVELHA:  return "La";
    }
    return "";
}

inline int precoProduto(int tipoAnimal)
{
    switch (tipoAnimal)
    {
    case GALINHA: return 25;
    case VACA:    return 50;
    case OVELHA:  return 70;
    }
    return 0;
}
