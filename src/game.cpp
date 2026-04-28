#include "GameState.h"
#include "Config.h"
#include "Iso.h"
#include "Desenho.h"
#include "Toolbar.h"
#include "Crops.h"
#include "Hud.h"
#include "Loja.h"
#include "Deposito.h"

#include <cmath>

static void inicializarCanteiros(GameState *s)
{
    for (int linha = 0; linha < GRID_LINHAS; linha++)
    {
        for (int coluna = 0; coluna < GRID_COLUNAS; coluna++)
        {
            int indice = linha * GRID_COLUNAS + coluna;
            s->canteiros[indice].coluna = coluna;
            s->canteiros[indice].linha = linha;
            s->canteiros[indice].estado = BLOQUEADO;
            s->canteiros[indice].tipoCrop = -1;
            s->canteiros[indice].estagioCrop = 0;
            s->canteiros[indice].timestampPlantio = 0;
            s->canteiros[indice].temporadaAtual = 0;
        }
    }

    int desbloqueados = 0;
    for (int linha = 0; linha < GRID_LINHAS && desbloqueados < CANTEIROS_INICIAIS; linha++)
    {
        for (int coluna = 0; coluna < GRID_COLUNAS && desbloqueados < CANTEIROS_INICIAIS; coluna++)
        {
            if (coluna < 2)
            {
                int indice = linha * GRID_COLUNAS + coluna;
                s->canteiros[indice].estado = VAZIO;
                desbloqueados++;
            }
        }
    }
}

GAME_API void game_init(GameState *s, SDL_Renderer *renderer)
{
    carregarConfig("assets/config.ini");

    if (s->inicializado)
    {
        return;
    }

    s->fonte = TTF_OpenFont("assets/fonts/Nunito.ttf", 18);
    s->fontePequena = TTF_OpenFont("assets/fonts/Nunito.ttf", 14);
    s->fonteTooltip = TTF_OpenFont("assets/fonts/Nunito.ttf", 16);
    if (s->fonteTooltip)
    {
        TTF_SetFontHinting(s->fonteTooltip, TTF_HINTING_LIGHT);
        TTF_SetFontStyle(s->fonteTooltip, TTF_STYLE_BOLD);
    }

    carregarIconesToolbar(renderer, s->toolbar);
    s->assets = carregarTodosAssets(renderer);
    s->cropAssets = carregarCropAssets(renderer);

    inicializarCanteiros(s);

    s->toolbar.selecionada = CURSOR;
    s->toolbar.sementeSelecionada = -1;
    s->toolbar.painelAberto = false;
    s->moedasVerdes = 0;

    s->tempoJogoMs = 0;
    s->velocidadeTempo = VELOCIDADE_TEMPO_NORMAL;
    s->colheitas = 0;
    s->xp = 0;
    s->ouro = 100;
    s->canteiroHover = -1;
    s->toolbarHover = -1;
    s->mouseX = 0;
    s->mouseY = 0;
    s->solicitouSair = false;
    s->valorDeposito = 0;
    s->lojaAberta = false;
    s->depositoAberto = false;
    s->canteirosComprados = 0;
    s->modoCompraCanteiro = false;

    for (int i = 0; i < TOTAL_CROPS; i++)
    {
        s->inventarioSementes[i] = 0;
    }

    s->inventarioSementes[NABO] = 5;

    for (int i = 0; i < TOTAL_CROPS; i++)
    {
        s->inventarioColhidos[i] = 0;
    }

    s->inicializado = true;
}

