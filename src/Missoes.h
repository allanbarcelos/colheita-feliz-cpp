#pragma once

#include <SDL2/SDL.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "Constantes.h"

enum TipoMissao
{
    MISSAO_COLHER = 0,
    MISSAO_PLANTAR = 1,
    MISSAO_REGAR = 2,
    MISSAO_REMOVER_PRAGA = 3
};

struct Missao
{
    int tipo;
    int alvo;
    int progresso;
    bool concluida;
    bool coletada;
    int recompensaOuro;
    int recompensaVerdes;
    int recompensaXp;
    char descricao[64];
};

static constexpr Uint32 RESET_DIARIO_MS = 24u * 3600u * 1000u;

inline Missao sortearMissao(int dificuldade)
{
    Missao m = {};
    m.tipo = rand() % 4;

    if (dificuldade == 0)
    {
        m.alvo = 3 + rand() % 3;
        m.recompensaOuro = 50;
        m.recompensaVerdes = 0;
        m.recompensaXp = 0;
    }
    else if (dificuldade == 1)
    {
        m.alvo = 5 + rand() % 6;
        m.recompensaOuro = 100;
        m.recompensaVerdes = 1;
        m.recompensaXp = 0;
    }
    else
    {
        m.alvo = 10 + rand() % 11;
        m.recompensaOuro = 200;
        m.recompensaVerdes = 2;
        m.recompensaXp = 20;
    }

    const char *acoes[] = {"Colha", "Plante", "Regue", "Remova praga em"};
    const char *alvos[] = {"plantas", "sementes", "canteiros", "canteiros"};
    snprintf(m.descricao, sizeof(m.descricao), "%s %d %s",
             acoes[m.tipo], m.alvo, alvos[m.tipo]);
    return m;
}

inline void sortearMissoesDiarias(Missao missoes[3])
{
    missoes[0] = sortearMissao(0);
    missoes[1] = sortearMissao(1);
    missoes[2] = sortearMissao(2);
}

inline void incrementarProgressoMissao(Missao missoes[3], int tipo)
{
    for (int i = 0; i < 3; i++)
    {
        if (missoes[i].tipo == tipo && !missoes[i].concluida)
        {
            missoes[i].progresso++;
            if (missoes[i].progresso >= missoes[i].alvo)
                missoes[i].concluida = true;
        }
    }
}
