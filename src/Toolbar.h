#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>

#include "Constantes.h"
#include "Tipos.h"
#include "Assets.h"
#include "Desenho.h"

inline void carregarIconesToolbar(SDL_Renderer *renderer, Toolbar &toolbar)
{
    const char *arquivos[7] = {
        "assets/sprites/icons/toolbar/cursor.png",
        "assets/sprites/icons/toolbar/enxada.png",
        "assets/sprites/icons/toolbar/sacola.png",
        "assets/sprites/icons/toolbar/regador.png",
        "assets/sprites/icons/toolbar/removedor.png",
        "assets/sprites/icons/toolbar/pesticida.png",
        "assets/sprites/icons/toolbar/mão.png",
    };

    for (int i = 0; i < 7; i++)
    {
        toolbar.icones[i] = carregarTextura(renderer, arquivos[i]);
    }

    toolbar.cursorApontando = carregarTextura(renderer, "assets/sprites/icons/toolbar/cursor_apontando.png");
    toolbar.cursorPegando = carregarTextura(renderer, "assets/sprites/icons/toolbar/cursor_pegando.png");
}

inline int slotX(int indice)
{
    return TOOLBAR_X + indice * (SLOT_TAMANHO + SLOT_ESPACAMENTO);
}

inline int slotY()
{
    return TOOLBAR_Y + TOOLBAR_PADDING;
}

inline void desenharRetanguloArredondado(SDL_Renderer *renderer, int x, int y, int w, int h, int raio)
{
    SDL_Rect centro = {x + raio, y, w - 2 * raio, h};
    SDL_RenderFillRect(renderer, &centro);

    SDL_Rect esquerda = {x, y + raio, raio, h - 2 * raio};
    SDL_RenderFillRect(renderer, &esquerda);

    SDL_Rect direita = {x + w - raio, y + raio, raio, h - 2 * raio};
    SDL_RenderFillRect(renderer, &direita);

    for (int dy = -raio; dy <= raio; dy++)
    {
        int dx = static_cast<int>(sqrt(raio * raio - dy * dy));

        SDL_RenderDrawLine(renderer,
                           x + raio - dx, y + raio + dy,
                           x + raio, y + raio + dy);

        SDL_RenderDrawLine(renderer,
                           x + w - raio, y + raio + dy,
                           x + w - raio + dx, y + raio + dy);

        SDL_RenderDrawLine(renderer,
                           x + raio - dx, y + h - raio + dy,
                           x + raio, y + h - raio + dy);

        SDL_RenderDrawLine(renderer,
                           x + w - raio, y + h - raio + dy,
                           x + w - raio + dx, y + h - raio + dy);
    }
}

inline void desenharContornoArredondado(SDL_Renderer *renderer, int x, int y, int w, int h, int raio)
{
    SDL_RenderDrawLine(renderer, x + raio, y, x + w - raio, y);
    SDL_RenderDrawLine(renderer, x + raio, y + h, x + w - raio, y + h);
    SDL_RenderDrawLine(renderer, x, y + raio, x, y + h - raio);
    SDL_RenderDrawLine(renderer, x + w, y + raio, x + w, y + h - raio);

    for (int ang = 0; ang <= 90; ang++)
    {
        float rad = ang * 3.14159f / 180.0f;
        int dx = static_cast<int>(raio * cos(rad));
        int dy = static_cast<int>(raio * sin(rad));

        SDL_RenderDrawPoint(renderer, x + raio - dx, y + raio - dy);
        SDL_RenderDrawPoint(renderer, x + w - raio + dx, y + raio - dy);
        SDL_RenderDrawPoint(renderer, x + raio - dx, y + h - raio + dy);
        SDL_RenderDrawPoint(renderer, x + w - raio + dx, y + h - raio + dy);
    }
}