static void processarEventos(GameState *s)
{
    SDL_Event evento;
    while (SDL_PollEvent(&evento))
    {
        if (evento.type == SDL_QUIT)
        {
            s->solicitouSair = true;
        }

        if (evento.type == SDL_MOUSEMOTION)
        {
            s->mouseX = evento.motion.x;
            s->mouseY = evento.motion.y;
        }

        if (evento.type == SDL_MOUSEBUTTONDOWN && evento.button.button == SDL_BUTTON_LEFT)
        {

            if (s->lojaAberta)
            {
                int resLoja = lojaHitTest(evento.button.x, evento.button.y);
                if (resLoja == -2)
                {

                    s->lojaAberta = false;
                }
                else if (resLoja >= 0)
                {

                    int preco = TABELA_CROPS[resLoja].precoCompra;
                    if (s->ouro >= preco)
                    {
                        s->ouro -= preco;
                        s->inventarioSementes[resLoja]++;
                    }
                }
                else if (resLoja == -1)
                {

                    s->lojaAberta = false;
                }

                continue;
            }

            if (s->depositoAberto)
            {
                int resDep = depositoHitTest(evento.button.x, evento.button.y);
                if (resDep == -2)
                {
                    s->depositoAberto = false;
                }
                else if (resDep == 100)
                {
                    if (s->valorDeposito > 0)
                    {
                        s->ouro += s->valorDeposito;
                        s->valorDeposito = 0;

                        for (int i = 0; i < TOTAL_CROPS; i++)
                            s->inventarioColhidos[i] = 0;
                    }
                }
                else if (resDep == -1)
                {
                    s->depositoAberto = false;
                }
                continue;
            }

            if (s->toolbar.painelAberto)
            {
                int resultado = painelSementeHitTest(evento.button.x, evento.button.y);
                if (resultado >= 0)
                {
                    s->toolbar.sementeSelecionada = resultado;
                    s->toolbar.painelAberto = false;
                }
                else
                {
                    s->toolbar.painelAberto = false;
                }
            }
            else
            {
                int slotClicado = toolbarHitTest(evento.button.x, evento.button.y);

                if (slotClicado >= 0)
                {
                    Ferramenta nova = static_cast<Ferramenta>(slotClicado);
                    if (nova == SACOLA)
                    {
                        s->toolbar.painelAberto = !s->toolbar.painelAberto;
                        s->toolbar.selecionada = SACOLA;
                    }
                    else
                    {
                        s->toolbar.selecionada = nova;
                        s->toolbar.painelAberto = false;
                    }
                }
                else if (s->canteiroHover >= 0)
                {
                    Canteiro &c = s->canteiros[s->canteiroHover];

                    if (s->modoCompraCanteiro && c.estado == BLOQUEADO)
                    {
                        int preco = PRECO_CANTEIRO_BASE + s->canteirosComprados * PRECO_CANTEIRO_INCREMENTO;
                        if (s->ouro >= preco)
                        {
                            s->ouro -= preco;
                            c.estado = VAZIO;
                            s->canteirosComprados++;
                        }
                        continue;
                    }

                    switch (s->toolbar.selecionada)
                    {
                    case ENXADA:
                        if (c.estado == RESTOS)
                        {
                            c.estado = VAZIO;
                            c.tipoCrop = -1;
                            c.estagioCrop = 0;
                        }
                        break;
                    case SACOLA:
                        if (c.estado == VAZIO && s->toolbar.sementeSelecionada >= 0)
                        {
                            int sem = s->toolbar.sementeSelecionada;

                            if (s->inventarioSementes[sem] > 0)
                            {
                                s->inventarioSementes[sem]--;

                                c.estado = PLANTADO;
                                c.tipoCrop = sem;
                                c.estagioCrop = 1;
                                c.timestampPlantio = s->tempoJogoMs;
                                c.temporadaAtual = 1;
                                c.saude = 100;
                                c.seca = false;
                                c.praga = 0;
                                c.ultimoSorteioEventoMs = s->tempoJogoMs;
                            }
                        }
                        break;
                    case MAO:
                        if (c.estado == MADURO)
                        {
                            s->colheitas++;

                            int ganho = TABELA_CROPS[c.tipoCrop].precoVenda * c.saude / 100;

                            s->valorDeposito += ganho;
                            s->inventarioColhidos[c.tipoCrop]++;

                            c.temporadaAtual++;

                            int totalTemp = TABELA_CROPS[c.tipoCrop].temporadas;

                            if (c.temporadaAtual <= totalTemp)
                            {
                                c.estado = PLANTADO;
                                c.estagioCrop = 1;
                                c.timestampPlantio = s->tempoJogoMs;
                                c.saude = 100;
                                c.seca = false;
                                c.praga = 0;
                                c.ultimoSorteioEventoMs = s->tempoJogoMs;
                            }
                            else
                            {
                                c.estado = RESTOS;
                                c.tipoCrop = -1;
                                c.estagioCrop = 0;
                                c.timestampPlantio = 0;
                                c.temporadaAtual = 0;
                                c.saude = 100;
                                c.seca = false;
                                c.praga = 0;
                            }
                        }
                        break;
                    case REGADOR:
                        if (c.seca)
                        {
                            c.seca = false;
                            s->xp += 2;
                        }
                        break;
                    case REMOVEDOR:
                        if (c.praga == 1)
                        {
                            c.praga = 0;
                            s->xp += 2;
                        }
                        break;
                    case PESTICIDA:
                        if (c.praga == 2)
                        {
                            c.praga = 0;
                            s->xp += 2;
                        }
                    case CURSOR:
                        break;
                    }
                }
            }
        }

        if (evento.type == SDL_KEYDOWN)
        {
            if (evento.key.keysym.sym == SDLK_ESCAPE)
            {
                if (s->toolbar.painelAberto)
                    s->toolbar.painelAberto = false;
                else
                    s->solicitouSair = true;
            }
            if (evento.key.keysym.sym == SDLK_t)
            {
                s->velocidadeTempo = VELOCIDADE_TEMPO_DEBUG;
            }
            if (evento.key.keysym.sym == SDLK_F5)
            {
                carregarConfig("assets/config.ini");
            }

            if (evento.key.keysym.sym == SDLK_l)
            {
                s->lojaAberta = !s->lojaAberta;
            }

            if (evento.key.keysym.sym == SDLK_d)
            {
                s->depositoAberto = !s->depositoAberto;
                if (s->depositoAberto)
                {
                    s->toolbar.painelAberto = false;
                    s->lojaAberta = false;
                }
            }

            if (evento.key.keysym.sym == SDLK_b)
            {
                s->modoCompraCanteiro = !s->modoCompraCanteiro;

                s->lojaAberta = false;
                s->depositoAberto = false;
                s->toolbar.painelAberto = false;
            }
        }

        if (evento.type == SDL_KEYUP)
        {
            if (evento.key.keysym.sym == SDLK_t)
            {
                s->velocidadeTempo = VELOCIDADE_TEMPO_NORMAL;
            }
        }
    }
}

