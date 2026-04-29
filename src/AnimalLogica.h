#pragma once

#include <SDL2/SDL.h>
#include <cmath>
#include <cstdlib>
#include "Constantes.h"
#include "Desenho.h"
#include "Animal.h"
#include "AnimalAssets.h"

inline void escolherNovoTarget(Animal &a, Uint32 tempoJogoMs)
{
    int margem = ANIMAL_TAMANHO / 2;
    float effRaioX = static_cast<float>(CERCADO_RAIO_X - margem);
    float effRaioY = static_cast<float>(CERCADO_RAIO_Y - margem);
    if (effRaioX < 1.0f) effRaioX = 1.0f;
    if (effRaioY < 1.0f) effRaioY = 1.0f;

    float zonaLargura = (2.0f * effRaioX) / 3.0f;
    float zonaInicio = -effRaioX + a.tipo * zonaLargura;
    float relX = zonaInicio + (rand() / static_cast<float>(RAND_MAX)) * zonaLargura;

    float relXNorm = fabsf(relX) / effRaioX;
    if (relXNorm > 1.0f) relXNorm = 1.0f;
    float yHalf = effRaioY * (1.0f - relXNorm);
    float relY = -yHalf + (rand() / static_cast<float>(RAND_MAX)) * 2.0f * yHalf;

    a.targetX = CERCADO_CENTRO_X + relX;
    a.targetY = CERCADO_CENTRO_Y + relY;
    a.timestampProximoTarget = tempoJogoMs + 5000 + (rand() % 5000);
}

inline void atualizarAnimal(Animal &a, float dt, Uint32 tempoJogoMs)
{
    float dx = a.targetX - a.x;
    float dy = a.targetY - a.y;
    float dist = sqrtf(dx * dx + dy * dy);
    if (tempoJogoMs >= a.timestampProximoTarget || dist < 4.0f)
    {
        escolherNovoTarget(a, tempoJogoMs);
        dx = a.targetX - a.x;
        dy = a.targetY - a.y;
        dist = sqrtf(dx * dx + dy * dy);
    }

    if (dist > 0.01f)
    {
        a.x += (dx / dist) * ANIMAL_VELOCIDADE_PX_S * dt;
        a.y += (dy / dist) * ANIMAL_VELOCIDADE_PX_S * dt;
        a.direcao = (dx < 0.0f) ? -1 : 1;
    }

    if (tempoJogoMs - a.timestampUltimaRefeicao >= TEMPO_FOME_MS)
        a.comFome = true;

    if (!a.comFome && !a.produtoPronto)
    {
        if (tempoJogoMs - a.timestampUltimaProducao >= INTERVALO_PRODUCAO_MS)
        {
            a.produtoPronto = true;
            a.timestampUltimaProducao = tempoJogoMs;
        }
    }
}

inline SDL_Texture *texturaDoAnimal(int tipo, const AnimalAssets &assets)
{
    switch (tipo)
    {
    case GALINHA: return assets.galinhaSheet;
    case VACA:    return assets.vacaSheet;
    case OVELHA:  return assets.ovelhaSheet;
    }
    return nullptr;
}

inline void desenharAnimal(SDL_Renderer *renderer, const Animal &a,
                            const AnimalAssets &assets, Uint32 tempoJogoMs)
{
    SDL_Texture *sheet = texturaDoAnimal(a.tipo, assets);
    if (!sheet) return;

    int frame = (tempoJogoMs / ANIMAL_FRAME_TEMPO_MS) % 4;
    SDL_Rect origem = {frame * ANIMAL_TAMANHO, 0, ANIMAL_TAMANHO, ANIMAL_TAMANHO};

    int destX = static_cast<int>(a.x) - ANIMAL_TAMANHO / 2;
    int destY = static_cast<int>(a.y) - ANIMAL_TAMANHO;
    SDL_Rect destino = {destX, destY, ANIMAL_TAMANHO, ANIMAL_TAMANHO};

    SDL_RendererFlip flip = (a.direcao < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, sheet, &origem, &destino, 0.0, nullptr, flip);
}

inline void desenharIndicadorProduto(SDL_Renderer *renderer, const Animal &a,
                                      const AnimalAssets &assets, Uint32 tempoJogoMs)
{
    if (!a.produtoPronto) return;

    SDL_Texture *icone = nullptr;
    switch (a.tipo)
    {
    case GALINHA: icone = assets.iconeOvo;   break;
    case VACA:    icone = assets.iconeLeite; break;
    case OVELHA:  icone = assets.iconeLa;    break;
    }
    if (!icone) return;

    float t = (tempoJogoMs % 1000) / 1000.0f;
    float pulse = 0.5f + 0.5f * sinf(t * 6.2832f);
    int tam = 24 + static_cast<int>(8 * pulse);

    SDL_Rect dest = {static_cast<int>(a.x) - tam / 2,
                     static_cast<int>(a.y) - ANIMAL_TAMANHO - 16 - tam / 2,
                     tam, tam};
    SDL_RenderCopy(renderer, icone, nullptr, &dest);
}

