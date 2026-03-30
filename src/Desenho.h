
#pragma once
#include <SDL2/SDL.h>
#include "Constantes.h"

inline void desenharLosangoPreenchido(SDL_Renderer *renderer, int centerX, int centerY, int r, int g, int b, int altura = TILE_ALTURA, int largura = TILE_LARGURA)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    for (int dy = -altura / 2; dy <= altura / 2; dy++)
    {
        int meiaLargura = (altura / 2 - abs(dy)) * largura / altura;

        SDL_RenderDrawLine(renderer, centerX - meiaLargura, centerY + dy, centerX + meiaLargura, centerY + dy);
    }
}

inline void desenharLosangoContorno(SDL_Renderer *renderer, int centerX, int centerY, int r, int g, int b, int altura = TILE_ALTURA, int largura = TILE_LARGURA)
{
    int topoX = centerX;
    int topoY = centerY - altura / 2;

    int direitaX = centerX + largura / 2;
    int direitaY = centerY;

    int baixoX = centerX;
    int baixoY = centerY + altura / 2;

    int esquerdaX = centerX - largura / 2;
    int esquerdaY = centerY;

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    SDL_RenderDrawLine(renderer, topoX, topoY, direitaX, direitaY);

    SDL_RenderDrawLine(renderer, direitaX, direitaY, baixoX, baixoY);

    SDL_RenderDrawLine(renderer, baixoX, baixoY, esquerdaX, esquerdaY);

    SDL_RenderDrawLine(renderer, esquerdaX, esquerdaY, topoX, topoY);
}

inline void desenharTile(SDL_Renderer *renderer, SDL_Texture *textura, int centerX, int centerY, int largura = TILE_LARGURA, int altura = TILE_ALTURA)
{
    if (!textura)
        return;

    SDL_Rect destino = {
        centerX - largura / 2,
        centerY - altura / 2,
        largura,
        altura
    };

    SDL_RenderCopy(renderer, textura, nullptr, &destino);
}

inline void desenharDecoracao(SDL_Renderer *renderer, SDL_Texture *textura, int baseX, int baseY, int largura, int altura)
{
    if (!textura)
        return;

    SDL_Rect destino = {
        baseX - largura / 2,
        baseY - altura,
        largura,
        altura};

    SDL_RenderCopy(renderer, textura, nullptr, &destino);
}