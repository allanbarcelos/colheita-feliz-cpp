#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <cstring>

#include "Constantes.h"
#include "Desenho.h"
#include "Crops.h"
#include "Tipos.h"
#include "Player.h"
#include "HudAssets.h"
#include "GameState.h"
#include "Toolbar.h"

inline void desenharBarraValor(SDL_Renderer *renderer, TTF_Font *fonte,
                                int x, int y, int w, int h,
                                float fracao,
                                Uint8 corR, Uint8 corG, Uint8 corB,
                                const char *textoSobreposto)
{
    if (fracao < 0.0f) fracao = 0.0f;
    if (fracao > 1.0f) fracao = 1.0f;

    int raio = h / 2;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 30, 25, 20, 220);
    desenharRetanguloArredondado(renderer, x, y, w, h, raio);

    int wPreench = static_cast<int>(w * fracao);
    if (wPreench >= raio * 2)
    {
        SDL_SetRenderDrawColor(renderer, corR, corG, corB, 255);
        desenharRetanguloArredondado(renderer, x, y, wPreench, h, raio);
    }
    else if (wPreench > 0)
    {
        SDL_SetRenderDrawColor(renderer, corR, corG, corB, 255);
        SDL_Rect preench = {x + raio - wPreench / 2, y, wPreench, h};
        SDL_RenderFillRect(renderer, &preench);
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 110);
    desenharContornoArredondado(renderer, x, y, w, h, raio);

    if (textoSobreposto && fonte)
    {
        SDL_Color corTxt = {255, 255, 255, 255};
        desenharTextoComContorno(renderer, fonte, textoSobreposto, x + w / 2, y + h / 2, corTxt, true);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

inline void desenharLinhaIconeValor(SDL_Renderer *renderer, TTF_Font *fonte,
                                     SDL_Texture *icone, int valor,
                                     int x, int y, int tamIcone, SDL_Color corTexto)
{
    if (icone)
    {
        SDL_Rect destino = {x, y, tamIcone, tamIcone};
        SDL_RenderCopy(renderer, icone, nullptr, &destino);
    }
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", valor);
    int alturaTexto = fonte ? TTF_FontHeight(fonte) : 18;
    int textoY = y + (tamIcone - alturaTexto) / 2;
    desenharTextoComContorno(renderer, fonte, buffer, x + tamIcone + 8, textoY, corTexto, false);
}

inline void desenharHudEsquerdo(SDL_Renderer *renderer, TTF_Font *fonte, TTF_Font *fontePequena,
                                 TTF_Font *fonteHud,
                                 const HudAssets &h, int xp, int ouro, int moedasVerdes,
                                 int popularidade, int generoJogador)
{
    if (!fonte) return;
    TTF_Font *fonteValor = fonteHud ? fonteHud : fonte;

    const int bx = 16;
    const int by = 16;
    const int bw = 520;
    const int bh = 96;

    if (h.painelStatus)
    {
        SDL_Rect destPainel = {bx, by, bw, bh};
        SDL_RenderCopy(renderer, h.painelStatus, nullptr, &destPainel);
    }
    else
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 42, 31, 20, 220);
        SDL_Rect fundo = {bx, by, bw, bh};
        SDL_RenderFillRect(renderer, &fundo);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    const int avatarTam = 68;
    const int avatarX = bx + 14;
    const int avatarY = by + (bh - avatarTam) / 2;

    SDL_Texture *avatar = (generoJogador == 1) ? h.avatarFeminino : h.avatarMasculino;
    if (avatar)
    {
        SDL_Rect destAvatar = {avatarX, avatarY, avatarTam, avatarTam};
        SDL_RenderCopy(renderer, avatar, nullptr, &destAvatar);
    }

    int nivel = nivelDoJogador(xp);
    char bufferNv[32];
    snprintf(bufferNv, sizeof(bufferNv), "Nivel %d", nivel);
    SDL_Color corNome = {255, 240, 200, 255};

    const int barraX = bx + 96;
    const int barraW = 140;
    const int barraCentroX = barraX + barraW / 2;
    desenharTextoComContorno(renderer, fonteValor, bufferNv, barraCentroX, by + 30, corNome, true);

    int xpAtual = xpAtualNoNivel(xp);
    int xpMax = xpDoNivel(xp);
    float fracaoXp = (xpMax > 0) ? (static_cast<float>(xpAtual) / static_cast<float>(xpMax)) : 0.0f;
    char bufferXp[32];
    snprintf(bufferXp, sizeof(bufferXp), "%d / %d", xpAtual, xpMax);
    desenharBarraValor(renderer, fonte, barraX, by + 52, barraW, 22,
                       fracaoXp, 52, 152, 219, bufferXp);

    SDL_Color corValor = {255, 255, 255, 255};
    const int icTam = 34;
    const int linhaY = by + (bh - icTam) / 2;
    desenharLinhaIconeValor(renderer, fonteValor, h.iconeOuro,       ouro,         bx + 252, linhaY, icTam, corValor);
    desenharLinhaIconeValor(renderer, fonteValor, h.iconeMoedaVerde, moedasVerdes, bx + 336, linhaY, icTam, corValor);
    desenharLinhaIconeValor(renderer, fonteValor, h.iconeCoracao,    popularidade, bx + 416, linhaY, icTam, corValor);
}

