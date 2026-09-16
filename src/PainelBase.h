#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include "Constantes.h"
#include "Desenho.h"
#include "Toolbar.h"

static const SDL_Color COR_HEADER_BG     = {110,  80,  50, 255};
static const SDL_Color COR_HEADER_TITULO = {255, 245, 220, 255};
static const SDL_Color COR_BODY_BG       = {245, 230, 195, 255};
static const SDL_Color COR_CARD_BG       = {255, 244, 220, 255};
static const SDL_Color COR_CARD_BORDER   = {200, 170, 100, 255};
static const SDL_Color COR_BOTAO_COMPRAR = { 76, 175,  80, 255};
static const SDL_Color COR_BADGE_LV      = {211,  47,  47, 255};
static const SDL_Color COR_TEXTO_PRECO   = {255, 165,   0, 255};
static const SDL_Color COR_TEXTO_DARK    = { 80,  50,  30, 255};

inline void desenharPainelBase(SDL_Renderer *renderer, TTF_Font *fonte, TTF_Font *fontePequena,
                                const char *titulo, int x, int y, int w, int h, float abertura = 1.0f)
{
    if (abertura < 0.01f) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>(100 * abertura));
    SDL_Rect overlay = {0, 0, LARGURA_JANELA, ALTURA_JANELA};
    SDL_RenderFillRect(renderer, &overlay);

    float scale = 0.85f + 0.15f * abertura;
    int pw = static_cast<int>(w * scale);
    int ph = static_cast<int>(h * scale);
    int px = x + (w - pw) / 2;
    int py = y + (h - ph) / 2;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>(80 * abertura));
    desenharRetanguloArredondado(renderer, px + 4, py + 6, pw, ph, 14);

    SDL_SetRenderDrawColor(renderer, 180, 140, 60, static_cast<Uint8>(255 * abertura));
    desenharRetanguloArredondado(renderer, px - 2, py - 2, pw + 4, ph + 4, 14);

    SDL_SetRenderDrawColor(renderer, COR_BODY_BG.r, COR_BODY_BG.g, COR_BODY_BG.b, static_cast<Uint8>(245 * abertura));
    desenharRetanguloArredondado(renderer, px, py, pw, ph, 14);

    if (abertura < 0.6f)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        return;
    }

    int headerH = 56;
    SDL_SetRenderDrawColor(renderer, COR_HEADER_BG.r, COR_HEADER_BG.g, COR_HEADER_BG.b, 255);
    SDL_Rect header = {x, y, w, headerH};
    SDL_RenderFillRect(renderer, &header);

    desenharTexto(renderer, fonte, titulo, x + 24, y + headerH / 2 - 10, COR_HEADER_TITULO, false);

    int xBtn = x + w - 44;
    int yBtn = y + 12;
    SDL_SetRenderDrawColor(renderer, COR_BADGE_LV.r, COR_BADGE_LV.g, COR_BADGE_LV.b, 255);
    desenharRetanguloArredondado(renderer, xBtn, yBtn, 32, 32, 6);
    SDL_Color corX = {255, 230, 230, 255};
    desenharTexto(renderer, fonte, "X", xBtn + 16, yBtn + 16, corX, true);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

inline bool painelBaseFechou(int mouseX, int mouseY, int x, int y, int w)
{
    int xBtn = x + w - 44;
    int yBtn = y + 12;
    int t = CLICK_TOLERANCIA;
    return mouseX >= xBtn - t && mouseX <= xBtn + 32 + t &&
           mouseY >= yBtn - t && mouseY <= yBtn + 32 + t;
}

