#pragma once

#include "Constantes.h"

inline int nivelDoJogador(int xpTotal)
{
    int nivel = 1;
    int xpRestante = xpTotal;
    while (true)
    {
        int xpNivel = XP_BASE_POR_NIVEL + nivel * XP_INCREMENTO_POR_NIVEL;
        if (xpRestante < xpNivel)
            return nivel;
        xpRestante -= xpNivel;
        nivel++;
    }
}

inline int xpAtualNoNivel(int xpTotal)
{
    int nivel = 1;
    int xpRestante = xpTotal;
    while (true)
    {
        int xpNivel = XP_BASE_POR_NIVEL + nivel * XP_INCREMENTO_POR_NIVEL;
        if (xpRestante < xpNivel)
            return xpRestante;
        xpRestante -= xpNivel;
        nivel++;
    }
}

inline int xpDoNivel(int xpTotal)
{
    int nivel = nivelDoJogador(xpTotal);
    return XP_BASE_POR_NIVEL + nivel * XP_INCREMENTO_POR_NIVEL;
}
