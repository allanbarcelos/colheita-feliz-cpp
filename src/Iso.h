#pragma once
#include "Constantes.h"

inline int isoParaTelaX(int coluna, int linha)
{
    return (coluna - linha) * (TILE_LARGURA / 2) + OFFSET_X;
}

inline int isoParaTelaY(int coluna, int linha)
{
    return (coluna + linha) * (TILE_ALTURA / 2) + OFFSET_Y;
}

inline float telaParaGridColuna(int mouseX, int mouseY)
{
    float relX = (float)(mouseX - OFFSET_X);
    float relY = (float)(mouseY - OFFSET_Y);

    return (relX / (TILE_LARGURA / 2.0f) + relY / (TILE_ALTURA / 2.0f)) / 2.0f;
}

inline float telaParaGridLinha(int mouseX, int mouseY)
{
    float relX = (float)(mouseX - OFFSET_X);
    float relY = (float)(mouseY - OFFSET_Y);

    return (relY / (TILE_ALTURA / 2.0f) - relX / (TILE_LARGURA / 2.0f)) / 2.0f;
}