inline void desenharTooltipModerno(SDL_Renderer *renderer, TTF_Font *fonte, const char *texto, int centerX, int baixoY)
{
    if (!fonte || !texto)
        return;

    int tw = 0, th = 0;
    TTF_SizeUTF8(fonte, texto, &tw, &th);

    int padX = 14;
    int padY = 7;
    int w = tw + 2 * padX;
    int h = th + 2 * padY;
    int raio = h / 2;

    int x = centerX - w / 2;
    int y = baixoY - h - 22;

    if (x < 4)
        x = 4;
    if (x + w > LARGURA_JANELA - 4)
        x = LARGURA_JANELA - 4 - w;
    if (y < 4)
        y = baixoY + 14;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 90);
    desenharRetanguloArredondado(renderer, x + 2, y + 4, w, h, raio);

    SDL_SetRenderDrawColor(renderer, 28, 22, 16, 235);
    desenharRetanguloArredondado(renderer, x, y, w, h, raio);

    SDL_SetRenderDrawColor(renderer, 218, 165, 32, 170);
    desenharContornoArredondado(renderer, x, y, w, h, raio);

    SDL_Color corTexto = {255, 240, 210, 255};
    desenharTextoComContorno(renderer, fonte, texto, x + w / 2, y + h / 2, corTexto, true);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

inline void atualizarAnimacoes(Toolbar &toolbar, float dt)
{
    float velocidade = 8.0f;

    for (int i = 0; i < TOTAL_FERRAMENTAS; i++)
    {
        float alvo;
        if (toolbar.painelAberto)
        {
            alvo = (i == CURSOR) ? 1.0f : 0.0f;
        }
        else
        {
            alvo = (static_cast<int>(toolbar.selecionada) == i) ? 1.0f : 0.0f;
        }
        float diff = alvo - toolbar.selecaoAnimacao[i];
        toolbar.selecaoAnimacao[i] += diff * velocidade * dt;

        if (toolbar.selecaoAnimacao[i] < 0.005f) toolbar.selecaoAnimacao[i] = 0.0f;
        if (toolbar.selecaoAnimacao[i] > 0.995f) toolbar.selecaoAnimacao[i] = 1.0f;
    }

    for (int i = 0; i < TOTAL_CROPS; i++)
    {
        float alvo = (toolbar.sementeSelecionada == i) ? 1.0f : 0.0f;
        float diff = alvo - toolbar.painelSelAnimacao[i];
        toolbar.painelSelAnimacao[i] += diff * velocidade * dt;

        if (toolbar.painelSelAnimacao[i] < 0.005f) toolbar.painelSelAnimacao[i] = 0.0f;
        if (toolbar.painelSelAnimacao[i] > 0.995f) toolbar.painelSelAnimacao[i] = 1.0f;
    }

    float alvoAbrir = toolbar.painelAberto ? 1.0f : 0.0f;
    float diffAbrir = alvoAbrir - toolbar.painelAberturaAnimacao;
    toolbar.painelAberturaAnimacao += diffAbrir * 6.0f * dt;
    if (toolbar.painelAberturaAnimacao < 0.005f) toolbar.painelAberturaAnimacao = 0.0f;
    if (toolbar.painelAberturaAnimacao > 0.995f) toolbar.painelAberturaAnimacao = 1.0f;
}