static void atualizarHover(GameState *s)
{
    float colF = telaParaGridColuna(s->mouseX, s->mouseY);
    float linF = telaParaGridLinha(s->mouseX, s->mouseY);
    int col = static_cast<int>(floor(colF));
    int lin = static_cast<int>(floor(linF));

    if (col >= 0 && col < GRID_COLUNAS && lin >= 0 && lin < GRID_LINHAS)
        s->canteiroHover = lin * GRID_COLUNAS + col;
    else
        s->canteiroHover = -1;

    s->toolbarHover = s->toolbar.painelAberto ? -1 : toolbarHitTest(s->mouseX, s->mouseY);
    if (s->toolbarHover >= 0)
        s->canteiroHover = -1;
}

static void atualizarCrescimento(GameState *s)
{
    for (int i = 0; i < GRID_COLUNAS * GRID_LINHAS; i++)
    {
        Canteiro &c = s->canteiros[i];
        if (c.estado != PLANTADO || c.tipoCrop < 0)
            continue;

        Uint32 passadoMs = s->tempoJogoMs - c.timestampPlantio;
        Uint32 totalMs = static_cast<Uint32>(TABELA_CROPS[c.tipoCrop].tempoTotalSegundos) * 1000;

        float fracao = static_cast<float>(passadoMs) / static_cast<float>(totalMs);
        if (fracao > 1.0f)
            fracao = 1.0f;

        int novoEstagio = static_cast<int>(fracao * TOTAL_ESTAGIOS) + 1;
        if (novoEstagio > TOTAL_ESTAGIOS)
            novoEstagio = TOTAL_ESTAGIOS;

        c.estagioCrop = novoEstagio;
        if (fracao >= 1.0f)
            c.estado = MADURO;

        sortearEventoCanteiro(c, s->tempoJogoMs);
    }
}