static constexpr int HUD_DIREITO_TOTAL = 6;
static constexpr int HUD_DIREITO_TAM = 60;
static constexpr int HUD_DIREITO_PADDING = 8;
static constexpr int HUD_DIREITO_GAP = 18;
static constexpr int HUD_DIREITO_Y = 22;

inline int hudDireitoCardW() { return HUD_DIREITO_TAM + HUD_DIREITO_PADDING * 2; }
inline int hudDireitoCardH() { return HUD_DIREITO_TAM + HUD_DIREITO_PADDING * 2; }
inline int hudDireitoStep()  { return hudDireitoCardW() + HUD_DIREITO_GAP; }

inline int hudDireitoStartX()
{
    int totalLargura = HUD_DIREITO_TOTAL * hudDireitoCardW() + (HUD_DIREITO_TOTAL - 1) * HUD_DIREITO_GAP;
    return LARGURA_JANELA - totalLargura - 24;
}

inline int botaoHudCardX(int indice) { return hudDireitoStartX() + indice * hudDireitoStep(); }
inline int botaoHudX(int indice)     { return botaoHudCardX(indice) + HUD_DIREITO_PADDING; }
inline int botaoHudY()               { return HUD_DIREITO_Y + HUD_DIREITO_PADDING; }
inline int botaoHudTamanho()         { return HUD_DIREITO_TAM; }

