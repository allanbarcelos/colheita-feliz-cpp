#pragma once

#include <SDL2/SDL.h>
#include <cmath>

#include "Constantes.h"
#include "Tipos.h"
#include "Assets.h"

inline void carregarIconesToolbar(SDL_Renderer *renderer, Toolbar &toolbar)
{
    const char *arquivos[7] = {
        "assets/sprites/icons/cursor.png",
        "assets/sprites/icons/enxada.png",
        "assets/sprites/icons/sacola.png",
        "assets/sprites/icons/regador.png",
        "assets/sprites/icons/removedor.png",
        "assets/sprites/icons/pesticida.png",
        "assets/sprites/icons/mão.png",
    };

    for (int i = 0; i < 7; i++)
    {
        toolbar.icones[i] = carregarTextura(renderer, arquivos[i]);
    }
}

inline int slotX(int indice)
{
    return TOOLBAR_X + indice * (SLOT_TAMANHO + SLOT_ESPACAMENTO);
}

inline int slotY()
{
    return TOOLBAR_Y + TOOLBAR_PADDING;
}

inline void desenharRetanguloArredondado(SDL_Renderer *renderer, int x, int y, int w, int h, int raio)
{
    SDL_Rect centro = {x + raio, y, w - 2 * raio, h};
    SDL_RenderFillRect(renderer, &centro);

    SDL_Rect esquerda = {x, y + raio, raio, h - 2 * raio};
    SDL_RenderFillRect(renderer, &esquerda);

    SDL_Rect direita = {x + w - raio, y + raio, raio, h - 2 * raio};
    SDL_RenderFillRect(renderer, &direita);

    for (int dy = -raio; dy <= raio; dy++)
    {
        int dx = static_cast<int>(sqrt(raio * raio - dy * dy));

        SDL_RenderDrawLine(renderer,
                           x + raio - dx, y + raio + dy,
                           x + raio, y + raio + dy);

        SDL_RenderDrawLine(renderer,
                           x + w - raio, y + raio + dy,
                           x + w - raio + dx, y + raio + dy);

        SDL_RenderDrawLine(renderer,
                           x + raio - dx, y + h - raio + dy,
                           x + raio, y + h - raio + dy);

        SDL_RenderDrawLine(renderer,
                           x + w - raio, y + h - raio + dy,
                           x + w - raio + dx, y + h - raio + dy);
    }
}

inline void desenharToolbar(SDL_Renderer *renderer, const Toolbar &toolbar)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 42, 31, 20, 200);

    desenharRetanguloArredondado(renderer,
                                 TOOLBAR_X - 8,
                                 TOOLBAR_Y,
                                 TOOLBAR_LARGURA + 16,
                                 TOOLBAR_ALTURA,
                                 12);

    for (int i = 0; i < TOTAL_FERRAMENTAS; i++)
    {
        int sx = slotX(i);
        int sy = slotY();

        SDL_SetRenderDrawColor(renderer, 60, 45, 30, 180);
        SDL_Rect fundoSlot = {sx, sy, SLOT_TAMANHO, SLOT_TAMANHO};
        SDL_RenderFillRect(renderer, &fundoSlot);

        if (toolbar.icones[i] != nullptr)
        {
            SDL_Rect destino = {
                sx + (SLOT_TAMANHO - ICONE_TAMANHO) / 2,
                sy + (SLOT_TAMANHO - ICONE_TAMANHO) / 2,
                ICONE_TAMANHO,
                ICONE_TAMANHO};

            SDL_RenderCopy(renderer, toolbar.icones[i], nullptr, &destino);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 120, 100, 80, 255);
            SDL_Rect fallback = {
                sx + 4, sy + 4, ICONE_TAMANHO, ICONE_TAMANHO};
            SDL_RenderFillRect(renderer, &fallback);
        }

        if (static_cast<int>(toolbar.selecionada) == i)
        {
            SDL_SetRenderDrawColor(renderer, 218, 165, 32, 255);

            SDL_Rect bordaExterna = {sx - 1, sy - 1, SLOT_TAMANHO + 2, SLOT_TAMANHO + 2};
            SDL_Rect bordaInterna = {sx - 2, sy - 2, SLOT_TAMANHO + 4, SLOT_TAMANHO + 4};
            SDL_RenderDrawRect(renderer, &bordaExterna);
            SDL_RenderDrawRect(renderer, &bordaInterna);
        }
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

inline int toolbarHitTest(int mouseX, int mouseY)
{
    int sy = slotY();

    if (mouseY < sy || mouseY > sy + SLOT_TAMANHO)
    {
        return -1;
    }

    for (int i = 0; i < TOTAL_FERRAMENTAS; i++)
    {
        int sx = slotX(i);

        if (mouseX >= sx && mouseX <= sx + SLOT_TAMANHO)
        {
            return i;
        }
    }

    return -1;
}

inline void desenharCursorFerramenta(SDL_Renderer *renderer, const Toolbar &toolbar, int mouseX, int mouseY)
{
    if (toolbar.selecionada == CURSOR)
    {
        SDL_ShowCursor(SDL_ENABLE);
        return;
    }

    SDL_ShowCursor(SDL_DISABLE);

    int indice = static_cast<int>(toolbar.selecionada);
    SDL_Texture *icone = toolbar.icones[indice];

    if (icone)
    {
        SDL_Rect destino = {
            mouseX - ICONE_TAMANHO / 2,
            mouseY - ICONE_TAMANHO / 2,
            ICONE_TAMANHO,
            ICONE_TAMANHO};

        SDL_RenderCopy(renderer, icone, nullptr, &destino);
    }
}

inline void liberarIconesToolbar(Toolbar &toolbar)
{
    for (int i = 0; i < 7; i++)
    {
        if(toolbar.icones[i] != nullptr){
            SDL_DestroyTexture(toolbar.icones[i]);
            toolbar.icones[i] = nullptr;
        }
    }
}