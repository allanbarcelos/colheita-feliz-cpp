#pragma once

#include <SDL2/SDL.h>
#include "Assets.h"

struct HudAssets
{
    SDL_Texture *painelStatus = nullptr;

    SDL_Texture *iconeOuro = nullptr;
    SDL_Texture *iconeMoedaVerde = nullptr;
    SDL_Texture *iconeCoracao = nullptr;
    SDL_Texture *iconeXp = nullptr;

    SDL_Texture *avatarMasculino = nullptr;
    SDL_Texture *avatarFeminino = nullptr;

    SDL_Texture *botoes[5] = {};
};

inline const char *nomeBotaoHud(int indice)
{
    switch (indice)
    {
    case 0: return "Armazem";
    case 1: return "Loja";
    case 2: return "Amigos";
    case 3: return "Ranking";
    case 4: return "Avisos";
    }
    return "";
}

inline HudAssets carregarHudAssets(SDL_Renderer *renderer)
{
    HudAssets h;

    h.painelStatus     = carregarTextura(renderer, "assets/sprites/icons/hud/esquerdo/painel_status.png");

    h.iconeOuro        = carregarTextura(renderer, "assets/sprites/icons/hud/esquerdo/ouro.png");
    h.iconeMoedaVerde  = carregarTextura(renderer, "assets/sprites/icons/hud/esquerdo/moeda_verde.png");
    h.iconeCoracao     = carregarTextura(renderer, "assets/sprites/icons/hud/esquerdo/coracao.png");
    h.iconeXp          = carregarTextura(renderer, "assets/sprites/icons/hud/esquerdo/xp.png");

    h.avatarMasculino  = carregarTextura(renderer, "assets/sprites/icons/hud/esquerdo/avatar_masculino.png");
    h.avatarFeminino   = carregarTextura(renderer, "assets/sprites/icons/hud/esquerdo/avatar_feminino.png");

    h.botoes[0] = carregarTextura(renderer, "assets/sprites/icons/hud/direito/armazen.png");
    h.botoes[1] = carregarTextura(renderer, "assets/sprites/icons/hud/direito/loja.png");
    h.botoes[2] = carregarTextura(renderer, "assets/sprites/icons/hud/direito/amigos.png");
    h.botoes[3] = carregarTextura(renderer, "assets/sprites/icons/hud/direito/ranking.png");
    h.botoes[4] = carregarTextura(renderer, "assets/sprites/icons/hud/direito/avisos.png");

    return h;
}

inline void liberarHudAssets(HudAssets &h)
{
    if (h.painelStatus)     SDL_DestroyTexture(h.painelStatus);
    if (h.iconeOuro)        SDL_DestroyTexture(h.iconeOuro);
    if (h.iconeMoedaVerde)  SDL_DestroyTexture(h.iconeMoedaVerde);
    if (h.iconeCoracao)     SDL_DestroyTexture(h.iconeCoracao);
    if (h.iconeXp)          SDL_DestroyTexture(h.iconeXp);
    if (h.avatarMasculino)  SDL_DestroyTexture(h.avatarMasculino);
    if (h.avatarFeminino)   SDL_DestroyTexture(h.avatarFeminino);
    for (int i = 0; i < 5; i++)
        if (h.botoes[i]) SDL_DestroyTexture(h.botoes[i]);
}
