#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include "Constantes.h"
#include "Desenho.h"
#include "Toolbar.h"
#include "PainelBase.h"
#include "Sons.h"

static constexpr int SETTINGS_LARGURA = 480;
static constexpr int SETTINGS_ALTURA = 380;
static constexpr int SETTINGS_X = (LARGURA_JANELA - SETTINGS_LARGURA) / 2;
static constexpr int SETTINGS_Y = (ALTURA_JANELA - SETTINGS_ALTURA) / 2;

inline void desenharPainelSettings(SDL_Renderer *renderer, TTF_Font *fonte, TTF_Font *fontePequena,
                                    const Sons &s, float abertura)
{
    if (abertura < 0.01f) return;

    desenharPainelBase(renderer, fonte, fontePequena, "Settings",
                       SETTINGS_X, SETTINGS_Y, SETTINGS_LARGURA, SETTINGS_ALTURA, abertura);

    if (abertura < 0.6f) return;

    int contentY = SETTINGS_Y + 80;
    int sliderX = SETTINGS_X + 40;
    int sliderW = SETTINGS_LARGURA - 80;

    desenharTexto(renderer, fonte, "Música", sliderX, contentY, COR_TEXTO_DARK, false);
    char bufVal[16];
    snprintf(bufVal, sizeof(bufVal), "%d%%", (s.volumeMusica * 100) / 128);
    desenharTexto(renderer, fontePequena, bufVal, sliderX + sliderW - 30, contentY + 4, COR_TEXTO_DARK, false);

    int trilhaY = contentY + 36;
    SDL_SetRenderDrawColor(renderer, 200, 170, 100, 255);
    desenharRetanguloArredondado(renderer, sliderX, trilhaY, sliderW, 10, 5);
    int knobX = sliderX + (s.volumeMusica * sliderW) / 128;
    SDL_SetRenderDrawColor(renderer, COR_BOTAO_COMPRAR.r, COR_BOTAO_COMPRAR.g, COR_BOTAO_COMPRAR.b, 255);
    desenharRetanguloArredondado(renderer, knobX - 10, trilhaY - 6, 20, 22, 8);

    int sfxY = contentY + 90;
    desenharTexto(renderer, fonte, "SFX", sliderX, sfxY, COR_TEXTO_DARK, false);
    snprintf(bufVal, sizeof(bufVal), "%d%%", (s.volumeSfx * 100) / 128);
    desenharTexto(renderer, fontePequena, bufVal, sliderX + sliderW - 30, sfxY + 4, COR_TEXTO_DARK, false);

    int sfxTrilhaY = sfxY + 36;
    SDL_SetRenderDrawColor(renderer, 200, 170, 100, 255);
    desenharRetanguloArredondado(renderer, sliderX, sfxTrilhaY, sliderW, 10, 5);
    int sfxKnobX = sliderX + (s.volumeSfx * sliderW) / 128;
    SDL_SetRenderDrawColor(renderer, COR_BOTAO_COMPRAR.r, COR_BOTAO_COMPRAR.g, COR_BOTAO_COMPRAR.b, 255);
    desenharRetanguloArredondado(renderer, sfxKnobX - 10, sfxTrilhaY - 6, 20, 22, 8);

    int muteY = sfxY + 90;
    SDL_Rect cb = {sliderX, muteY, 28, 28};
    SDL_SetRenderDrawColor(renderer, 80, 50, 30, 255);
    desenharRetanguloArredondado(renderer, cb.x, cb.y, cb.w, cb.h, 4);
    if (s.mudo)
    {
        SDL_SetRenderDrawColor(renderer, COR_BOTAO_COMPRAR.r, COR_BOTAO_COMPRAR.g, COR_BOTAO_COMPRAR.b, 255);
        desenharRetanguloArredondado(renderer, cb.x + 4, cb.y + 4, cb.w - 8, cb.h - 8, 2);
    }
    desenharTexto(renderer, fonte, "Mudo (silencia tudo)", sliderX + 40, muteY + 14, COR_TEXTO_DARK, false);
}

inline int painelSettingsHitTest(int mouseX, int mouseY, int &valorRetorno)
{
    if (painelBaseFechou(mouseX, mouseY, SETTINGS_X, SETTINGS_Y, SETTINGS_LARGURA))
        return -2;

    int t = CLICK_TOLERANCIA;
    int contentY = SETTINGS_Y + 80;
    int sliderX = SETTINGS_X + 40;
    int sliderW = SETTINGS_LARGURA - 80;

    int trilhaY = contentY + 36;
    if (mouseY >= trilhaY - 12 - t && mouseY <= trilhaY + 18 + t &&
        mouseX >= sliderX - t && mouseX <= sliderX + sliderW + t)
    {
        valorRetorno = ((mouseX - sliderX) * 128) / sliderW;
        if (valorRetorno < 0) valorRetorno = 0;
        if (valorRetorno > 128) valorRetorno = 128;
        return 1;
    }

    int sfxY = contentY + 90;
    int sfxTrilhaY = sfxY + 36;
    if (mouseY >= sfxTrilhaY - 12 - t && mouseY <= sfxTrilhaY + 18 + t &&
        mouseX >= sliderX - t && mouseX <= sliderX + sliderW + t)
    {
        valorRetorno = ((mouseX - sliderX) * 128) / sliderW;
        if (valorRetorno < 0) valorRetorno = 0;
        if (valorRetorno > 128) valorRetorno = 128;
        return 2;
    }

    int muteY = sfxY + 90;
    if (mouseX >= sliderX - t && mouseX <= sliderX + 28 + t &&
        mouseY >= muteY - t && mouseY <= muteY + 28 + t)
        return 3;

    if (mouseX >= SETTINGS_X && mouseX <= SETTINGS_X + SETTINGS_LARGURA &&
        mouseY >= SETTINGS_Y && mouseY <= SETTINGS_Y + SETTINGS_ALTURA)
        return -3;

    return -1;
}
