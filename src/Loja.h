#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>

#include "Constantes.h"
#include "Tipos.h"
#include "Desenho.h"
#include "Crops.h"
#include "Player.h"
#include "PainelBase.h"
#include "Decoracao.h"
#include "GameState.h"

enum TabLoja
{
    TAB_SEMENTES = 0,
    TAB_RACOES = 1,
    TAB_DECORACAO = 2,
    TAB_FERTILIZANTES = 3,
    TAB_FLORES = 4,
    TAB_ANIMAIS = 5,
    TAB_VIP = 6
};

inline const char *labelTabLoja(int idx)
{
    switch (idx)
    {
    case 0: return "Sementes";
    case 1: return "Rações";
    case 2: return "Decoração";
    case 3: return "Fertilizantes";
    case 4: return "Flores";
    case 5: return "Animais";
    case 6: return "VIP";
    }
    return "";
}

inline void desenharCardsDecoracao(SDL_Renderer *renderer, TTF_Font *fonte, TTF_Font *fontePequena,
                                    const HudAssets &h, const GameState &s, int contentY)
{
    for (int i = 0; i < TOTAL_DECORACOES; i++)
    {
        int col = i % 5;
        int lin = i / 5;
        int slotX = LOJA_X + 16 + col * (204 + 12);
        int slotY = contentY + lin * (280 + 12);

        const ItemDecoracao &d = TABELA_DECORACOES[i];
        int nivelAtual = nivelDoJogador(s.xp);
        int requerLv = (nivelAtual < d.requerLv) ? d.requerLv : 0;

        bool podeComprar = (s.ouro >= d.precoOuro) && (s.moedasVerdes >= d.precoVerdes) && requerLv == 0;

        char info[64];
        if (d.precoVerdes > 0 && d.precoOuro > 0)
            snprintf(info, sizeof(info), "+ %d verdes", d.precoVerdes);
        else if (d.precoVerdes > 0)
            snprintf(info, sizeof(info), "Tenho: %d", s.inventarioDecoracoes[i]);
        else
            snprintf(info, sizeof(info), "Tenho: %d", s.inventarioDecoracoes[i]);

        SDL_Texture *iconeMoeda = (d.precoOuro > 0) ? h.iconeOuro : h.iconeMoedaVerde;
        int precoMostrar = (d.precoOuro > 0) ? d.precoOuro : d.precoVerdes;

        bool hover = (s.mouseX >= slotX && s.mouseX <= slotX + 204 &&
                      s.mouseY >= slotY && s.mouseY <= slotY + 280);
        desenharCardItem(renderer, fonte, fontePequena,
                          spritePreview(s.decoracaoAssets, i), d.nome, info,
                          precoMostrar, iconeMoeda, podeComprar, requerLv,
                          slotX, slotY, "Comprar", hover);
    }
}

