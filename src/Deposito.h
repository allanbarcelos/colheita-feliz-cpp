#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>

#include "Constantes.h"
#include "Tipos.h"
#include "Desenho.h"
#include "Crops.h"
#include "GameState.h"

inline void desenharDeposito(SDL_Renderer *renderer, TTF_Font *fonte, TTF_Font *fontePequena, const CropAssets &ca, const GameState &s)
{
    float abertura = s.depositoAbertura;
    if (abertura < 0.01f)
        return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>(100 * abertura));
    SDL_Rect overlay = {0, 0, LARGURA_JANELA, ALTURA_JANELA};
    SDL_RenderFillRect(renderer, &overlay);

    float scale = 0.85f + 0.15f * abertura;
    int pw = static_cast<int>(PAINEL_LARGURA * scale);
    int ph = static_cast<int>(PAINEL_ALTURA * scale);
    int px = PAINEL_X + (PAINEL_LARGURA - pw) / 2;
    int py = PAINEL_Y + (PAINEL_ALTURA - ph) / 2;

    SDL_SetRenderDrawColor(renderer, 180, 140, 60, static_cast<Uint8>(255 * abertura));
    SDL_Rect bordaExterna = {px - 3, py - 3, pw + 6, ph + 6};
    SDL_RenderFillRect(renderer, &bordaExterna);

    SDL_SetRenderDrawColor(renderer, 54, 40, 26, static_cast<Uint8>(240 * abertura));
    SDL_Rect fundo = {px, py, pw, ph};
    SDL_RenderFillRect(renderer, &fundo);

    if (abertura < 0.6f)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        return;
    }

    SDL_SetRenderDrawColor(renderer, 140, 105, 50, 255);
    SDL_Rect barraTitulo = {PAINEL_X, PAINEL_Y, PAINEL_LARGURA, PAINEL_TITULO_ALTURA};
    SDL_RenderFillRect(renderer, &barraTitulo);

    SDL_Color corTitulo = {255, 235, 180, 255};
    desenharTexto(renderer, fonte, "Deposito", PAINEL_X + PAINEL_LARGURA / 2, PAINEL_Y + PAINEL_TITULO_ALTURA / 2, corTitulo, true);

    SDL_SetRenderDrawColor(renderer, 160, 50, 50, 255);
    SDL_Rect btnFechar = {PAINEL_X + PAINEL_LARGURA - 24, PAINEL_Y + 6, 20, 20};
    SDL_RenderFillRect(renderer, &btnFechar);
    SDL_Color corX = {255, 200, 200, 255};
    desenharTexto(renderer, fontePequena, "X", PAINEL_X + PAINEL_LARGURA - 14, PAINEL_Y + 16, corX, true);

    for (int i = 0; i < TOTAL_CROPS; i++)
    {
        int col = i % PAINEL_COLUNAS_SEMENTE;
        int lin = i / PAINEL_COLUNAS_SEMENTE;
        int sx = painelSementeSlotX(col);
        int sy = painelSementeSlotY(lin);

        int qtd = s.inventarioColhidos[i];
        Uint8 alfa = qtd > 0 ? 220 : 80;

        SDL_SetRenderDrawColor(renderer, 70, 55, 35, alfa);
        desenharRetanguloArredondado(renderer, sx, sy, SEMENTE_SLOT_LARGURA, SEMENTE_SLOT_ALTURA, 6);

        if (ca.estagios[i][TOTAL_ESTAGIOS - 1])
        {
            int padding = 8;
            int tamIcone = SEMENTE_ICONE - padding;
            int iconeX = sx + (SEMENTE_SLOT_LARGURA - tamIcone) / 2;
            int iconeY = sy + padding;
            SDL_Rect destino = {iconeX, iconeY, tamIcone, tamIcone};

            SDL_SetTextureAlphaMod(ca.estagios[i][TOTAL_ESTAGIOS - 1], alfa);
            SDL_RenderCopy(renderer, ca.estagios[i][TOTAL_ESTAGIOS - 1], nullptr, &destino);
            SDL_SetTextureAlphaMod(ca.estagios[i][TOTAL_ESTAGIOS - 1], 255);
        }

        SDL_Color corNome = {230, 220, 200, alfa};
        desenharTexto(renderer, fontePequena, TABELA_CROPS[i].nome, sx + SEMENTE_SLOT_LARGURA / 2, sy + SEMENTE_SLOT_ALTURA - 26, corNome, true);

        char buffer[32];
        snprintf(buffer, sizeof(buffer), "x%d", qtd);
        SDL_Color corQtd = {180, 220, 255, alfa};
        desenharTexto(renderer, fontePequena, buffer, sx + SEMENTE_SLOT_LARGURA / 2, sy + SEMENTE_SLOT_ALTURA - 12, corQtd, true);
    }

    int linhaAnimaisY = PAINEL_Y + PAINEL_ALTURA - 100;
    SDL_Color corLabelAnimais = {255, 235, 180, 255};
    desenharTexto(renderer, fonte, "Animais:", PAINEL_X + PAINEL_PADDING, linhaAnimaisY, corLabelAnimais, false);

    SDL_Texture *iconesAnimais[3] = {s.animalAssets.iconeOvo, s.animalAssets.iconeLeite, s.animalAssets.iconeLa};
    const char *nomesAnimais[3] = {"Ovos", "Leite", "La"};
    for (int i = 0; i < 3; i++)
    {
        int slotXa = PAINEL_X + PAINEL_PADDING + 90 + i * 130;
        int slotYa = linhaAnimaisY - 4;
        if (iconesAnimais[i])
        {
            SDL_Rect destA = {slotXa, slotYa, 28, 28};
            SDL_RenderCopy(renderer, iconesAnimais[i], nullptr, &destA);
        }
        char bufA[32];
        snprintf(bufA, sizeof(bufA), "%s x%d", nomesAnimais[i], s.inventarioProdutos[i]);
        SDL_Color corNumA = {220, 220, 220, 255};
        desenharTexto(renderer, fontePequena, bufA, slotXa + 32, slotYa + 8, corNumA, false);
    }

    int btnY = PAINEL_Y + PAINEL_ALTURA - 50;
    int btnH = 36;
    int btnW = 220;
    int btnX = PAINEL_X + (PAINEL_LARGURA - btnW) / 2;

    bool podeVender = s.valorDeposito > 0;
    if (podeVender)
        SDL_SetRenderDrawColor(renderer, 60, 160, 60, 255);
    else
        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    SDL_Rect btnVender = {btnX, btnY, btnW, btnH};
    SDL_RenderFillRect(renderer, &btnVender);

    char btnTxt[64];
    snprintf(btnTxt, sizeof(btnTxt), "Vender Tudo - %d ouro", s.valorDeposito);
    SDL_Color corBtnTxt = {255, 255, 255, 255};
    desenharTexto(renderer, fonte, btnTxt, btnX + btnW / 2, btnY + btnH / 2, corBtnTxt, true);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

inline int depositoHitTest(int mouseX, int mouseY)
{
    int fecharX = PAINEL_X + PAINEL_LARGURA - 24;
    int fecharY = PAINEL_Y + 6;
    if (mouseX >= fecharX && mouseX <= fecharX + 20 && mouseY >= fecharY && mouseY <= fecharY + 20)
        return -2;

    int btnY = PAINEL_Y + PAINEL_ALTURA - 50;
    int btnW = 220;
    int btnX = PAINEL_X + (PAINEL_LARGURA - btnW) / 2;
    if (mouseX >= btnX && mouseX <= btnX + btnW && mouseY >= btnY && mouseY <= btnY + 36)
        return 100;

    if (mouseX >= PAINEL_X && mouseX <= PAINEL_X + PAINEL_LARGURA && mouseY >= PAINEL_Y && mouseY <= PAINEL_Y + PAINEL_ALTURA)
        return -3;

    return -1;
}