#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>

#include "Constantes.h"
#include "Tipos.h"
#include "Desenho.h"
#include "Crops.h"
#include "PainelBase.h"
#include "GameState.h"

inline void desenharDeposito(SDL_Renderer *renderer, TTF_Font *fonte, TTF_Font *fontePequena,
                              const CropAssets &ca, const HudAssets &h, const GameState &s)
{
    if (s.depositoAbertura < 0.01f) return;

    desenharPainelBase(renderer, fonte, fontePequena, "Armazem da Fazenda",
                       LOJA_X, LOJA_Y, LOJA_LARGURA, LOJA_ALTURA, s.depositoAbertura);

    if (s.depositoAbertura < 0.6f) return;

    int contentY = LOJA_Y + 56 + 16;

    for (int i = 0; i < 10 && i < TOTAL_CROPS; i++)
    {
        int col = i % 5;
        int lin = i / 5;
        int slotX = LOJA_X + 16 + col * (204 + 12);
        int slotY = contentY + lin * (280 + 12);

        const DadosCrop &d = TABELA_CROPS[i];
        char info[32];
        snprintf(info, sizeof(info), "Tenho: %d", s.inventarioColhidos[i]);
        desenharCardItem(renderer, fonte, fontePequena,
                          ca.estagios[i][TOTAL_ESTAGIOS - 1], d.nome, info,
                          d.precoVenda, h.iconeOuro,
                          false, 0,
                          slotX, slotY, "Maduro");
    }

    int btnW = 460;
    int btnX = LOJA_X + (LOJA_LARGURA - btnW) / 2;
    int btnY = LOJA_Y + LOJA_ALTURA - 80;
    int btnH = 56;
    bool podeVender = s.valorDeposito > 0;
    SDL_SetRenderDrawColor(renderer,
                            podeVender ? COR_BOTAO_COMPRAR.r : 130,
                            podeVender ? COR_BOTAO_COMPRAR.g : 130,
                            podeVender ? COR_BOTAO_COMPRAR.b : 130, 255);
    desenharRetanguloArredondado(renderer, btnX, btnY, btnW, btnH, 12);

    char buf[64];
    snprintf(buf, sizeof(buf), "Vender Tudo - %d ouro", s.valorDeposito);
    SDL_Color corBtnTxt = {255, 255, 255, 255};
    desenharTexto(renderer, fonte, buf, btnX + btnW / 2, btnY + btnH / 2, corBtnTxt, true);
}

inline int depositoHitTest(int mouseX, int mouseY)
{
    if (painelBaseFechou(mouseX, mouseY, LOJA_X, LOJA_Y, LOJA_LARGURA)) return -2;

    int btnW = 460;
    int btnX = LOJA_X + (LOJA_LARGURA - btnW) / 2;
    int btnY = LOJA_Y + LOJA_ALTURA - 80;
    int btnH = 56;
    int t = CLICK_TOLERANCIA;
    if (mouseX >= btnX - t && mouseX <= btnX + btnW + t &&
        mouseY >= btnY - t && mouseY <= btnY + btnH + t)
        return 100;

    if (mouseX >= LOJA_X && mouseX <= LOJA_X + LOJA_LARGURA &&
        mouseY >= LOJA_Y && mouseY <= LOJA_Y + LOJA_ALTURA)
        return -3;

    return -1;
}
