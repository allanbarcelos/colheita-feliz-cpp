#pragma once

#include <SDL2/SDL.h>
#include "Constantes.h"
#include "Assets.h"

static constexpr int TOTAL_DECORACOES = 9;
static constexpr int MAX_DECORACOES_COLOCADAS = 64;

struct ItemDecoracao
{
    const char *nome;
    const char *nomeArquivo;
    int precoOuro;
    int precoVerdes;
    int requerLv;
    int framesAnimacao;
    int largura;
    int altura;
};

struct DecoracaoColocada
{
    int idItem;
    int x, y;
    int rotacao;
};

static constexpr int MAX_FRAMES_ANIMACAO = 8;

struct DecoracaoAssets
{
    SDL_Texture *sprites[TOTAL_DECORACOES] = {};
    SDL_Texture *previews[TOTAL_DECORACOES] = {};
    int frameOffsetX[TOTAL_DECORACOES][MAX_FRAMES_ANIMACAO] = {};
    int frameOffsetY[TOTAL_DECORACOES][MAX_FRAMES_ANIMACAO] = {};
};

static const ItemDecoracao TABELA_DECORACOES[TOTAL_DECORACOES] = {
    {"Espantalho",     "espantalho",     80,  0, 1, 1,  48,  80},
    {"Banco de Madeira", "banco",       100,  0, 1, 1,  96,  72},
    {"Árvore",         "arvore",        150,  0, 2, 1,  64,  96},
    {"Vaso de Flor",   "vaso",           50,  0, 1, 1,  64,  96},
    {"Poço",           "poco",          250,  1, 3, 1,  48,  64},
    {"Churrasqueira",  "churrasqueira", 350,  1, 4, 1,  96,  96},
    {"Moinho",         "moinho",        500,  2, 6, 1,  96, 128},
    {"Estátua",        "estatua",         0,  5, 5, 1,  96, 144},
    {"Fonte",          "fonte",           0,  8, 8, 4, 120, 120}
};

inline void analisarOffsetsFrame(SDL_Surface *origem, int idItem, DecoracaoAssets &d)
{
    const ItemDecoracao &item = TABELA_DECORACOES[idItem];
    int frameW = item.largura;
    int frameH = item.altura;
    int totalFrames = item.framesAnimacao;
    if (totalFrames <= 1 || totalFrames > MAX_FRAMES_ANIMACAO) return;

    SDL_Surface *surf = SDL_ConvertSurfaceFormat(origem, SDL_PIXELFORMAT_RGBA8888, 0);
    if (!surf) return;

    SDL_LockSurface(surf);
    Uint32 *pixels = static_cast<Uint32 *>(surf->pixels);
    int pitch = surf->pitch / 4;

    int yBaseInicio = static_cast<int>(frameH * 0.6f);
    int frameRefCx = -1;
    int frameRefCy = -1;

    for (int f = 0; f < totalFrames; f++)
    {
        long long sumX = 0, sumY = 0;
        long long count = 0;
        int frameOffsetInSheet = f * frameW;
        for (int y = yBaseInicio; y < frameH; y++)
        {
            for (int x = 0; x < frameW; x++)
            {
                int sx = frameOffsetInSheet + x;
                if (sx >= surf->w) break;
                Uint32 pixel = pixels[y * pitch + sx];
                Uint8 r, g, b, a;
                SDL_GetRGBA(pixel, surf->format, &r, &g, &b, &a);
                if (a > 96)
                {
                    sumX += x;
                    sumY += y;
                    count++;
                }
            }
        }
        if (count > 0)
        {
            int cx = static_cast<int>(sumX / count);
            int cy = static_cast<int>(sumY / count);
            if (frameRefCx < 0)
            {
                frameRefCx = cx;
                frameRefCy = cy;
                d.frameOffsetX[idItem][f] = 0;
                d.frameOffsetY[idItem][f] = 0;
            }
            else
            {
                d.frameOffsetX[idItem][f] = frameRefCx - cx;
                d.frameOffsetY[idItem][f] = 0;
            }
        }
        else
        {
            d.frameOffsetX[idItem][f] = 0;
            d.frameOffsetY[idItem][f] = 0;
        }
    }

    SDL_UnlockSurface(surf);
    SDL_FreeSurface(surf);
}

inline DecoracaoAssets carregarDecoracaoAssets(SDL_Renderer *renderer)
{
    DecoracaoAssets d;
    char caminho[256];
    for (int i = 0; i < TOTAL_DECORACOES; i++)
    {
        snprintf(caminho, sizeof(caminho), "assets/sprites/buildings/%s.png", TABELA_DECORACOES[i].nomeArquivo);
        SDL_Surface *surf = IMG_Load(caminho);
        if (surf)
        {
            d.sprites[i] = SDL_CreateTextureFromSurface(renderer, surf);
            if (TABELA_DECORACOES[i].framesAnimacao > 1)
                analisarOffsetsFrame(surf, i, d);
            SDL_FreeSurface(surf);
        }
        snprintf(caminho, sizeof(caminho), "assets/sprites/buildings/%s-base1.png", TABELA_DECORACOES[i].nomeArquivo);
        d.previews[i] = carregarTextura(renderer, caminho);
    }
    return d;
}