inline void desenharBotaoHud(SDL_Renderer *renderer, TTF_Font *fontePequena,
                              SDL_Texture *icone, const char *label,
                              int indice, bool hover)
{
    int bx = botaoHudX(indice);
    int by = botaoHudY();
    int tam = botaoHudTamanho();

    const int raio = 14;
    int cardX = botaoHudCardX(indice);
    int cardY = HUD_DIREITO_Y;
    int cardW = hudDireitoCardW();
    int cardH = hudDireitoCardH();

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 80);
    desenharRetanguloArredondado(renderer, cardX + 2, cardY + 3, cardW, cardH, raio);

    if (hover)
        SDL_SetRenderDrawColor(renderer, 60, 44, 28, 235);
    else
        SDL_SetRenderDrawColor(renderer, 42, 31, 20, 215);
    desenharRetanguloArredondado(renderer, cardX, cardY, cardW, cardH, raio);

    if (hover)
        SDL_SetRenderDrawColor(renderer, 255, 215, 100, 255);
    else
        SDL_SetRenderDrawColor(renderer, 180, 140, 60, 220);
    desenharContornoArredondado(renderer, cardX, cardY, cardW, cardH, raio);

    if (icone)
    {
        SDL_Rect destIcone = {bx, by, tam, tam};
        SDL_RenderCopy(renderer, icone, nullptr, &destIcone);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    int alturaLabel = fontePequena ? TTF_FontHeight(fontePequena) : 16;
    int tw = 0, th = 0;
    if (fontePequena) TTF_SizeUTF8(fontePequena, label, &tw, &th);

    int pillPadX = 8;
    int pillPadY = 3;
    int pillW = tw + pillPadX * 2;
    int pillH = alturaLabel + pillPadY * 2;
    int pillX = cardX + cardW / 2 - pillW / 2;
    int pillY = cardY + cardH + 6;
    int pillRaio = pillH / 2;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 70);
    desenharRetanguloArredondado(renderer, pillX + 1, pillY + 2, pillW, pillH, pillRaio);
    SDL_SetRenderDrawColor(renderer, 28, 22, 16, 230);
    desenharRetanguloArredondado(renderer, pillX, pillY, pillW, pillH, pillRaio);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    SDL_Color corLabel = hover ? SDL_Color{255, 235, 130, 255} : SDL_Color{255, 240, 210, 255};
    desenharTextoComContorno(renderer, fontePequena, label, cardX + cardW / 2, pillY + pillH / 2, corLabel, true);
}

inline void desenharHudDireito(SDL_Renderer *renderer, TTF_Font *fontePequena,
                                const HudAssets &h, int botaoHover)
{
    for (int i = 0; i < HUD_DIREITO_TOTAL; i++)
    {
        desenharBotaoHud(renderer, fontePequena, h.botoes[i],
                         nomeBotaoHud(i), i, i == botaoHover);
    }
}

inline int hudDireitoHitTest(int mouseX, int mouseY)
{
    int cardY = HUD_DIREITO_Y;
    int cardH = hudDireitoCardH();
    int t = CLICK_TOLERANCIA;
    if (mouseY < cardY - t || mouseY > cardY + cardH + t) return -1;

    int cardW = hudDireitoCardW();
    for (int i = 0; i < HUD_DIREITO_TOTAL; i++)
    {
        int cardX = botaoHudCardX(i);
        if (mouseX >= cardX - t && mouseX <= cardX + cardW + t)
            return i;
    }
    return -1;
}

