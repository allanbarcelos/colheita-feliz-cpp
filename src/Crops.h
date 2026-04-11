#pragma once

#include "Constantes.h"
#include "Tipos.h"
#include "Assets.h"
#include <SDL2/SDL_ttf.h>

struct DadosCrop
{
    const char *nome;
    const char *nomeArquivo;
    int precoCompra;
    int precoVenda;
    int temporadas;
};

static const DadosCrop TABELA_CROPS[TOTAL_CROPS] = {
    {"Nabo", "nabo", 10, 20, 4},
    {"Cenoura", "cenoura", 15, 30, 3},
    {"Tomate", "tomate", 20, 45, 3},
    {"Milho", "milho", 25, 55, 2},
    {"Morango", "morango", 30, 65, 3},
    {"Batata", "batata", 18, 40, 3},
    {"Melancia", "melancia", 50, 120, 1},
    {"Arroz", "arroz", 22, 50, 2},
    {"Girassol", "girassol", 35, 75, 1},
    {"Repolho", "repolho", 20, 42, 2},
    {"Abóbora", "abobora", 45, 100, 1},
    {"Berinjela", "berinjela", 55, 130, 2},
    {"Uva", "uva", 70, 160, 3},
    {"Pepino", "pepino", 25, 52, 2},
    {"Pimentão", "pimentao", 60, 140, 2}};

static constexpr int CROP_TAMANHOS[TOTAL_ESTAGIOS] = {
    24,
    30,
    38,
    44,
    48};

struct CropAssets
{
    SDL_Texture *estagios[TOTAL_CROPS][TOTAL_ESTAGIOS] = {};
    SDL_Texture *sementes[TOTAL_CROPS] = {};
};

inline CropAssets carregarCropAssets(SDL_Renderer *renderer)
{
    CropAssets ca;
    char caminho[256];

    for (int tipo = 0; tipo < TOTAL_CROPS; tipo++)
    {
        for (int est = 0; est < TOTAL_ESTAGIOS; est++)
        {
            snprintf(caminho, sizeof(caminho), "assets/sprites/crops/%s_%d.png", TABELA_CROPS[tipo].nomeArquivo, est + 1);
            ca.estagios[tipo][est] = carregarTextura(renderer, caminho);
        }

        snprintf(caminho, sizeof(caminho), "assets/sprites/icons/seeds/semente_%s.png", TABELA_CROPS[tipo].nomeArquivo);
        ca.sementes[tipo] = carregarTextura(renderer, caminho);
    }

    return ca;
}

inline void liberarCropAssets(CropAssets &ca)
{
    for (int tipo = 0; tipo < TOTAL_CROPS; tipo++)
    {
        for (int est = 0; est < TOTAL_ESTAGIOS; est++)
        {
            if (ca.estagios[tipo][est])
            {
                SDL_DestroyTexture(ca.estagios[tipo][est]);
                ca.estagios[tipo][est] = nullptr;
            }
        }

        if (ca.sementes[tipo])
        {
            SDL_DestroyTexture(ca.sementes[tipo]);
            ca.sementes[tipo] = nullptr;
        }
    }
}

inline void desenharCrop(SDL_Renderer *renderer, const CropAssets &ca, int tipoCrop, int estagio, int telaX, int telaY)
{
    if (tipoCrop < 0 || tipoCrop >= TOTAL_CROPS)
        return;
    if (estagio < 1 || estagio > TOTAL_ESTAGIOS)
        return;

    SDL_Texture *textura = ca.estagios[tipoCrop][estagio - 1];
    int tam = CROP_TAMANHOS[estagio - 1];

    if (textura)
    {
        SDL_Rect destino = {
            telaX - tam / 2,
            telaY - tam + 4,
            tam,
            tam};
        SDL_RenderCopy(renderer, textura, nullptr, &destino);
    }
    else
    {
        int verde = 80 + estagio * 35;

        SDL_SetRenderDrawColor(renderer, 30, verde, 30, 25);
        SDL_Rect fallback = {
            telaX - tam / 2,
            telaY - tam + 4,
            tam,
            tam};
        SDL_RenderFillRect(renderer, &fallback);
    }
}

inline int painelSementeSlotX(int coluna)
{
    return PAINEL_X + PAINEL_PADDING + coluna * (SEMENTE_SLOT_LARGURA + SEMENTE_ESPACO);
}