inline void desenharIndicadorFome(SDL_Renderer *renderer, const Animal &a,
                                   const AnimalAssets &assets)
{
    if (!a.comFome) return;

    int tam = 28;
    int x = static_cast<int>(a.x) - tam / 2;
    int y = static_cast<int>(a.y) - ANIMAL_TAMANHO - 16 - tam / 2;

    if (assets.iconeFome)
    {
        SDL_Rect dest = {x, y, tam, tam};
        SDL_RenderCopy(renderer, assets.iconeFome, nullptr, &dest);
        return;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 200, 60, 60, 235);
    desenharRetanguloArredondado(renderer, x, y, tam, tam, 8);
    SDL_SetRenderDrawColor(renderer, 255, 220, 100, 255);
    desenharContornoArredondado(renderer, x, y, tam, tam, 8);
    SDL_Rect tigela = {x + 6, y + tam / 2, tam - 12, tam / 2 - 4};
    SDL_SetRenderDrawColor(renderer, 80, 50, 30, 255);
    SDL_RenderFillRect(renderer, &tigela);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

inline void desenharAnimais(SDL_Renderer *renderer, const Animal animais[3],
                             const AnimalAssets &assets, Uint32 tempoJogoMs)
{
    int ordem[3] = {0, 1, 2};
    for (int i = 0; i < 2; i++)
        for (int j = i + 1; j < 3; j++)
            if (animais[ordem[i]].y > animais[ordem[j]].y)
            {
                int tmp = ordem[i];
                ordem[i] = ordem[j];
                ordem[j] = tmp;
            }
    for (int k = 0; k < 3; k++)
    {
        const Animal &a = animais[ordem[k]];
        desenharAnimal(renderer, a, assets, tempoJogoMs);
        desenharIndicadorProduto(renderer, a, assets, tempoJogoMs);
        desenharIndicadorFome(renderer, a, assets);
    }
}

inline int animalHitTest(const Animal animais[3], int mouseX, int mouseY)
{
    int margem = ANIMAL_TAMANHO / 2;
    int t = CLICK_TOLERANCIA;
    for (int i = 0; i < 3; i++)
    {
        int ax = static_cast<int>(animais[i].x);
        int ay = static_cast<int>(animais[i].y);
        if (mouseX >= ax - margem - t && mouseX <= ax + margem + t &&
            mouseY >= ay - ANIMAL_TAMANHO - t && mouseY <= ay + t)
            return i;
    }
    return -1;
}

inline void escolherNovoTargetCachorro(Cachorro &c, Uint32 tempoJogoMs)
{
    int margem = ANIMAL_TAMANHO / 2;
    c.targetX = static_cast<float>(CACHORRO_AREA_X + margem +
                                    (rand() % (CACHORRO_AREA_LARGURA - 2 * margem)));
    c.targetY = static_cast<float>(CACHORRO_AREA_Y + margem +
                                    (rand() % (CACHORRO_AREA_ALTURA - 2 * margem)));
    c.timestampProximoTarget = tempoJogoMs + 4000 + (rand() % 3000);
    c.emMovimento = true;
}

inline void atualizarCachorro(Cachorro &c, float dt, Uint32 tempoJogoMs)
{
    float dx = c.targetX - c.x;
    float dy = c.targetY - c.y;
    float dist = sqrtf(dx * dx + dy * dy);

    if (dist > 4.0f && c.emMovimento)
    {
        c.x += (dx / dist) * ANIMAL_VELOCIDADE_PX_S * dt;
        c.y += (dy / dist) * ANIMAL_VELOCIDADE_PX_S * dt;
        c.direcao = (dx < 0.0f) ? -1 : 1;
    }
    else
    {
        c.emMovimento = false;
        if (tempoJogoMs >= c.timestampProximoTarget)
            escolherNovoTargetCachorro(c, tempoJogoMs);
    }
}

inline void desenharCachorro(SDL_Renderer *renderer, const Cachorro &c,
                              const AnimalAssets &assets, Uint32 tempoJogoMs)
{
    if (c.raca < 0 || c.raca > 2) return;

    SDL_Texture *textura = c.emMovimento
                            ? assets.cachorroSheets[c.raca]
                            : assets.cachorroSentados[c.raca];
    if (!textura) return;

    int tamRender = c.emMovimento ? ANIMAL_TAMANHO : ANIMAL_SENTADO_TAM;
    int destX = static_cast<int>(c.x) - tamRender / 2;
    int destY = static_cast<int>(c.y) - tamRender;
    SDL_Rect destino = {destX, destY, tamRender, tamRender};

    SDL_RendererFlip flip = (c.direcao < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    if (c.emMovimento)
    {
        int frame = (tempoJogoMs / ANIMAL_FRAME_TEMPO_MS) % 4;
        SDL_Rect origem = {frame * ANIMAL_TAMANHO, 0, ANIMAL_TAMANHO, ANIMAL_TAMANHO};
        SDL_RenderCopyEx(renderer, textura, &origem, &destino, 0.0, nullptr, flip);
    }
    else
    {
        SDL_RenderCopyEx(renderer, textura, nullptr, &destino, 0.0, nullptr, flip);
    }
}
