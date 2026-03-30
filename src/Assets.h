#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>

#include "Tipos.h"

inline SDL_Texture *carregarTextura(SDL_Renderer *renderer, const char *caminho)
{

    SDL_Surface *superficie = IMG_Load(caminho);

    if (!superficie)
    {
        std::cerr << "Erro ao carregar imagem: " << caminho << ":" << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture *textura = SDL_CreateTextureFromSurface(renderer, superficie);

    SDL_FreeSurface(superficie);

    if (!textura)
    {
        std::cerr << "Erro ao criar textura de: " << caminho << ":" << SDL_GetError() << std::endl;
        return nullptr;
    }

    return textura;
}

inline Assets carregarTodosAssets(SDL_Renderer *renderer)
{
    Assets a;

    a.background = carregarTextura(renderer, "assets/background/farm.jpg");

    a.tileGrama = carregarTextura(renderer, "assets/sprites/tile_grama.png");
    a.tileGramaEscuro = carregarTextura(renderer, "assets/sprites/tile_grama_escuro.png");
    a.tileTerra = carregarTextura(renderer, "assets/sprites/tile_terra.png");
    a.tileTerraSeca = carregarTextura(renderer, "assets/sprites/tile_terra_seca.png");
    a.tileTerraVermelha = carregarTextura(renderer, "assets/sprites/tile_terra_vermelha.png");
    a.tileBloqueado = carregarTextura(renderer, "assets/sprites/tile_bloqueado.png");
    a.tileRestos = carregarTextura(renderer, "assets/sprites/tile_restos.png");

    a.casa = carregarTextura(renderer, "assets/sprites/casa.png");
    a.casaCachorro = carregarTextura(renderer, "assets/sprites/casinha_cachorro.png");
    a.cerca = carregarTextura(renderer, "assets/sprites/cerca.png");

    return a;
}

inline void liberarAssets(Assets &a)
{
    if (a.background)
        SDL_DestroyTexture(a.background);
    if (a.tileGrama)
        SDL_DestroyTexture(a.tileGrama);
    if (a.tileGramaEscuro)
        SDL_DestroyTexture(a.tileGramaEscuro);
    if (a.tileTerra)
        SDL_DestroyTexture(a.tileTerra);
    if (a.tileTerraSeca)
        SDL_DestroyTexture(a.tileTerraSeca);
    if (a.tileTerraVermelha)
        SDL_DestroyTexture(a.tileTerraVermelha);
    if (a.tileBloqueado)
        SDL_DestroyTexture(a.tileBloqueado);
    if (a.tileRestos)
        SDL_DestroyTexture(a.tileRestos);
    if (a.casa)
        SDL_DestroyTexture(a.casa);
    if (a.casaCachorro)
        SDL_DestroyTexture(a.casaCachorro);
    if (a.cerca)
        SDL_DestroyTexture(a.cerca);
}