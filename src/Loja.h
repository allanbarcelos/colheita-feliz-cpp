#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>

#include "Constantes.h"
#include "Tipos.h"
#include "Desenho.h"
#include "Crops.h"
#include "Player.h"
#include "GameState.h"

inline void desenharLoja(SDL_Renderer *renderer, TTF_Font *fonte, TTF_Font *fontePequena, const CropAssets &ca, const GameState &s)
{
    float abertura = s.lojaAbertura;
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
    desenharTexto(renderer, fonte, "Loja de Sementes", PAINEL_X + PAINEL_LARGURA / 2, PAINEL_Y + PAINEL_TITULO_ALTURA / 2, corTitulo, true);

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

        bool desbloqueada = atingiuNivel(s.xp, TABELA_CROPS[i].nivelDesbloqueio);

        if (desbloqueada)
            SDL_SetRenderDrawColor(renderer, 70, 55, 35, 220);
        else
            SDL_SetRenderDrawColor(renderer, 30, 25, 20, 220);
        desenharRetanguloArredondado(renderer, sx, sy, SEMENTE_SLOT_LARGURA, SEMENTE_SLOT_ALTURA, 6);

        if (ca.sementes[i])
        {
            int padding = 8;
            int tamIcone = SEMENTE_ICONE - padding;
            int iconeX = sx + (SEMENTE_SLOT_LARGURA - tamIcone) / 2;
            int iconeY = sy + padding;
            SDL_Rect destino = {iconeX, iconeY, tamIcone, tamIcone};
            if (!desbloqueada) SDL_SetTextureAlphaMod(ca.sementes[i], 70);
            SDL_RenderCopy(renderer, ca.sementes[i], nullptr, &destino);
            if (!desbloqueada) SDL_SetTextureAlphaMod(ca.sementes[i], 255);
        }

        if (desbloqueada)
        {
            SDL_Color corNome = {230, 220, 200, 255};
            desenharTexto(renderer, fontePequena, TABELA_CROPS[i].nome, sx + SEMENTE_SLOT_LARGURA / 2, sy + SEMENTE_SLOT_ALTURA - 26, corNome, true);

            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%d ouro", TABELA_CROPS[i].precoCompra);
            SDL_Color corPreco = {255, 220, 100, 255};
            desenharTexto(renderer, fontePequena, buffer, sx + SEMENTE_SLOT_LARGURA / 2, sy + SEMENTE_SLOT_ALTURA - 12, corPreco, true);
        }
        else
        {
            SDL_Color corCinza = {130, 130, 130, 255};
            desenharTexto(renderer, fontePequena, TABELA_CROPS[i].nome, sx + SEMENTE_SLOT_LARGURA / 2, sy + SEMENTE_SLOT_ALTURA - 26, corCinza, true);

            char buf[32];
            snprintf(buf, sizeof(buf), "Nv %d", TABELA_CROPS[i].nivelDesbloqueio);
            SDL_Color corBloq = {200, 120, 120, 255};
            desenharTexto(renderer, fontePequena, buf, sx + SEMENTE_SLOT_LARGURA / 2, sy + SEMENTE_SLOT_ALTURA - 12, corBloq, true);
        }
    }

    int racaoY = PAINEL_Y + PAINEL_ALTURA - 60;
    int racaoX = PAINEL_X + PAINEL_PADDING;
    int racaoLargura = PAINEL_LARGURA - 2 * PAINEL_PADDING;
    int racaoAltura = 44;

    bool podeComprarRacao = (s.ouro >= PRECO_RACAO);
    SDL_SetRenderDrawColor(renderer,
                           podeComprarRacao ? 100 : 60,
                           podeComprarRacao ? 80  : 50,
                           podeComprarRacao ? 50  : 45,
                           240);
    desenharRetanguloArredondado(renderer, racaoX, racaoY, racaoLargura, racaoAltura, 8);

    if (s.animalAssets.iconeRacao)
    {
        SDL_Rect destIcone = {racaoX + 8, racaoY + 6, 32, 32};
        SDL_RenderCopy(renderer, s.animalAssets.iconeRacao, nullptr, &destIcone);
    }

    SDL_Color corLabel = {255, 235, 180, 255};
    desenharTexto(renderer, fonte, "Racao Animal", racaoX + 50, racaoY + 8, corLabel, false);

    char bufRacao[64];
    snprintf(bufRacao, sizeof(bufRacao), "%d ouro - Tenho: %d", PRECO_RACAO, s.inventarioRacao);
    SDL_Color corPrecoRacao = {255, 220, 100, 255};
    desenharTexto(renderer, fontePequena, bufRacao, racaoX + 50, racaoY + 26, corPrecoRacao, false);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

inline int lojaHitTest(int mouseX, int mouseY)
{
    int fecharX = PAINEL_X + PAINEL_LARGURA - 24;
    int fecharY = PAINEL_Y + 6;
    if (mouseX >= fecharX && mouseX <= fecharX + 20 && mouseY >= fecharY && mouseY <= fecharY + 20)
        return -2;

    for (int i = 0; i < TOTAL_CROPS; i++)
    {
        int col = i % PAINEL_COLUNAS_SEMENTE;
        int lin = i / PAINEL_COLUNAS_SEMENTE;
        int sx = painelSementeSlotX(col);
        int sy = painelSementeSlotY(lin);

        if (mouseX >= sx && mouseX <= sx + SEMENTE_SLOT_LARGURA && mouseY >= sy && mouseY <= sy + SEMENTE_SLOT_ALTURA)
            return i;
    }

    {
        int racaoY = PAINEL_Y + PAINEL_ALTURA - 60;
        int racaoX = PAINEL_X + PAINEL_PADDING;
        int racaoLargura = PAINEL_LARGURA - 2 * PAINEL_PADDING;
        int racaoAltura = 44;
        if (mouseX >= racaoX && mouseX <= racaoX + racaoLargura &&
            mouseY >= racaoY && mouseY <= racaoY + racaoAltura)
            return 999;
    }

    if (mouseX >= PAINEL_X && mouseX <= PAINEL_X + PAINEL_LARGURA && mouseY >= PAINEL_Y && mouseY <= PAINEL_Y + PAINEL_ALTURA)
        return -3;

    return -1;
}