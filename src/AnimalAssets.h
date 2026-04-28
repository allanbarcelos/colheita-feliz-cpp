#pragma once

#include <SDL2/SDL.h>
#include "Assets.h"
#include "Animal.h"

struct AnimalAssets
{
    SDL_Texture *galinhaSheet = nullptr;
    SDL_Texture *vacaSheet = nullptr;
    SDL_Texture *ovelhaSheet = nullptr;

    SDL_Texture *cachorroSheets[3] = {};
    SDL_Texture *cachorroSentados[3] = {};

    SDL_Texture *iconeOvo = nullptr;
    SDL_Texture *iconeLeite = nullptr;
    SDL_Texture *iconeLa = nullptr;
    SDL_Texture *iconeRacao = nullptr;
    SDL_Texture *iconeFome = nullptr;
};

inline AnimalAssets carregarAnimalAssets(SDL_Renderer *renderer)
{
    AnimalAssets a;
    a.galinhaSheet = carregarTextura(renderer, "assets/sprites/animals/galinha.png");
    a.vacaSheet    = carregarTextura(renderer, "assets/sprites/animals/vaca.png");
    a.ovelhaSheet  = carregarTextura(renderer, "assets/sprites/animals/ovelha.png");

    a.cachorroSheets[GOLDEN]   = carregarTextura(renderer, "assets/sprites/animals/cachorro_golden.png");
    a.cachorroSheets[DOGO]     = carregarTextura(renderer, "assets/sprites/animals/cachorro_dogo.png");
    a.cachorroSheets[KANGAL]   = carregarTextura(renderer, "assets/sprites/animals/cachorro_kangal.png");
    a.cachorroSentados[GOLDEN] = carregarTextura(renderer, "assets/sprites/animals/cachorro_golden_sentado.png");
    a.cachorroSentados[DOGO]   = carregarTextura(renderer, "assets/sprites/animals/cachorro_dogo_sentado.png");
    a.cachorroSentados[KANGAL] = carregarTextura(renderer, "assets/sprites/animals/cachorro_kangal_sentado.png");

    a.iconeOvo     = carregarTextura(renderer, "assets/sprites/items/ovo.png");
    a.iconeLeite   = carregarTextura(renderer, "assets/sprites/items/leite.png");
    a.iconeLa      = carregarTextura(renderer, "assets/sprites/items/lã.png");
    a.iconeRacao   = carregarTextura(renderer, "assets/sprites/items/ração.png");
    a.iconeFome    = nullptr;
    return a;
}

inline void liberarAnimalAssets(AnimalAssets &a)
{
    if (a.galinhaSheet) SDL_DestroyTexture(a.galinhaSheet);
    if (a.vacaSheet)    SDL_DestroyTexture(a.vacaSheet);
    if (a.ovelhaSheet)  SDL_DestroyTexture(a.ovelhaSheet);
    for (int i = 0; i < 3; i++)
    {
        if (a.cachorroSheets[i])   SDL_DestroyTexture(a.cachorroSheets[i]);
        if (a.cachorroSentados[i]) SDL_DestroyTexture(a.cachorroSentados[i]);
    }
    if (a.iconeOvo)     SDL_DestroyTexture(a.iconeOvo);
    if (a.iconeLeite)   SDL_DestroyTexture(a.iconeLeite);
    if (a.iconeLa)      SDL_DestroyTexture(a.iconeLa);
    if (a.iconeRacao)   SDL_DestroyTexture(a.iconeRacao);
    if (a.iconeFome)    SDL_DestroyTexture(a.iconeFome);
}
