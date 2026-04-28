#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>

#include "Constantes.h"
#include "Desenho.h"
#include "Crops.h"

inline void desenharHUD(SDL_Renderer *renderer, TTF_Font *fonte, int xp, int ouro, int moedasVerdes)
{
    if (!fonte)
        return;

    char buffer[64];
    SDL_Color corTexto = {255, 255, 255, 255};
    SDL_Color corVerde = {120, 220, 100, 255};

    snprintf(buffer, sizeof(buffer), "XP: %d", xp);
    desenharTexto(renderer, fonte, buffer, HUD_MARGEM, HUD_MARGEM, corTexto, false);

    snprintf(buffer, sizeof(buffer), "Ouro: %d", ouro);
    desenharTexto(renderer, fonte, buffer, HUD_MARGEM, HUD_MARGEM + 22 + HUD_ESPACO_Y, corTexto, false);

    snprintf(buffer, sizeof(buffer), "Moedas Verdes: %d", moedasVerdes);
    desenharTexto(renderer, fonte, buffer, HUD_MARGEM, HUD_MARGEM + 2 * (22 + HUD_ESPACO_Y), corVerde, false);
}