inline void desenharTabs(SDL_Renderer *renderer, TTF_Font *fonte,
                          const char **labels, const bool *habilitadas, int totalTabs,
                          int tabAtiva, int x, int y, int w)
{
    int tabW = w / totalTabs;
    int tabH = 50;

    for (int i = 0; i < totalTabs; i++)
    {
        int tx = x + i * tabW;
        bool ativa = (i == tabAtiva);
        bool habilitada = !habilitadas || habilitadas[i];

        if (!habilitada)
        {
            SDL_SetRenderDrawColor(renderer, 180, 165, 140, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer,
                                    ativa ? COR_BODY_BG.r : 220,
                                    ativa ? COR_BODY_BG.g : 200,
                                    ativa ? COR_BODY_BG.b : 165, 255);
        }
        SDL_Rect tab = {tx, y, tabW, tabH};
        SDL_RenderFillRect(renderer, &tab);

        if (ativa && habilitada)
        {
            SDL_SetRenderDrawColor(renderer, COR_TEXTO_PRECO.r, COR_TEXTO_PRECO.g, COR_TEXTO_PRECO.b, 255);
            SDL_Rect underline = {tx, y + tabH - 4, tabW, 4};
            SDL_RenderFillRect(renderer, &underline);
        }

        SDL_Color cor;
        if (!habilitada)         cor = SDL_Color{120, 105, 90, 200};
        else if (ativa)          cor = COR_TEXTO_PRECO;
        else                     cor = COR_TEXTO_DARK;
        desenharTexto(renderer, fonte, labels[i], tx + tabW / 2, y + tabH / 2 - 6, cor, true);

        if (!habilitada)
        {
            SDL_Color corBreve = {120, 105, 90, 200};
            desenharTexto(renderer, fonte, "Em breve", tx + tabW / 2, y + tabH / 2 + 12, corBreve, true);
        }
    }
}

inline int tabsHitTest(int mouseX, int mouseY, int totalTabs, int x, int y, int w)
{
    int tabW = w / totalTabs;
    int tabH = 50;
    int t = CLICK_TOLERANCIA;
    if (mouseY < y - t || mouseY >= y + tabH + t) return -1;
    if (mouseX < x || mouseX >= x + w) return -1;
    int idx = (mouseX - x) / tabW;
    if (idx < 0 || idx >= totalTabs) return -1;
    return idx;
}

static constexpr int CARD_W = 204;
static constexpr int CARD_H = 280;
static constexpr int CARD_H_COMPACTA = 176;
static constexpr int CARD_GAP = 12;

inline void slotCardGrade(int indice, int colunas, int originX, int originY,
                          int cardW, int cardH, int gap, int &sx, int &sy)
{
    int col = indice % colunas;
    int lin = indice / colunas;
    sx = originX + col * (cardW + gap);
    sy = originY + lin * (cardH + gap);
}