static void desenharTileEstado(SDL_Renderer *renderer, const Assets &assets, const Canteiro &c, int telaX, int telaY)
{
    switch (c.estado)
    {
    case BLOQUEADO:
        if (assets.tileGramaEscuro)
            desenharTile(renderer, assets.tileGramaEscuro, telaX, telaY);
        else if (assets.tileBloqueado)
            desenharTile(renderer, assets.tileBloqueado, telaX, telaY);
        else
            desenharLosangoPreenchido(renderer, telaX, telaY, 80, 120, 50);
        break;
    case VAZIO:
        if (assets.tileTerra)
            desenharTile(renderer, assets.tileTerra, telaX, telaY);
        else
            desenharLosangoPreenchido(renderer, telaX, telaY, 139, 100, 60);
        break;
    case PLANTADO:
        if (c.seca && assets.tileTerraSeca)
            desenharTile(renderer, assets.tileTerraSeca, telaX, telaY);
        else if (assets.tileTerra)
            desenharTile(renderer, assets.tileTerra, telaX, telaY);
        else
            desenharLosangoPreenchido(renderer, telaX, telaY, 120, 80, 45);
        break;
    case MADURO:
        if (c.seca && assets.tileTerraSeca)
            desenharTile(renderer, assets.tileTerraSeca, telaX, telaY);
        else if (assets.tileTerra)
            desenharTile(renderer, assets.tileTerra, telaX, telaY);
        else
            desenharLosangoPreenchido(renderer, telaX, telaY, 120, 80, 45);
        break;
    case RESTOS:
        if (assets.tileRestos)
            desenharTile(renderer, assets.tileRestos, telaX, telaY);
        else
        {
            desenharLosangoPreenchido(renderer, telaX, telaY, 120, 80, 45);
            SDL_SetRenderDrawColor(renderer, 80, 70, 50, 255);
            SDL_RenderDrawLine(renderer, telaX - 6, telaY, telaX + 6, telaY);
        }
        break;
    }
}

