
#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
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

inline void desenharLosangoPreenchidoAlfa(SDL_Renderer *renderer, int centerX, int centerY, int r, int g, int b, int a, int altura = TILE_ALTURA, int largura = TILE_LARGURA)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    for (int dy = -altura / 2; dy <= altura / 2; dy++)
    {
        int meiaLargura = (altura / 2 - abs(dy)) * largura / altura;
        SDL_RenderDrawLine(renderer, centerX - meiaLargura, centerY + dy, centerX + meiaLargura, centerY + dy);
    }
}

inline void desenharElipsePreenchidaAlfa(SDL_Renderer *renderer, int cx, int cy, int rx, int ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    if (rx <= 0 || ry <= 0)
        return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    for (int dy = -ry; dy <= ry; dy++)
    {
        float t = static_cast<float>(dy) / static_cast<float>(ry);
        int hw = static_cast<int>(rx * std::sqrt(1.0f - t * t));
        SDL_RenderDrawLine(renderer, cx - hw, cy + dy, cx + hw, cy + dy);
    }
}

inline void desenharSombraElipseSuave(SDL_Renderer *renderer, int cx, int cy, int rx, int ry, Uint8 alfaCentro)
{
    desenharElipsePreenchidaAlfa(renderer, cx, cy + 2, rx + 6, ry + 4, 0, 0, 0, alfaCentro / 4);
    desenharElipsePreenchidaAlfa(renderer, cx, cy + 1, rx + 3, ry + 2, 0, 0, 0, alfaCentro / 2);
    desenharElipsePreenchidaAlfa(renderer, cx, cy, rx, ry, 0, 0, 0, alfaCentro);
}

inline void desenharSombraTile(SDL_Renderer *renderer, int centerX, int centerY)
{
    desenharLosangoPreenchidoAlfa(renderer, centerX + 3, centerY + 5, 0, 0, 0, 55, TILE_ALTURA, TILE_LARGURA);
}

inline void desenharSombraSprite(SDL_Renderer *renderer, int baseX, int baseY, int largura)
{
    int rx = largura / 3;
    int ry = rx / 3 + 2;
    desenharSombraElipseSuave(renderer, baseX, baseY, rx, ry, 100);
}

inline void desenharTexto(SDL_Renderer *renderer, TTF_Font *fonte, const char *texto, int x, int y, SDL_Color cor, bool centralizar = false)
{
    if (!fonte || !texto)
        return;

    SDL_Surface *superficie = TTF_RenderUTF8_Blended(fonte, texto, cor);
    if (!superficie)
        return;

    SDL_Texture *textura = SDL_CreateTextureFromSurface(renderer, superficie);
    int w = superficie->w;
    int h = superficie->h;
    SDL_FreeSurface(superficie);

    if (!textura)
        return;

    SDL_SetTextureBlendMode(textura, SDL_BLENDMODE_BLEND);

    SDL_Rect destino;
    if (centralizar)
    {
        destino = {x - w / 2, y - h / 2, w, h};
    }
    else
    {
        destino = {x, y, w, h};
    }

    SDL_RenderCopy(renderer, textura, nullptr, &destino);
    SDL_DestroyTexture(textura);
}

inline void desenharTextoComContorno(SDL_Renderer *renderer, TTF_Font *fonte, const char *texto, int x, int y, SDL_Color cor, bool centralizar = false)
{
    SDL_Color sombra = {0, 0, 0, 200};
    desenharTexto(renderer, fonte, texto, x - 1, y,     sombra, centralizar);
    desenharTexto(renderer, fonte, texto, x + 1, y,     sombra, centralizar);
    desenharTexto(renderer, fonte, texto, x,     y - 1, sombra, centralizar);
    desenharTexto(renderer, fonte, texto, x,     y + 1, sombra, centralizar);
    desenharTexto(renderer, fonte, texto, x + 1, y + 1, sombra, centralizar);
    desenharTexto(renderer, fonte, texto, x, y, cor, centralizar);
}
