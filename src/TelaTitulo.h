#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include "Constantes.h"
#include "Desenho.h"
#include "Toolbar.h"
#include "GameState.h"

// NOVO (fase 11): glow radial procedural — substitui PNG.
// Cria textura RGBA com falloff quadratico (gaussian-like) do centro pra borda.
// Center: branco puro alpha 255, edge: alpha 0 + cream-yellow tint.
// Usado com SDL_BLENDMODE_ADD em desenharTelaTitulo. Tunavel sem regerar asset.
inline SDL_Texture *gerarGlowRadial(SDL_Renderer *renderer, int size = 512)
{
    SDL_Surface *surf = SDL_CreateRGBSurfaceWithFormat(0, size, size, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surf) return nullptr;
    SDL_LockSurface(surf);
    Uint32 *pixels = static_cast<Uint32*>(surf->pixels);
    int pitch = surf->pitch / 4;
    float cx = size * 0.5f;
    float cy = size * 0.5f;
    float maxR = size * 0.5f;
    SDL_PixelFormat *fmt = surf->format;
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            float dx = x - cx;
            float dy = y - cy;
            float dist = sqrtf(dx * dx + dy * dy);
            float t = dist / maxR;
            if (t > 1.0f) t = 1.0f;
            // Falloff quadratico — gaussian-like, smooth edge
            float falloff = (1.0f - t) * (1.0f - t);
            Uint8 a = static_cast<Uint8>(255.0f * falloff);
            // Cor: branco no centro, cream-yellow na borda
            Uint8 r = 255;
            Uint8 g = static_cast<Uint8>(255 - 22 * t);   // 255 → 233
            Uint8 b = static_cast<Uint8>(255 - 87 * t);   // 255 → 168
            pixels[y * pitch + x] = SDL_MapRGBA(fmt, r, g, b, a);
        }
    }
    SDL_UnlockSurface(surf);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    return tex;
}

static const char *TIPS[] = {
    "Aperte T pra acelerar o tempo 60x",
    "Cuidados com pragas e seca dao XP — fique de olho",
    "Saude da planta = preco final na colheita",
    "Plante varios tipos pra completar missoes diarias",
    "A caixa de presente diaria sempre tem algo bom",
    "Uva e Melancia desbloqueiam no Nv 8 — vale o esforco",
    "Pressione P pra colocar decoracao, R pra rotacionar",
    "Ctrl+S salva o jogo manualmente"
};
static constexpr int TOTAL_TIPS = sizeof(TIPS) / sizeof(TIPS[0]);

inline SDL_Rect rectBotaoTitulo(int idx)
{
    return SDL_Rect{(LARGURA_JANELA - 360) / 2, 480 + idx * 86, 360, 70};
}

inline SDL_Rect rectIconeRodape(int idx)
{
    return SDL_Rect{24 + idx * 100, ALTURA_JANELA - 56, 88, 36};
}

inline void atualizarTelaTitulo(GameState &s, float dt)
{
    Uint32 agora = SDL_GetTicks();

    if (agora - s.tipUltimaTrocaMs > 3000)
    {
        s.tipAtualIndex = (s.tipAtualIndex + 1) % TOTAL_TIPS;
        s.tipUltimaTrocaMs = agora;
    }

    s.passaroX1 += 30.0f * dt;
    if (s.passaroX1 > LARGURA_JANELA + 64) s.passaroX1 = -64;
    s.passaroY1 = 180 + sinf(s.passaroX1 * 0.01f) * 24.0f;

    s.passaroX2 -= 25.0f * dt;
    if (s.passaroX2 < -64) s.passaroX2 = LARGURA_JANELA + 64;
    s.passaroY2 = 280 + cosf(s.passaroX2 * 0.012f) * 18.0f;
}