inline void desenharToolbar(SDL_Renderer *renderer, const Toolbar &toolbar, SDL_Texture *sementeIcone = nullptr)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    int barX = TOOLBAR_X - 8;
    int barY = TOOLBAR_Y;
    int barW = TOOLBAR_LARGURA + 16;
    int barH = TOOLBAR_ALTURA;
    int barRaio = 14;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 80);
    desenharRetanguloArredondado(renderer, barX + 2, barY + 3, barW, barH, barRaio);

    SDL_SetRenderDrawColor(renderer, 42, 31, 20, 215);
    desenharRetanguloArredondado(renderer, barX, barY, barW, barH, barRaio);

    SDL_SetRenderDrawColor(renderer, 180, 140, 60, 220);
    desenharContornoArredondado(renderer, barX, barY, barW, barH, barRaio);

    for (int i = 0; i < TOTAL_FERRAMENTAS; i++)
    {
        int sx = slotX(i);
        int sy = slotY();
        float anim = toolbar.selecaoAnimacao[i];

        int bgR = 60 + static_cast<int>(20 * anim);
        int bgG = 45 + static_cast<int>(15 * anim);
        int bgB = 30 + static_cast<int>(10 * anim);
        SDL_SetRenderDrawColor(renderer, bgR, bgG, bgB, 180);
        desenharRetanguloArredondado(renderer, sx, sy, SLOT_TAMANHO, SLOT_TAMANHO, 6);

        if (toolbar.icones[i] != nullptr)
        {
            SDL_Rect destino = {
                sx + (SLOT_TAMANHO - ICONE_TAMANHO) / 2,
                sy + (SLOT_TAMANHO - ICONE_TAMANHO) / 2,
                ICONE_TAMANHO,
                ICONE_TAMANHO};

            SDL_RenderCopy(renderer, toolbar.icones[i], nullptr, &destino);
        }

        if (i == SACOLA && sementeIcone != nullptr)
        {
            int miniTam = 22;
            SDL_Rect miniDest = {
                sx + SLOT_TAMANHO - miniTam - 1,
                sy + SLOT_TAMANHO - miniTam - 1,
                miniTam,
                miniTam};
            SDL_RenderCopy(renderer, sementeIcone, nullptr, &miniDest);
        }

        if (anim > 0.01f)
        {
            int expansao = static_cast<int>(3.0f * anim);
            Uint8 alfa = static_cast<Uint8>(255 * anim);
            SDL_SetRenderDrawColor(renderer, 218, 165, 32, alfa);

            desenharContornoArredondado(renderer,
                                        sx - expansao, sy - expansao,
                                        SLOT_TAMANHO + 2 * expansao, SLOT_TAMANHO + 2 * expansao,
                                        4);

            if (anim > 0.5f)
            {
                Uint8 alfa2 = static_cast<Uint8>(255 * (anim - 0.5f) * 2.0f);
                SDL_SetRenderDrawColor(renderer, 218, 165, 32, alfa2);
                desenharContornoArredondado(renderer,
                                            sx - expansao - 1, sy - expansao - 1,
                                            SLOT_TAMANHO + 2 * expansao + 2, SLOT_TAMANHO + 2 * expansao + 2,
                                            5);
            }
        }
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

inline int toolbarHitTest(int mouseX, int mouseY)
{
    int sy = slotY();
    int t = CLICK_TOLERANCIA;

    if (mouseY < sy - t || mouseY > sy + SLOT_TAMANHO + t)
    {
        return -1;
    }

    for (int i = 0; i < TOTAL_FERRAMENTAS; i++)
    {
        int sx = slotX(i);

        if (mouseX >= sx - t && mouseX <= sx + SLOT_TAMANHO + t)
        {
            return i;
        }
    }

    return -1;
}

enum TipoCursor
{
    CURSOR_NORMAL = 0,
    CURSOR_APONTANDO = 1,
    CURSOR_PEGANDO = 2
};

inline void desenharCursorFerramenta(SDL_Renderer *renderer, const Toolbar &toolbar, int mouseX, int mouseY, SDL_Texture *sementeIcone = nullptr, bool forcarCursor = false, int tipoCursor = CURSOR_NORMAL)
{
    SDL_ShowCursor(SDL_DISABLE);

    SDL_Texture *icone = nullptr;

    if (tipoCursor == CURSOR_PEGANDO && toolbar.cursorPegando)
    {
        icone = toolbar.cursorPegando;
    }
    else if (tipoCursor == CURSOR_APONTANDO && toolbar.cursorApontando)
    {
        icone = toolbar.cursorApontando;
    }
    else if (forcarCursor || toolbar.painelAberto || toolbar.selecionada == CURSOR)
    {
        icone = toolbar.icones[CURSOR];
    }
    else if (toolbar.selecionada == SACOLA && sementeIcone != nullptr)
    {
        icone = sementeIcone;
    }
    else
    {
        int indice = static_cast<int>(toolbar.selecionada);
        icone = toolbar.icones[indice];
    }

    if (icone)
    {
        SDL_Rect destino = {
            mouseX - ICONE_TAMANHO / 2,
            mouseY - ICONE_TAMANHO / 2,
            ICONE_TAMANHO,
            ICONE_TAMANHO};

        SDL_RenderCopy(renderer, icone, nullptr, &destino);
    }
}

inline void liberarIconesToolbar(Toolbar &toolbar)
{
    if (toolbar.cursorApontando) { SDL_DestroyTexture(toolbar.cursorApontando); toolbar.cursorApontando = nullptr; }
    if (toolbar.cursorPegando)   { SDL_DestroyTexture(toolbar.cursorPegando);   toolbar.cursorPegando = nullptr; }
    for (int i = 0; i < 7; i++)
    {
        if(toolbar.icones[i] != nullptr){
            SDL_DestroyTexture(toolbar.icones[i]);
            toolbar.icones[i] = nullptr;
        }
    }
}
