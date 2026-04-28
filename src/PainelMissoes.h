#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>

#include "Constantes.h"
#include "Desenho.h"
#include "Toolbar.h"
#include "HudAssets.h"
#include "Missoes.h"

static constexpr int PAINEL_MISSOES_LARGURA = 540;
static constexpr int PAINEL_MISSOES_ALTURA = 520;
static constexpr int PAINEL_MISSOES_X = (LARGURA_JANELA - PAINEL_MISSOES_LARGURA) / 2;
static constexpr int PAINEL_MISSOES_Y = (ALTURA_JANELA - PAINEL_MISSOES_ALTURA) / 2;
static constexpr int CARD_MISSAO_ALTURA = 144;
static constexpr int CARD_MISSAO_PADDING_X = 24;
static constexpr int BTN_COLETAR_LARGURA = 124;
static constexpr int BTN_COLETAR_ALTURA = 36;

inline void desenharBarraMissao(SDL_Renderer *renderer, int x, int y, int w, int h, float fracao,
                                 Uint8 r, Uint8 g, Uint8 b)
{
    if (fracao < 0.0f) fracao = 0.0f;
    if (fracao > 1.0f) fracao = 1.0f;

    int raio = h / 2;

    SDL_SetRenderDrawColor(renderer, 30, 25, 20, 220);
    desenharRetanguloArredondado(renderer, x, y, w, h, raio);

    int wPreench = static_cast<int>(w * fracao);
    if (wPreench >= raio * 2)
    {
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        desenharRetanguloArredondado(renderer, x, y, wPreench, h, raio);
    }
    else if (wPreench > 0)
    {
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_Rect preench = {x + raio - wPreench / 2, y, wPreench, h};
        SDL_RenderFillRect(renderer, &preench);
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 110);
    desenharContornoArredondado(renderer, x, y, w, h, raio);
}

inline void desenharCardMissao(SDL_Renderer *renderer, TTF_Font *fonte, TTF_Font *fontePequena,
                                const HudAssets &h, const Missao &m, int cardIndice,
                                int xBase, int yBase, Uint8 alfa)
{
    int cardX = xBase + CARD_MISSAO_PADDING_X;
    int cardY = yBase + cardIndice * CARD_MISSAO_ALTURA;
    int cardW = PAINEL_MISSOES_LARGURA - CARD_MISSAO_PADDING_X * 2;
    int cardH = CARD_MISSAO_ALTURA - 12;

    if (m.concluida && !m.coletada)
        SDL_SetRenderDrawColor(renderer, 60, 110, 50, alfa);
    else
        SDL_SetRenderDrawColor(renderer, 70, 55, 35, alfa);
    desenharRetanguloArredondado(renderer, cardX, cardY, cardW, cardH, 10);

    int padX = 18;
    SDL_Color corDesc = {255, 235, 180, alfa};
    desenharTexto(renderer, fonte, m.descricao, cardX + padX, cardY + 14, corDesc, false);

    float fracao = (m.alvo > 0) ? static_cast<float>(m.progresso) / m.alvo : 0.0f;
    desenharBarraMissao(renderer, cardX + padX, cardY + 50, 320, 18, fracao, 100, 200, 100);

    char bufProg[32];
    snprintf(bufProg, sizeof(bufProg), "%d / %d", m.progresso, m.alvo);
    SDL_Color corBranco = {255, 255, 255, alfa};
    desenharTexto(renderer, fontePequena, bufProg, cardX + padX + 160, cardY + 50 + 9, corBranco, true);

    int recompX = cardX + padX;
    int recompY = cardY + 90;
    if (m.recompensaOuro > 0)
    {
        if (h.iconeOuro)
        {
            SDL_Rect dest = {recompX, recompY, 24, 24};
            SDL_RenderCopy(renderer, h.iconeOuro, nullptr, &dest);
        }
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", m.recompensaOuro);
        desenharTexto(renderer, fontePequena, buf, recompX + 30, recompY + 4, corDesc, false);
        recompX += 88;
    }
    if (m.recompensaVerdes > 0)
    {
        if (h.iconeMoedaVerde)
        {
            SDL_Rect dest = {recompX, recompY, 24, 24};
            SDL_RenderCopy(renderer, h.iconeMoedaVerde, nullptr, &dest);
        }
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", m.recompensaVerdes);
        desenharTexto(renderer, fontePequena, buf, recompX + 30, recompY + 4, corDesc, false);
        recompX += 88;
    }
    if (m.recompensaXp > 0)
    {
        if (h.iconeXp)
        {
            SDL_Rect dest = {recompX, recompY, 24, 24};
            SDL_RenderCopy(renderer, h.iconeXp, nullptr, &dest);
        }
        char buf[16];
        snprintf(buf, sizeof(buf), "+%d", m.recompensaXp);
        desenharTexto(renderer, fontePequena, buf, recompX + 30, recompY + 4, corDesc, false);
    }

    int btnW = BTN_COLETAR_LARGURA;
    int btnH = BTN_COLETAR_ALTURA;
    int btnX = cardX + cardW - padX - btnW;
    int btnY = cardY + 86;
    bool podeColetar = m.concluida && !m.coletada;
    if (podeColetar)        SDL_SetRenderDrawColor(renderer, 80, 180, 80, alfa);
    else if (m.coletada)    SDL_SetRenderDrawColor(renderer, 60, 60, 60, alfa);
    else                    SDL_SetRenderDrawColor(renderer, 100, 80, 50, static_cast<Uint8>(alfa * 0.8f));
    desenharRetanguloArredondado(renderer, btnX, btnY, btnW, btnH, 8);

    SDL_Color corBtn = {255, 255, 255, alfa};
    const char *txtBtn = m.coletada ? "Coletado" : (podeColetar ? "Coletar" : "Em progresso");
    desenharTexto(renderer, fontePequena, txtBtn, btnX + btnW / 2, btnY + btnH / 2, corBtn, true);
}