inline int painelSementeSlotY(int linha)
{
    return PAINEL_Y + PAINEL_TITULO_ALTURA + PAINEL_PADDING + linha * (SEMENTE_SLOT_ALTURA + SEMENTE_ESPACO);
}

inline void desenharTexto(SDL_Renderer *renderer, TTF_Font *fonte, const char *texto, int x, int y, SDL_Color cor, bool centralizar = false)
{
    if (!fonte || !texto)
        return;

    SDL_Surface *superficie = TTF_RenderUTF8_Blended(fonte, texto, cor);
    if (!superficie)
        return;

    SDL_Texture *textura = SDL_CreateTextureFromSurface(renderer, superficie);
    int w = superficie->w;
    int h = superficie->h;
    SDL_FreeSurface(superficie);

    if (!textura)
        return;

    SDL_Rect destino;
    if (centralizar)
    {
        destino = {x - w / 2, y - h / 2, w, h};
    }
    else
    {
        destino = {x, y, w, h};
    }

    SDL_RenderCopy(renderer, textura, nullptr, &destino);
    SDL_DestroyTexture(textura);
}

inline void desenharPainelSementes(SDL_Renderer *renderer, TTF_Font *fonte, TTF_Font *fontePequena, const CropAssets &ca, const Toolbar &toolbar)
{
    float abertura = toolbar.painelAberturaAnimacao;
    if (abertura < 0.01f)
        return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    Uint8 overlayAlfa = static_cast<Uint8>(100 * abertura);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, overlayAlfa);
    SDL_Rect overlay = {0, 0, LARGURA_JANELA, ALTURA_JANELA};
    SDL_RenderFillRect(renderer, &overlay);

    int pw = static_cast<int>(PAINEL_LARGURA * abertura);
    int ph = static_cast<int>(PAINEL_ALTURA * abertura);
    int px = PAINEL_X + (PAINEL_LARGURA - pw) / 2;
    int py = PAINEL_Y + (PAINEL_ALTURA - ph) / 2;

    SDL_SetRenderDrawColor(renderer, 180, 140, 60, static_cast<Uint8>(255 * abertura));
    SDL_Rect bordaExterna = {px - 3, py - 3, pw + 6, ph + 6};
    SDL_RenderFillRect(renderer, &bordaExterna);

    SDL_SetRenderDrawColor(renderer, 54, 40, 26, static_cast<Uint8>(240 * abertura));
    SDL_Rect fundo = {px, py, pw, ph};
    SDL_RenderFillRect(renderer, &fundo);

    if (abertura < 0.5f)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        return;
    }

    float conteudoAlfa = (abertura - 0.5f) * 2.0f;

    SDL_SetRenderDrawColor(renderer, 140, 105, 50, static_cast<Uint8>(255 * conteudoAlfa));
    SDL_Rect barraTitulo = {PAINEL_X, PAINEL_Y, PAINEL_LARGURA, PAINEL_TITULO_ALTURA};
    SDL_RenderFillRect(renderer, &barraTitulo);

    SDL_SetRenderDrawColor(renderer, 180, 140, 60, static_cast<Uint8>(255 * conteudoAlfa));
    SDL_RenderDrawLine(renderer, PAINEL_X, PAINEL_Y + PAINEL_TITULO_ALTURA,
                       PAINEL_X + PAINEL_LARGURA, PAINEL_Y + PAINEL_TITULO_ALTURA);

    SDL_Color corTitulo = {255, 235, 180, static_cast<Uint8>(255 * conteudoAlfa)};
    desenharTexto(renderer, fonte, "Sementes",
                  PAINEL_X + PAINEL_LARGURA / 2,
                  PAINEL_Y + PAINEL_TITULO_ALTURA / 2,
                  corTitulo, true);

    SDL_Color corX = {255, 200, 200, static_cast<Uint8>(255 * conteudoAlfa)};
    int fecharX = PAINEL_X + PAINEL_LARGURA - 24;
    int fecharY = PAINEL_Y + 6;
    SDL_SetRenderDrawColor(renderer, 160, 50, 50, static_cast<Uint8>(255 * conteudoAlfa));
    SDL_Rect btnFechar = {fecharX, fecharY, 20, 20};
    SDL_RenderFillRect(renderer, &btnFechar);
    desenharTexto(renderer, fontePequena, "X",
                  fecharX + 10, fecharY + 10, corX, true);

    SDL_Color corNome = {230, 220, 200, static_cast<Uint8>(255 * conteudoAlfa)};
    SDL_Color corNomeSel = {255, 235, 130, static_cast<Uint8>(255 * conteudoAlfa)};

    for (int i = 0; i < TOTAL_CROPS; i++)
    {
        int col = i % PAINEL_COLUNAS_SEMENTE;
        int lin = i / PAINEL_COLUNAS_SEMENTE;
        int sx = painelSementeSlotX(col);
        int sy = painelSementeSlotY(lin);

        float anim = toolbar.painelSelAnimacao[i];

        int bgR = 70 + static_cast<int>(30 * anim);
        int bgG = 55 + static_cast<int>(25 * anim);
        int bgB = 35 + static_cast<int>(5 * anim);
        SDL_SetRenderDrawColor(renderer, bgR, bgG, bgB, static_cast<Uint8>(200 * conteudoAlfa));
        desenharRetanguloArredondado(renderer, sx, sy, SEMENTE_SLOT_LARGURA, SEMENTE_SLOT_ALTURA, 6);

        if (ca.sementes[i])
        {
            int padding = 8;
            int tamIcone = SEMENTE_ICONE - padding;
            int iconeX = sx + (SEMENTE_SLOT_LARGURA - tamIcone) / 2;
            int iconeY = sy + padding;
            SDL_Rect destino = {iconeX, iconeY, tamIcone, tamIcone};
            SDL_RenderCopy(renderer, ca.sementes[i], nullptr, &destino);
        }

        desenharTexto(renderer, fontePequena, TABELA_CROPS[i].nome,
                      sx + SEMENTE_SLOT_LARGURA / 2,
                      sy + SEMENTE_SLOT_ALTURA - 12,
                      anim > 0.5f ? corNomeSel : corNome, true);

        if (anim > 0.01f)
        {
            int expansao = static_cast<int>(3.0f * anim);
            Uint8 alfa = static_cast<Uint8>(255 * anim * conteudoAlfa);
            SDL_SetRenderDrawColor(renderer, 218, 165, 32, alfa);
            desenharContornoArredondado(renderer,
                                        sx - expansao, sy - expansao,
                                        SEMENTE_SLOT_LARGURA + 2 * expansao,
                                        SEMENTE_SLOT_ALTURA + 2 * expansao,
                                        4);

            if (anim > 0.5f)
            {
                Uint8 alfa2 = static_cast<Uint8>(255 * (anim - 0.5f) * 2.0f * conteudoAlfa);
                SDL_SetRenderDrawColor(renderer, 218, 165, 32, alfa2);
                desenharContornoArredondado(renderer,
                                            sx - expansao - 1, sy - expansao - 1,
                                            SEMENTE_SLOT_LARGURA + 2 * expansao + 2,
                                            SEMENTE_SLOT_ALTURA + 2 * expansao + 2,
                                            5);
            }
        }
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

inline int painelSementeHitTest(int mouseX, int mouseY)
{
    int fecharX = PAINEL_X + PAINEL_LARGURA - 24;
    int fecharY = PAINEL_Y + 6;
    if (mouseX >= fecharX && mouseX <= fecharX + 20 &&
        mouseY >= fecharY && mouseY <= fecharY + 20)
    {
        return -2;
    }

    for (int i = 0; i < TOTAL_CROPS; i++)
    {
        int col = i % PAINEL_COLUNAS_SEMENTE;
        int lin = i / PAINEL_COLUNAS_SEMENTE;
        int sx = painelSementeSlotX(col);
        int sy = painelSementeSlotY(lin);

        if (mouseX >= sx && mouseX <= sx + SEMENTE_SLOT_LARGURA &&
            mouseY >= sy && mouseY <= sy + SEMENTE_SLOT_ALTURA)
        {
            return i;
        }
    }

    if (mouseX >= PAINEL_X && mouseX <= PAINEL_X + PAINEL_LARGURA &&
        mouseY >= PAINEL_Y && mouseY <= PAINEL_Y + PAINEL_ALTURA)
    {
        return -3;
    }

    return -1;
}