inline void liberarDecoracaoAssets(DecoracaoAssets &d)
{
    for (int i = 0; i < TOTAL_DECORACOES; i++)
    {
        if (d.sprites[i])  SDL_DestroyTexture(d.sprites[i]);
        if (d.previews[i]) SDL_DestroyTexture(d.previews[i]);
    }
}

inline SDL_Texture *spritePreview(const DecoracaoAssets &assets, int idItem)
{
    if (idItem < 0 || idItem >= TOTAL_DECORACOES) return nullptr;
    return assets.previews[idItem] ? assets.previews[idItem] : assets.sprites[idItem];
}

inline int primeiroItemNoInventario(const int inventario[TOTAL_DECORACOES])
{
    for (int i = 0; i < TOTAL_DECORACOES; i++)
        if (inventario[i] > 0) return i;
    return -1;
}

inline int proximoItemInventario(const int inventario[TOTAL_DECORACOES], int atual)
{
    for (int delta = 1; delta <= TOTAL_DECORACOES; delta++)
    {
        int idx = (atual + delta) % TOTAL_DECORACOES;
        if (inventario[idx] > 0) return idx;
    }
    return atual;
}

inline SDL_Rect frameDecoracao(int idItem, Uint32 tempoJogoMs)
{
    const ItemDecoracao &d = TABELA_DECORACOES[idItem];
    if (d.framesAnimacao <= 1)
        return SDL_Rect{0, 0, d.largura, d.altura};
    int frame = (tempoJogoMs / 165) % d.framesAnimacao;
    return SDL_Rect{frame * d.largura, 0, d.largura, d.altura};
}

inline void desenharFantasmaDecoracao(SDL_Renderer *renderer, const DecoracaoAssets &assets,
                                       int idItem, int mouseX, int mouseY, Uint32 tempoJogoMs,
                                       int rotacao)
{
    if (idItem < 0 || idItem >= TOTAL_DECORACOES) return;
    SDL_Texture *sprite = assets.sprites[idItem];
    if (!sprite) return;

    const ItemDecoracao &d = TABELA_DECORACOES[idItem];
    SDL_Rect origem = frameDecoracao(idItem, tempoJogoMs);

    SDL_SetTextureAlphaMod(sprite, 130);
    int w = 0, h = 0;
    SDL_QueryTexture(sprite, nullptr, nullptr, &w, &h);
    bool ehSheet = (w > d.largura);
    int offX = 0, offY = 0;
    if (ehSheet && d.framesAnimacao > 1)
    {
        int frame = (tempoJogoMs / 165) % d.framesAnimacao;
        offX = assets.frameOffsetX[idItem][frame];
        offY = assets.frameOffsetY[idItem][frame];
    }
    SDL_Rect dest = {mouseX - d.largura / 2 + offX, mouseY - d.altura + offY, d.largura, d.altura};
    SDL_RendererFlip flip = (rotacao != 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    if (ehSheet)
        SDL_RenderCopyEx(renderer, sprite, &origem, &dest, 0.0, nullptr, flip);
    else
        SDL_RenderCopyEx(renderer, sprite, nullptr, &dest, 0.0, nullptr, flip);
    SDL_SetTextureAlphaMod(sprite, 255);
}

inline void desenharDecoracoesColocadas(SDL_Renderer *renderer, const DecoracaoAssets &assets,
                                         const DecoracaoColocada *colocadas, int total,
                                         Uint32 tempoJogoMs)
{
    for (int i = 0; i < total; i++)
    {
        const DecoracaoColocada &c = colocadas[i];
        if (c.idItem < 0 || c.idItem >= TOTAL_DECORACOES) continue;
        SDL_Texture *sprite = assets.sprites[c.idItem];
        if (!sprite) continue;

        const ItemDecoracao &d = TABELA_DECORACOES[c.idItem];
        SDL_Rect origem = frameDecoracao(c.idItem, tempoJogoMs);
        int w = 0, h = 0;
        SDL_QueryTexture(sprite, nullptr, nullptr, &w, &h);
        bool ehSheet = (w > d.largura);
        int offX = 0, offY = 0;
        if (ehSheet && d.framesAnimacao > 1)
        {
            int frame = (tempoJogoMs / 165) % d.framesAnimacao;
            offX = assets.frameOffsetX[c.idItem][frame];
            offY = assets.frameOffsetY[c.idItem][frame];
        }
        SDL_Rect dest = {c.x - d.largura / 2 + offX, c.y - d.altura + offY, d.largura, d.altura};
        SDL_RendererFlip flip = (c.rotacao != 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        if (ehSheet)
            SDL_RenderCopyEx(renderer, sprite, &origem, &dest, 0.0, nullptr, flip);
        else
            SDL_RenderCopyEx(renderer, sprite, nullptr, &dest, 0.0, nullptr, flip);
    }
}

inline int decoracaoColocadaHitTest(const DecoracaoColocada *colocadas, int total,
                                     int mouseX, int mouseY)
{
    int t = CLICK_TOLERANCIA;
    for (int i = total - 1; i >= 0; i--)
    {
        const DecoracaoColocada &c = colocadas[i];
        if (c.idItem < 0 || c.idItem >= TOTAL_DECORACOES) continue;
        const ItemDecoracao &d = TABELA_DECORACOES[c.idItem];
        if (mouseX >= c.x - d.largura / 2 - t && mouseX <= c.x + d.largura / 2 + t &&
            mouseY >= c.y - d.altura - t && mouseY <= c.y + t)
            return i;
    }
    return -1;
}
