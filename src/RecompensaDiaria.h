#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "Constantes.h"
#include "Desenho.h"
#include "Crops.h"
#include "Toolbar.h"
#include "HudAssets.h"

static constexpr int CAIXA_RECOMPENSA_X = 560;
static constexpr int CAIXA_RECOMPENSA_Y = 28;
static constexpr int CAIXA_RECOMPENSA_TAM = 64;

inline void desenharCaixaRecompensa(SDL_Renderer *renderer, const HudAssets &h,
                                     bool disponivel, Uint32 tempoJogoMs)
{
    if (!disponivel) return;

    float t = (tempoJogoMs % 1000) / 1000.0f;
    int offsetY = static_cast<int>(4.0f * sinf(t * 6.2832f));

    int x = CAIXA_RECOMPENSA_X;
    int y = CAIXA_RECOMPENSA_Y + offsetY;

    if (h.caixaPresente)
    {
        SDL_Rect dest = {x, y, CAIXA_RECOMPENSA_TAM, CAIXA_RECOMPENSA_TAM};
        SDL_RenderCopy(renderer, h.caixaPresente, nullptr, &dest);
    }
    else
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 200, 50, 60, 240);
        desenharRetanguloArredondado(renderer, x, y, CAIXA_RECOMPENSA_TAM, CAIXA_RECOMPENSA_TAM, 8);
        SDL_SetRenderDrawColor(renderer, 255, 220, 100, 255);
        desenharContornoArredondado(renderer, x, y, CAIXA_RECOMPENSA_TAM, CAIXA_RECOMPENSA_TAM, 8);
        SDL_Rect fitaH = {x, y + CAIXA_RECOMPENSA_TAM / 2 - 4, CAIXA_RECOMPENSA_TAM, 8};
        SDL_RenderFillRect(renderer, &fitaH);
        SDL_Rect fitaV = {x + CAIXA_RECOMPENSA_TAM / 2 - 4, y, 8, CAIXA_RECOMPENSA_TAM};
        SDL_RenderFillRect(renderer, &fitaV);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
}

inline bool caixaRecompensaClicada(int mouseX, int mouseY, bool disponivel)
{
    if (!disponivel) return false;
    return mouseX >= CAIXA_RECOMPENSA_X &&
           mouseX <= CAIXA_RECOMPENSA_X + CAIXA_RECOMPENSA_TAM &&
           mouseY >= CAIXA_RECOMPENSA_Y &&
           mouseY <= CAIXA_RECOMPENSA_Y + CAIXA_RECOMPENSA_TAM;
}

inline void sortearRecompensaDiaria(int *ouro, int *moedasVerdes, int *inventarioSementes,
                                     char *mensagemFora, int mensagemCap)
{
    int sorteio = rand() % 100;

    if (sorteio < 50)
    {
        *ouro += 100;
        snprintf(mensagemFora, mensagemCap, "Recompensa diaria: +100 ouro!");
    }
    else if (sorteio < 80)
    {
        *ouro += 50;
        *moedasVerdes += 1;
        snprintf(mensagemFora, mensagemCap, "Recompensa: +50 ouro, +1 moeda verde!");
    }
    else if (sorteio < 95)
    {
        int sementesIniciais[] = {NABO, CENOURA, TOMATE, REPOLHO};
        int idx = sementesIniciais[rand() % 4];
        inventarioSementes[idx] += 3;
        snprintf(mensagemFora, mensagemCap, "Recompensa: +3 %s!", TABELA_CROPS[idx].nome);
    }
    else
    {
        *moedasVerdes += 3;
        snprintf(mensagemFora, mensagemCap, "JACKPOT! +3 moedas verdes!");
    }
}
