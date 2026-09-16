#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <string>

#include "Tipos.h"

inline bool arquivoExiste(const char *caminho)
{
    SDL_RWops *rw = SDL_RWFromFile(caminho, "rb");
    if (!rw)
        return false;
    SDL_RWclose(rw);
    return true;
}

inline std::string resolverCaminho(const char *relativo)
{
    if (!relativo || !relativo[0])
        return {};

    if (arquivoExiste(relativo))
        return relativo;

    char *base = SDL_GetBasePath();
    if (base)
    {
        std::string aoLadoDoExe = std::string(base) + relativo;
        if (arquivoExiste(aoLadoDoExe.c_str()))
        {
            SDL_free(base);
            return aoLadoDoExe;
        }

        std::string umNivelAcima = std::string(base) + "../" + relativo;
        if (arquivoExiste(umNivelAcima.c_str()))
        {
            SDL_free(base);
            return umNivelAcima;
        }

        SDL_free(base);
        return aoLadoDoExe;
    }

    return relativo;
}

inline std::string resolverCaminhoSeExistir(const char *relativo)
{
    if (!relativo || !relativo[0])
        return {};

    if (arquivoExiste(relativo))
        return relativo;

    char *base = SDL_GetBasePath();
    if (!base)
        return {};

    std::string aoLadoDoExe = std::string(base) + relativo;
    if (arquivoExiste(aoLadoDoExe.c_str()))
    {
        SDL_free(base);
        return aoLadoDoExe;
    }

    std::string umNivelAcima = std::string(base) + "../" + relativo;
    SDL_free(base);
    if (arquivoExiste(umNivelAcima.c_str()))
        return umNivelAcima;

    return {};
}

inline SDL_Texture *carregarTextura(SDL_Renderer *renderer, const char *caminho)
{
    std::string resolvido = resolverCaminho(caminho);
    SDL_Surface *superficie = IMG_Load(resolvido.c_str());

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

inline SDL_Texture *carregarTexturaOpcional(SDL_Renderer *renderer, const char *caminho)
{
    if (resolverCaminhoSeExistir(caminho).empty())
        return nullptr;
    return carregarTextura(renderer, caminho);
}

inline Assets carregarTodosAssets(SDL_Renderer *renderer)
{
    Assets a;

    a.background = carregarTextura(renderer, "assets/background/farm.jpg");

    a.tileGrama = carregarTextura(renderer, "assets/sprites/tiles/tile_grama.png");
    a.tileGramaEscuro = carregarTextura(renderer, "assets/sprites/tiles/tile_grama_escuro.png");
    a.tileTerra = carregarTextura(renderer, "assets/sprites/tiles/tile_terra.png");
    a.tileTerraSeca = carregarTextura(renderer, "assets/sprites/tiles/tile_terra_seca.png");
    a.tileTerraVermelha = carregarTextura(renderer, "assets/sprites/tiles/tile_terra_vermelha.png");
    a.tileBloqueado = carregarTextura(renderer, "assets/sprites/tiles/tile_bloqueado.png");
    a.tileRestos = carregarTextura(renderer, "assets/sprites/tiles/tile_restos.png");

    a.casa = carregarTextura(renderer, "assets/sprites/buildings/casa.png");
    a.casaCachorro = carregarTextura(renderer, "assets/sprites/buildings/casinha_cachorro.png");
    a.cerca = carregarTextura(renderer, "assets/sprites/buildings/cerca.png");

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