inline void desenharPainelMissoes(SDL_Renderer *renderer, TTF_Font *fonte, TTF_Font *fontePequena,
                                   const HudAssets &h, const Missao missoes[3], float abertura)
{
    if (abertura < 0.01f) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    Uint8 overlayAlfa = static_cast<Uint8>(120 * abertura);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, overlayAlfa);
    SDL_Rect overlay = {0, 0, LARGURA_JANELA, ALTURA_JANELA};
    SDL_RenderFillRect(renderer, &overlay);

    float scale = 0.85f + 0.15f * abertura;
    int pw = static_cast<int>(PAINEL_MISSOES_LARGURA * scale);
    int ph = static_cast<int>(PAINEL_MISSOES_ALTURA * scale);
    int px = PAINEL_MISSOES_X + (PAINEL_MISSOES_LARGURA - pw) / 2;
    int py = PAINEL_MISSOES_Y + (PAINEL_MISSOES_ALTURA - ph) / 2;

    Uint8 alfaPainel = static_cast<Uint8>(255 * abertura);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>(80 * abertura));
    desenharRetanguloArredondado(renderer, px + 4, py + 6, pw, ph, 14);

    SDL_SetRenderDrawColor(renderer, 180, 140, 60, alfaPainel);
    desenharRetanguloArredondado(renderer, px - 2, py - 2, pw + 4, ph + 4, 14);

    SDL_SetRenderDrawColor(renderer, 54, 40, 26, static_cast<Uint8>(245 * abertura));
    desenharRetanguloArredondado(renderer, px, py, pw, ph, 14);

    if (abertura < 0.5f)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        return;
    }

    float conteudoAlfa = (abertura - 0.5f) * 2.0f;
    Uint8 alfaC = static_cast<Uint8>(255 * conteudoAlfa);

    SDL_SetRenderDrawColor(renderer, 140, 105, 50, alfaC);
    SDL_Rect barra = {PAINEL_MISSOES_X, PAINEL_MISSOES_Y, PAINEL_MISSOES_LARGURA, PAINEL_TITULO_ALTURA};
    SDL_RenderFillRect(renderer, &barra);

    SDL_Color corTitulo = {255, 235, 180, alfaC};
    desenharTexto(renderer, fonte, "Missoes Diarias",
                  PAINEL_MISSOES_X + PAINEL_MISSOES_LARGURA / 2,
                  PAINEL_MISSOES_Y + PAINEL_TITULO_ALTURA / 2, corTitulo, true);

    SDL_SetRenderDrawColor(renderer, 160, 50, 50, alfaC);
    desenharRetanguloArredondado(renderer, PAINEL_MISSOES_X + PAINEL_MISSOES_LARGURA - 32,
                                 PAINEL_MISSOES_Y + 8, 24, 24, 6);
    SDL_Color corX = {255, 220, 220, alfaC};
    desenharTexto(renderer, fontePequena, "X",
                  PAINEL_MISSOES_X + PAINEL_MISSOES_LARGURA - 20, PAINEL_MISSOES_Y + 20, corX, true);

    int cardsBaseY = PAINEL_MISSOES_Y + PAINEL_TITULO_ALTURA + 18;
    for (int i = 0; i < 3; i++)
        desenharCardMissao(renderer, fonte, fontePequena, h, missoes[i], i, PAINEL_MISSOES_X, cardsBaseY, alfaC);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

inline int painelMissoesHitTest(int mouseX, int mouseY, const Missao missoes[3])
{
    int fecharX = PAINEL_MISSOES_X + PAINEL_MISSOES_LARGURA - 32;
    int fecharY = PAINEL_MISSOES_Y + 8;
    if (mouseX >= fecharX && mouseX <= fecharX + 24 &&
        mouseY >= fecharY && mouseY <= fecharY + 24) return -2;

    int cardsBaseY = PAINEL_MISSOES_Y + PAINEL_TITULO_ALTURA + 18;
    int cardW = PAINEL_MISSOES_LARGURA - CARD_MISSAO_PADDING_X * 2;
    int padX = 18;
    for (int i = 0; i < 3; i++)
    {
        int cardX = PAINEL_MISSOES_X + CARD_MISSAO_PADDING_X;
        int cardY = cardsBaseY + i * CARD_MISSAO_ALTURA;
        int btnX = cardX + cardW - padX - BTN_COLETAR_LARGURA;
        int btnY = cardY + 86;
        if (mouseX >= btnX && mouseX <= btnX + BTN_COLETAR_LARGURA &&
            mouseY >= btnY && mouseY <= btnY + BTN_COLETAR_ALTURA &&
            missoes[i].concluida && !missoes[i].coletada)
            return i;
    }

    if (mouseX >= PAINEL_MISSOES_X && mouseX <= PAINEL_MISSOES_X + PAINEL_MISSOES_LARGURA &&
        mouseY >= PAINEL_MISSOES_Y && mouseY <= PAINEL_MISSOES_Y + PAINEL_MISSOES_ALTURA)
        return -3;

    return -1;
}