inline void desenharLoja(SDL_Renderer *renderer, TTF_Font *fonte, TTF_Font *fontePequena,
                          const CropAssets &ca, const HudAssets &h, const GameState &s)
{
    if (s.lojaAbertura < 0.01f) return;

    desenharPainelBase(renderer, fonte, fontePequena, "Loja da Fazenda",
                       LOJA_X, LOJA_Y, LOJA_LARGURA, LOJA_ALTURA, s.lojaAbertura);

    if (s.lojaAbertura < 0.6f) return;

    const char *labels[TOTAL_TABS_LOJA];
    bool habilitadas[TOTAL_TABS_LOJA] = {true, true, true, false, false, false, false};
    for (int i = 0; i < TOTAL_TABS_LOJA; i++) labels[i] = labelTabLoja(i);
    desenharTabs(renderer, fontePequena, labels, habilitadas, TOTAL_TABS_LOJA, s.lojaTabAtiva,
                 LOJA_X, LOJA_Y + 56, LOJA_LARGURA);

    int contentY = LOJA_Y + 56 + 50 + 16;

    if (s.lojaTabAtiva == TAB_SEMENTES)
    {
        for (int i = 0; i < 10 && i < TOTAL_CROPS; i++)
        {
            int col = i % 5;
            int lin = i / 5;
            int slotX = LOJA_X + 16 + col * (204 + 12);
            int slotY = contentY + lin * (280 + 12);

            const DadosCrop &d = TABELA_CROPS[i];
            int nivelAtual = nivelDoJogador(s.xp);
            int requerLv = (nivelAtual < d.nivelDesbloqueio) ? d.nivelDesbloqueio : 0;
            bool podeComprar = (s.ouro >= d.precoCompra) && requerLv == 0;

            char rendeBuf[32];
            snprintf(rendeBuf, sizeof(rendeBuf), "Rende: %d", d.precoVenda);
            bool hover = (s.mouseX >= slotX && s.mouseX <= slotX + 204 &&
                          s.mouseY >= slotY && s.mouseY <= slotY + 280);
            desenharCardItem(renderer, fonte, fontePequena,
                              ca.sementes[i], d.nome, rendeBuf,
                              d.precoCompra, h.iconeOuro,
                              podeComprar, requerLv,
                              slotX, slotY, "Comprar", hover);
        }
    }
    else if (s.lojaTabAtiva == TAB_DECORACAO)
    {
        desenharCardsDecoracao(renderer, fonte, fontePequena, h, s, contentY);
    }
    else if (s.lojaTabAtiva == TAB_RACOES)
    {
        int slotX = LOJA_X + (LOJA_LARGURA - 204) / 2;
        int slotY = contentY;
        bool podeComprar = (s.ouro >= PRECO_RACAO);
        char buf[32];
        snprintf(buf, sizeof(buf), "Tenho: %d", s.inventarioRacao);
        bool hover = (s.mouseX >= slotX && s.mouseX <= slotX + 204 &&
                      s.mouseY >= slotY && s.mouseY <= slotY + 280);
        desenharCardItem(renderer, fonte, fontePequena,
                          s.animalAssets.iconeRacao, "Ração Animal", buf,
                          PRECO_RACAO, h.iconeOuro, podeComprar, 0, slotX, slotY, "Comprar", hover);
    }
    else
    {
        SDL_Color cor = COR_TEXTO_DARK;
        desenharTexto(renderer, fonte, "Em breve",
                      LOJA_X + LOJA_LARGURA / 2, LOJA_Y + LOJA_ALTURA / 2,
                      cor, true);
    }
}

inline int lojaHitTest(int mouseX, int mouseY, const GameState &s)
{
    if (painelBaseFechou(mouseX, mouseY, LOJA_X, LOJA_Y, LOJA_LARGURA)) return -2;

    int tabClicada = tabsHitTest(mouseX, mouseY, TOTAL_TABS_LOJA,
                                  LOJA_X, LOJA_Y + 56, LOJA_LARGURA);
    if (tabClicada >= 0) return 100 + tabClicada;

    int contentY = LOJA_Y + 56 + 50 + 16;

    if (s.lojaTabAtiva == TAB_SEMENTES)
    {
        for (int i = 0; i < 10 && i < TOTAL_CROPS; i++)
        {
            int col = i % 5;
            int lin = i / 5;
            int slotX = LOJA_X + 16 + col * (204 + 12);
            int slotY = contentY + lin * (280 + 12);
            if (cardCompraClicado(mouseX, mouseY, slotX, slotY))
                return 200 + i;
        }
    }
    else if (s.lojaTabAtiva == TAB_DECORACAO)
    {
        for (int i = 0; i < TOTAL_DECORACOES; i++)
        {
            int col = i % 5;
            int lin = i / 5;
            int slotX = LOJA_X + 16 + col * (204 + 12);
            int slotY = contentY + lin * (280 + 12);
            if (cardCompraClicado(mouseX, mouseY, slotX, slotY))
                return 300 + i;
        }
    }
    else if (s.lojaTabAtiva == TAB_RACOES)
    {
        int slotX = LOJA_X + (LOJA_LARGURA - 204) / 2;
        int slotY = contentY;
        if (cardCompraClicado(mouseX, mouseY, slotX, slotY))
            return 999;
    }

    if (mouseX >= LOJA_X && mouseX <= LOJA_X + LOJA_LARGURA &&
        mouseY >= LOJA_Y && mouseY <= LOJA_Y + LOJA_ALTURA)
        return -3;

    return -1;
}