inline void desenharTelaTitulo(SDL_Renderer *renderer, TTF_Font *fonte, TTF_Font *fonteHud,
                                TTF_Font *fontePequena, const GameState &s)
{
    // NOVO (fase 11): prefere bg dedicado MJ-quality. Fallback: farm in-game da fase 1.
    SDL_Texture *bgTex = s.tituloBackground ? s.tituloBackground : s.assets.background;
    if (bgTex)
    {
        SDL_Rect destBg = {0, 0, LARGURA_JANELA, ALTURA_JANELA};
        SDL_RenderCopy(renderer, bgTex, nullptr, &destBg);

        // Overlay alpha 50 com MJ (mood ja adequado), 80 com fallback farm.
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, s.tituloBackground ? 50 : 80);
        SDL_RenderFillRect(renderer, &destBg);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, 168, 197, 68, 255);
        SDL_RenderClear(renderer);
    }

    if (s.tituloPassaros)
    {
        // NOVO (fase 11): cellW dinamico — Nano Banana 2 gera sheets em alta resolucao
        // (~4096x1016 com 4 cells de 1024 cada). Calcula cellW pelo tamanho real do PNG.
        int sheetW, sheetH;
        SDL_QueryTexture(s.tituloPassaros, nullptr, nullptr, &sheetW, &sheetH);
        int cellW = sheetW / 4;
        int frame = (SDL_GetTicks() / 165) % 4;
        SDL_Rect origem = {frame * cellW, 0, cellW, sheetH};
        SDL_Rect dest1 = {static_cast<int>(s.passaroX1), static_cast<int>(s.passaroY1), 64, 64};
        SDL_RenderCopy(renderer, s.tituloPassaros, &origem, &dest1);
        SDL_Rect dest2 = {static_cast<int>(s.passaroX2), static_cast<int>(s.passaroY2), 64, 64};
        SDL_RenderCopyEx(renderer, s.tituloPassaros, &origem, &dest2, 0.0, nullptr, SDL_FLIP_HORIZONTAL);
    }

    // NOVO (fase 11): glow pulsante ATRAS do logo via SDL_BLENDMODE_ADD.
    // Alpha breathing 96..192 numa onda senoidal de ~1Hz. Cor warm amber via ColorMod.
    if (s.tituloGlow)
    {
        Uint8 alphaBreath = static_cast<Uint8>(144 + 48 * sinf(SDL_GetTicks() / 1000.0f));
        SDL_SetTextureBlendMode(s.tituloGlow, SDL_BLENDMODE_ADD);
        SDL_SetTextureAlphaMod(s.tituloGlow, alphaBreath);
        SDL_SetTextureColorMod(s.tituloGlow, 255, 230, 150);  // warm amber
        SDL_Rect destGlow = {(LARGURA_JANELA - 700) / 2, 30, 700, 350};
        SDL_RenderCopy(renderer, s.tituloGlow, nullptr, &destGlow);
    }

    // NOVO (fase 11): sparkles 4 frames atras do logo, em 3 spots ao redor das letras.
    if (s.tituloSparkles)
    {
        int sheetW, sheetH;
        SDL_QueryTexture(s.tituloSparkles, nullptr, nullptr, &sheetW, &sheetH);
        int cellW = sheetW / 4;
        int frame = (SDL_GetTicks() / 180) % 4;
        SDL_Rect origem = {frame * cellW, 0, cellW, sheetH};
        SDL_SetTextureBlendMode(s.tituloSparkles, SDL_BLENDMODE_BLEND);
        const SDL_Rect spots[3] = {
            {LARGURA_JANELA / 2 - 380, 110, 64, 64},
            {LARGURA_JANELA / 2 + 320, 130, 64, 64},
            {LARGURA_JANELA / 2 - 30,  300, 64, 64},
        };
        for (int i = 0; i < 3; i++)
            SDL_RenderCopy(renderer, s.tituloSparkles, &origem, &spots[i]);
    }

    if (s.tituloLogo)
    {
        // NOVO (fase 11): aspect dinamico — PNG do Nano nem sempre vem em 1024x320 esperado.
        // Mantem aspect ratio real, fixando largura em 700 e calculando altura proporcional.
        int logoW, logoH;
        SDL_QueryTexture(s.tituloLogo, nullptr, nullptr, &logoW, &logoH);
        int destW = 700;
        int destH = (logoH > 0) ? (destW * logoH / logoW) : 250;
        SDL_Rect destLogo = {(LARGURA_JANELA - destW) / 2, 60, destW, destH};
        SDL_RenderCopy(renderer, s.tituloLogo, nullptr, &destLogo);
    }
    else
    {
        SDL_Color corTitulo = {255, 245, 220, 255};
        desenharTextoComContorno(renderer, fonteHud, "Fazenda dos Sonhos", LARGURA_JANELA / 2, 200, corTitulo, true);
    }


    // NOVO (fase 11): botoes cartoon LIMPO — drop shadow + outline dark-brown + body solido.
    // Sem bands chapados. Hover = grow 4px each side + brilha levemente.
    const char *labels[3] = {"NOVO JOGO", "CONTINUAR", "CONFIGURAÇÕES"};
    for (int i = 0; i < 3; i++)
    {
        SDL_Rect btn = rectBotaoTitulo(i);
        bool hover = (s.botaoTituloHover == i);
        bool disabled = (i == 1 && !s.saveExiste);

        // Hover: cresce levemente (4px expand pra cada lado)
        if (hover && !disabled) {
            btn.x -= 4; btn.y -= 4; btn.w += 8; btn.h += 8;
        }

        // 1. Drop shadow diffuso (offset 4,6, alpha 120)
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 120);
        desenharRetanguloArredondado(renderer, btn.x + 4, btn.y + 6, btn.w, btn.h, 16);

        // 2. Outline dark-brown 2px (cresce rect 2px each side)
        SDL_SetRenderDrawColor(renderer, 60, 38, 18, 255);
        desenharRetanguloArredondado(renderer, btn.x - 2, btn.y - 2, btn.w + 4, btn.h + 4, 17);

        // 3. Body solido (sem bands artificiais — limpo)
        Uint8 r, g, b;
        if (disabled)            { r =  90; g =  82; b =  72; }
        else if (i == 0)         { r = static_cast<Uint8>(hover ?  95 : 76);  g = static_cast<Uint8>(hover ? 200 : 175); b = static_cast<Uint8>(hover ?  95 : 80); }
        else if (i == 1)         { r = static_cast<Uint8>(hover ? 230 : 200); g = static_cast<Uint8>(hover ? 165 : 140); b = static_cast<Uint8>(hover ?  50 : 30); }
        else                     { r = static_cast<Uint8>(hover ? 100 :  80); g = static_cast<Uint8>(hover ? 130 : 100); b = static_cast<Uint8>(hover ? 200 : 160); }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        desenharRetanguloArredondado(renderer, btn.x, btn.y, btn.w, btn.h, 16);

        // 4. Texto branco com contorno preto (legibilidade total sobre qualquer cor)
        SDL_Color corBtn = disabled ? SDL_Color{190, 178, 160, 255} : SDL_Color{255, 255, 255, 255};
        desenharTextoComContorno(renderer, fonteHud, labels[i], btn.x + btn.w / 2, btn.y + btn.h / 2, corBtn, true);
    }

    // NOVO (fase 11): tip pill — ABAIXO dos 3 botoes (y=750) com fundo dark-brown OPACO
    // pra evitar bg vazar (path+grass do MJ tem cores diferentes que apareciam atraves de alpha).
    {
        char buffer[160];
        snprintf(buffer, sizeof(buffer), "%s", TIPS[s.tipAtualIndex]);
        int textW = 0, textH = 0;
        TTF_SizeUTF8(fontePequena, buffer, &textW, &textH);
        int padX = 28;
        int padY = 10;
        int pillW = textW + padX * 2;
        int pillH = textH + padY * 2;
        int pillY = 420;
        SDL_Rect pill = {(LARGURA_JANELA - pillW) / 2, pillY, pillW, pillH};

        // 1. Drop shadow
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
        desenharRetanguloArredondado(renderer, pill.x + 2, pill.y + 3, pill.w, pill.h, pillH / 2);

        // 2. Outline dark-brown (cresce 2px each side)
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer, 40, 26, 12, 255);
        desenharRetanguloArredondado(renderer, pill.x - 2, pill.y - 2, pill.w + 4, pill.h + 4, (pillH + 4) / 2);

        // 3. Body OPACO dark-brown (alpha 255 — bg nao vaza mais)
        SDL_SetRenderDrawColor(renderer, 70, 50, 30, 255);
        desenharRetanguloArredondado(renderer, pill.x, pill.y, pill.w, pill.h, pillH / 2);

        // 4. Texto cream-yellow centrado
        SDL_Color corTip = {255, 245, 220, 255};
        desenharTextoComContorno(renderer, fontePequena, buffer, LARGURA_JANELA / 2, pillY + pillH / 2, corTip, true);
    }

    SDL_Texture *iconesRodape[3] = {s.tituloIconeGithub, s.tituloIconeLivepix, s.tituloIconeDiscord};
    const char *fallbackLabel[3] = {"GitHub", "PIX", "Discord"};
    for (int i = 0; i < 3; i++)
    {
        SDL_Rect dest = rectIconeRodape(i);
        if (s.rodapeIconeHover == i)
        {
            dest.x -= 2; dest.y -= 2; dest.w += 4; dest.h += 4;
        }
        if (iconesRodape[i])
        {
            SDL_RenderCopy(renderer, iconesRodape[i], nullptr, &dest);
        }
        else
        {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 120);
            desenharRetanguloArredondado(renderer, dest.x + 2, dest.y + 3, dest.w, dest.h, 10);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

            SDL_SetRenderDrawColor(renderer, 60, 38, 18, 255);
            desenharRetanguloArredondado(renderer, dest.x - 2, dest.y - 2, dest.w + 4, dest.h + 4, 11);

            bool hover = (s.rodapeIconeHover == i);
            SDL_SetRenderDrawColor(renderer, hover ? 96 : 70, hover ? 70 : 50, hover ? 38 : 28, 255);
            desenharRetanguloArredondado(renderer, dest.x, dest.y, dest.w, dest.h, 10);

            SDL_Color corLbl = {255, 245, 220, 255};
            desenharTextoComContorno(renderer, fontePequena, fallbackLabel[i],
                                      dest.x + dest.w / 2, dest.y + dest.h / 2, corLbl, true);
        }
    }

    SDL_Color corVersao = {255, 245, 220, 200};
    const char *versaoTxt = "Fazenda dos Sonhos · v1.0.0-desktop · feito em C++";
    int versaoW = 0, versaoH = 0;
    TTF_SizeUTF8(fontePequena, versaoTxt, &versaoW, &versaoH);
    desenharTextoComContorno(renderer, fontePequena, versaoTxt,
                              LARGURA_JANELA - versaoW - 24,
                              ALTURA_JANELA - versaoH - 16,
                              corVersao, false);
}

inline int telaTituloHitTest(int mouseX, int mouseY, bool saveExiste)
{
    int t = CLICK_TOLERANCIA;
    for (int i = 0; i < 3; i++)
    {
        SDL_Rect btn = rectBotaoTitulo(i);
        if (mouseX >= btn.x - t && mouseX <= btn.x + btn.w + t &&
            mouseY >= btn.y - t && mouseY <= btn.y + btn.h + t)
        {
            if (i == 1 && !saveExiste) return -1;
            return 100 + i;
        }
    }
    for (int i = 0; i < 3; i++)
    {
        SDL_Rect ic = rectIconeRodape(i);
        if (mouseX >= ic.x - t && mouseX <= ic.x + ic.w + t &&
            mouseY >= ic.y - t && mouseY <= ic.y + ic.h + t)
            return 200 + i;
    }
    return -1;
}