inline int desenharCardItem(SDL_Renderer *renderer, TTF_Font *fonte, TTF_Font *fontePequena,
                             SDL_Texture *sprite, const char *nome, const char *infoLinha,
                             int preco, SDL_Texture *iconeMoeda,
                             bool podeComprar, int requerLv,
                             int slotX, int slotY,
                             const char *labelBotao = "Comprar",
                             bool hover = false,
                             int cardH = CARD_H)
{
    const int cardW = CARD_W;
    const bool compacta = cardH < 240;

    if (hover)
    {
        SDL_SetRenderDrawColor(renderer, 255, 215, 100, 255);
        desenharRetanguloArredondado(renderer, slotX - 3, slotY - 3, cardW + 6, cardH + 6, 14);
    }

    SDL_SetRenderDrawColor(renderer, COR_CARD_BG.r, COR_CARD_BG.g, COR_CARD_BG.b, 255);
    desenharRetanguloArredondado(renderer, slotX, slotY, cardW, cardH, 12);
    SDL_SetRenderDrawColor(renderer, COR_CARD_BORDER.r, COR_CARD_BORDER.g, COR_CARD_BORDER.b, 255);
    desenharContornoArredondado(renderer, slotX, slotY, cardW, cardH, 12);

    if (sprite)
    {
        int boxTam = compacta ? (hover ? 64 : 56) : (hover ? 104 : 96);
        int boxX = slotX + (cardW - boxTam) / 2;
        int boxY = slotY + (compacta ? 6 : 16);
        int srcW = 0, srcH = 0;
        SDL_QueryTexture(sprite, nullptr, nullptr, &srcW, &srcH);
        int displayW = boxTam, displayH = boxTam;
        if (srcW > 0 && srcH > 0)
        {
            float aspect = static_cast<float>(srcW) / srcH;
            if (aspect >= 1.0f)
            {
                displayW = boxTam;
                displayH = static_cast<int>(boxTam / aspect);
            }
            else
            {
                displayH = boxTam;
                displayW = static_cast<int>(boxTam * aspect);
            }
        }
        SDL_Rect destSprite = {boxX + (boxTam - displayW) / 2,
                                boxY + (boxTam - displayH),
                                displayW, displayH};
        SDL_RenderCopy(renderer, sprite, nullptr, &destSprite);
    }

    int nomeY = slotY + (compacta ? 70 : 134);
    int infoY = slotY + (compacta ? 86 : 152);
    int precoIconeY = slotY + (compacta ? 100 : 184);
    int precoTextoY = slotY + (compacta ? 104 : 188);

    desenharTexto(renderer, compacta ? fontePequena : fonte, nome, slotX + cardW / 2, nomeY, COR_TEXTO_DARK, true);

    if (infoLinha)
        desenharTexto(renderer, fontePequena, infoLinha, slotX + cardW / 2, infoY, COR_TEXTO_DARK, true);

    if (iconeMoeda)
    {
        SDL_Rect destIcone = {slotX + 50, precoIconeY, compacta ? 16 : 22, compacta ? 16 : 22};
        SDL_RenderCopy(renderer, iconeMoeda, nullptr, &destIcone);
    }
    char bufPreco[16];
    snprintf(bufPreco, sizeof(bufPreco), "%d", preco);
    desenharTexto(renderer, compacta ? fontePequena : fonte, bufPreco, slotX + 80, precoTextoY, COR_TEXTO_PRECO, false);

    int btnH = compacta ? 26 : 36;
    int btnY = slotY + cardH - (compacta ? 34 : 50);

    if (requerLv > 0)
    {
        SDL_SetRenderDrawColor(renderer, COR_BADGE_LV.r, COR_BADGE_LV.g, COR_BADGE_LV.b, 255);
        desenharRetanguloArredondado(renderer, slotX + 16, btnY, cardW - 32, btnH, 8);
        char bufLv[24];
        snprintf(bufLv, sizeof(bufLv), "Requer Lv.%d", requerLv);
        SDL_Color corBadge = {255, 255, 255, 255};
        desenharTexto(renderer, fontePequena, bufLv, slotX + cardW / 2, btnY + btnH / 2, corBadge, true);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer,
                                podeComprar ? COR_BOTAO_COMPRAR.r : 130,
                                podeComprar ? COR_BOTAO_COMPRAR.g : 130,
                                podeComprar ? COR_BOTAO_COMPRAR.b : 130, 255);
        desenharRetanguloArredondado(renderer, slotX + 16, btnY, cardW - 32, btnH, 8);
        SDL_Color corBtnTxt = {255, 255, 255, 255};
        desenharTexto(renderer, fontePequena, labelBotao, slotX + cardW / 2, btnY + btnH / 2, corBtnTxt, true);
    }

    return btnY;
}

inline bool cardCompraClicado(int mouseX, int mouseY, int slotX, int slotY, int cardH = CARD_H)
{
    const int cardW = CARD_W;
    const bool compacta = cardH < 240;
    int btnH = compacta ? 26 : 36;
    int btnY = slotY + cardH - (compacta ? 34 : 50);
    int t = CLICK_TOLERANCIA;
    return mouseX >= slotX + 16 - t && mouseX <= slotX + cardW - 16 + t &&
           mouseY >= btnY - t && mouseY <= btnY + btnH + t;
}