static void renderizar(GameState *s, SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 56, 142, 24, 255);
    SDL_RenderClear(renderer);

    if (s->assets.background)
    {
        SDL_Rect destBg = {0, -50, LARGURA_JANELA, ALTURA_JANELA + 50};
        SDL_RenderCopy(renderer, s->assets.background, nullptr, &destBg);
    }

    for (int linha = 0; linha < GRID_LINHAS; linha++)
    {
        for (int coluna = 0; coluna < GRID_COLUNAS; coluna++)
        {
            int telaX = isoParaTelaX(coluna, linha);
            int telaY = isoParaTelaY(coluna, linha);
            int indice = linha * GRID_COLUNAS + coluna;
            Canteiro &c = s->canteiros[indice];

            desenharSombraTile(renderer, telaX, telaY);
            desenharTileEstado(renderer, s->assets, c, telaX, telaY);

            if (s->modoCompraCanteiro && c.estado == BLOQUEADO)
            {
                Uint8 brilho = 80 + (s->tempoJogoMs / 4) % 80;
                desenharLosangoPreenchidoAlfa(renderer, telaX, telaY, 255, 220, 100, brilho);

                int preco = PRECO_CANTEIRO_BASE + s->canteirosComprados * PRECO_CANTEIRO_INCREMENTO;
                char buffer[32];
                snprintf(buffer, sizeof(buffer), "%d ouro", preco);
                SDL_Color corPreco = {255, 220, 100, 255};
                desenharTexto(renderer, s->fontePequena, buffer, telaX, telaY, corPreco, true);
            }

            if (c.estado == PLANTADO)
            {
                desenharCrop(renderer, s->cropAssets, c.tipoCrop, c.estagioCrop, telaX, telaY);
                if (c.tipoCrop >= 0)
                {
                    Uint32 passado = s->tempoJogoMs - c.timestampPlantio;
                    Uint32 total = static_cast<Uint32>(TABELA_CROPS[c.tipoCrop].tempoTotalSegundos) * 1000;
                    float frac = static_cast<float>(passado) / static_cast<float>(total);
                    desenharBarraProgresso(renderer, telaX, telaY, frac);
                }
                desenharPragaOverlay(renderer, s->cropAssets, c.praga, telaX, telaY, s->tempoJogoMs);
            }
            else if (c.estado == MADURO)
            {
                desenharCrop(renderer, s->cropAssets, c.tipoCrop, TOTAL_ESTAGIOS, telaX, telaY);
                desenharPragaOverlay(renderer, s->cropAssets, c.praga, telaX, telaY, s->tempoJogoMs);
            }

            if (indice == s->canteiroHover)
            {
                desenharLosangoPreenchidoAlfa(renderer, telaX, telaY, 255, 240, 200, 70);
                desenharLosangoPreenchidoAlfa(renderer, telaX, telaY, 255, 250, 220, 45, TILE_ALTURA - 8, TILE_LARGURA - 14);
            }
        }
    }

    if (s->canteiroHover >= 0)
    {
        Canteiro &cHov = s->canteiros[s->canteiroHover];
        int tx = isoParaTelaX(cHov.coluna, cHov.linha);
        int ty = isoParaTelaY(cHov.coluna, cHov.linha);
        desenharTooltipModerno(renderer, s->fonteTooltip,
                               legendaEstadoCanteiro(cHov.estado),
                               tx, ty - TILE_ALTURA / 2 - 4);
    }

    if (s->assets.casa)
    {
        int baseX = g_config.casaX + g_config.casaW / 2;
        int baseY = g_config.casaY + g_config.casaH - 30;
        desenharSombraElipseSuave(renderer, baseX, baseY, g_config.casaW / 2 - 20, 12, 130);

        SDL_Rect destCasa = {g_config.casaX, g_config.casaY, g_config.casaW, g_config.casaH};
        SDL_RenderCopyEx(renderer, s->assets.casa, nullptr, &destCasa, 0.0, nullptr, SDL_FLIP_HORIZONTAL);
    }

    if (s->assets.casaCachorro)
    {
        int baseX = g_config.casinhaX + g_config.casinhaW / 2;
        int baseY = g_config.casinhaY + g_config.casinhaH - 16;
        desenharSombraElipseSuave(renderer, baseX, baseY, g_config.casinhaW / 2 - 10, 7, 130);

        SDL_Rect destCasinha = {g_config.casinhaX, g_config.casinhaY, g_config.casinhaW, g_config.casinhaH};
        SDL_RenderCopyEx(renderer, s->assets.casaCachorro, nullptr, &destCasinha, 0.0, nullptr, SDL_FLIP_HORIZONTAL);
    }

    SDL_Texture *sementeIcone = nullptr;
    if (s->toolbar.sementeSelecionada >= 0 && s->toolbar.sementeSelecionada < TOTAL_CROPS)
    {
        sementeIcone = s->cropAssets.sementes[s->toolbar.sementeSelecionada];
    }

    desenharToolbar(renderer, s->toolbar, sementeIcone);

    if (s->toolbarHover >= 0 && !s->toolbar.painelAberto)
    {
        int sx = slotX(s->toolbarHover);
        int sy = slotY();
        desenharTooltipModerno(renderer, s->fonteTooltip,
                               nomeFerramenta(static_cast<Ferramenta>(s->toolbarHover)),
                               sx + SLOT_TAMANHO / 2, sy);
    }
    desenharDeposito(renderer, s->fonte, s->fontePequena, s->cropAssets, *s);
    desenharLoja(renderer, s->fonte, s->fontePequena, s->cropAssets, *s);
    desenharHUD(renderer, s->fonte, s->xp, s->ouro, s->moedasVerdes);
    desenharPainelSementes(renderer, s->fonte, s->fontePequena, s->cropAssets, s->toolbar);
    desenharCursorFerramenta(renderer, s->toolbar, s->mouseX, s->mouseY, sementeIcone);

    SDL_RenderPresent(renderer);
}

GAME_API void game_frame(GameState *s, SDL_Renderer *renderer, float dt)
{
    s->tempoJogoMs += static_cast<Uint32>(dt * s->velocidadeTempo * 1000.0f);

    processarEventos(s);
    atualizarAnimacoes(s->toolbar, dt);
    atualizarHover(s);
    atualizarCrescimento(s);
    renderizar(s, renderer);
}
