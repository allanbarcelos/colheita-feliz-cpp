#pragma once
#include "Constantes.h"
#include "Config.h"

inline int isoParaTelaX(int coluna, int linha)
{
    return (coluna - linha) * (TILE_LARGURA / 2) + g_config.offsetX;
}

inline int isoParaTelaY(int coluna, int linha)
{
    return (coluna + linha) * (TILE_ALTURA / 2) + g_config.offsetY;
}

inline float telaParaGridColuna(int mouseX, int mouseY)
{
    float relX = (float)(mouseX - g_config.offsetX);
    float relY = (float)(mouseY - g_config.offsetY);

    return (relX / (TILE_LARGURA / 2.0f) + relY / (TILE_ALTURA / 2.0f)) / 2.0f;
}

inline float telaParaGridLinha(int mouseX, int mouseY)
{
    float relX = (float)(mouseX - g_config.offsetX);
    float relY = (float)(mouseY - g_config.offsetY);

    return (relY / (TILE_ALTURA / 2.0f) - relX / (TILE_LARGURA / 2.0f)) / 2.0f;
}