inline void desenharTooltipPlanta(SDL_Renderer *renderer, TTF_Font *fonte,
                                   const Canteiro &c, Uint32 tempoJogoMs,
                                   int telaX, int telaY)
{
    if (!fonte || c.tipoCrop < 0) return;
    if (c.estado != PLANTADO && c.estado != MADURO) return;

    const DadosCrop &dados = TABELA_CROPS[c.tipoCrop];

    char linha1[64];
    snprintf(linha1, sizeof(linha1), "%s - Temporada %d/%d",
             dados.nome, c.temporadaAtual, dados.temporadas);

    char linha2[80];
    if (c.estado == MADURO)
    {
        snprintf(linha2, sizeof(linha2), "Maduro - Pronto pra colher!");
    }
    else
    {
        Uint32 totalMs = static_cast<Uint32>(dados.tempoTotalSegundos) * 1000;
        Uint32 passadoMs = tempoJogoMs - c.timestampPlantio;
        int restanteSeg = (totalMs > passadoMs) ? (totalMs - passadoMs) / 1000 : 0;
        snprintf(linha2, sizeof(linha2), "Estagio %d/%d - %ds ate maduro",
                 c.estagioCrop, TOTAL_ESTAGIOS, restanteSeg);
    }

    char linha3[64];
    snprintf(linha3, sizeof(linha3), "Saude: %d%%", c.saude);

    int padding = 8;
    int alturaLinha = 18;
    int largura = 240;
    int altura = padding * 2 + alturaLinha * 3;

    int caixaX = telaX - largura / 2;
    int caixaY = telaY - altura - 12;

    if (caixaX < 4) caixaX = 4;
    if (caixaX + largura > LARGURA_JANELA - 4) caixaX = LARGURA_JANELA - largura - 4;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
    SDL_Rect sombra = {caixaX + 3, caixaY + 3, largura, altura};
    SDL_RenderFillRect(renderer, &sombra);

    SDL_SetRenderDrawColor(renderer, 180, 140, 60, 255);
    SDL_Rect borda = {caixaX - 1, caixaY - 1, largura + 2, altura + 2};
    SDL_RenderFillRect(renderer, &borda);

    SDL_SetRenderDrawColor(renderer, 30, 22, 15, 240);
    SDL_Rect fundo = {caixaX, caixaY, largura, altura};
    SDL_RenderFillRect(renderer, &fundo);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    SDL_Color corNome = {255, 235, 180, 255};
    SDL_Color corInfo = {220, 220, 220, 255};
    SDL_Color corSaude;
    if      (c.saude >= 80) corSaude = {120, 255, 120, 255};
    else if (c.saude >= 40) corSaude = {255, 215, 100, 255};
    else                    corSaude = {255, 110, 110, 255};

    int textoX = caixaX + padding;
    int textoY = caixaY + padding;
    desenharTexto(renderer, fonte, linha1, textoX, textoY,                   corNome,  false);
    desenharTexto(renderer, fonte, linha2, textoX, textoY + alturaLinha,     corInfo,  false);
    desenharTexto(renderer, fonte, linha3, textoX, textoY + 2 * alturaLinha, corSaude, false);
}

inline void adicionarLog(GameState *s, const char *mensagem)
{
    for (int i = 4; i > 0; i--)
    {
        memcpy(s->logMensagens[i], s->logMensagens[i - 1], sizeof(s->logMensagens[0]));
    }
    snprintf(s->logMensagens[0], sizeof(s->logMensagens[0]), "%s", mensagem);
    if (s->logCount < 5) s->logCount++;
}

inline void desenharLogEventos(SDL_Renderer *renderer, TTF_Font *fontePequena, const GameState &s)
{
    if (s.logCount == 0) return;

    const int largura = 440;
    const int alturaLinha = 24;
    const int paddingX = 16;
    const int paddingY = 14;
    const int alturaTitulo = 22;
    const int alturaTotal = paddingY * 2 + alturaTitulo + s.logCount * alturaLinha;

    const int x = 16;
    const int y = ALTURA_JANELA - TOOLBAR_ALTURA - alturaTotal - 16;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 80);
    desenharRetanguloArredondado(renderer, x + 2, y + 3, largura, alturaTotal, 12);

    SDL_SetRenderDrawColor(renderer, 180, 140, 60, 230);
    desenharRetanguloArredondado(renderer, x - 2, y - 2, largura + 4, alturaTotal + 4, 12);

    SDL_SetRenderDrawColor(renderer, 30, 22, 15, 230);
    desenharRetanguloArredondado(renderer, x, y, largura, alturaTotal, 12);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    SDL_Color corTitulo = {255, 215, 100, 255};
    desenharTexto(renderer, fontePequena, "Assistente da Fazenda",
                  x + paddingX, y + paddingY - 2, corTitulo, false);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 180, 140, 60, 110);
    int linhaY = y + paddingY + alturaTitulo - 2;
    SDL_RenderDrawLine(renderer, x + paddingX, linhaY, x + largura - paddingX, linhaY);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    int textoBaseY = y + paddingY + alturaTitulo + 4;
    for (int i = 0; i < s.logCount; i++)
    {
        Uint8 alpha = static_cast<Uint8>(255 - i * 32);
        SDL_Color cor = {255, 235, 180, alpha};
        desenharTexto(renderer, fontePequena, s.logMensagens[i],
                      x + paddingX, textoBaseY + i * alturaLinha, cor, false);
    }
}
