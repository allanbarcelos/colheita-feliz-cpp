#include "GameState.h"
#include "Config.h"
#include "Iso.h"
#include "Desenho.h"
#include "Toolbar.h"
#include "Crops.h"
#include "Hud.h"
#include "Loja.h"
#include "Deposito.h"
#include "Missoes.h"
#include "PainelMissoes.h"
#include "RecompensaDiaria.h"
#include "AnimalLogica.h"

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

    s->fonte = TTF_OpenFont("assets/fonts/Poppins-SemiBold.ttf", 17);
    s->fontePequena = TTF_OpenFont("assets/fonts/Poppins-Bold.ttf", 14);
    s->fonteTooltip = TTF_OpenFont("assets/fonts/Poppins-SemiBold.ttf", 16);
    s->fonteHud = TTF_OpenFont("assets/fonts/Poppins-Bold.ttf", 20);
    if (s->fonte)         TTF_SetFontHinting(s->fonte,         TTF_HINTING_LIGHT);
    if (s->fontePequena)  TTF_SetFontHinting(s->fontePequena,  TTF_HINTING_LIGHT);
    if (s->fonteTooltip)  TTF_SetFontHinting(s->fonteTooltip,  TTF_HINTING_LIGHT);
    if (s->fonteHud)      TTF_SetFontHinting(s->fonteHud,      TTF_HINTING_LIGHT);

    carregarIconesToolbar(renderer, s->toolbar);
    s->assets = carregarTodosAssets(renderer);
    s->cropAssets = carregarCropAssets(renderer);
    s->hudAssets = carregarHudAssets(renderer);

    inicializarCanteiros(s);

    s->toolbar.selecionada = CURSOR;
    s->toolbar.sementeSelecionada = -1;
    s->toolbar.painelAberto = false;
    s->moedasVerdes = 0;
    s->popularidade = 0;
    s->generoJogador = 0;

    s->tempoJogoMs = 0;
    s->velocidadeTempo = VELOCIDADE_TEMPO_NORMAL;
    s->colheitas = 0;
    s->xp = 0;
    s->ouro = 100;
    s->canteiroHover = -1;
    s->toolbarHover = -1;
    s->hudDireitoHover = -1;
    s->mouseX = 0;
    s->mouseY = 0;
    s->solicitouSair = false;
    s->valorDeposito = 0;
    s->lojaAberta = false;
    s->depositoAberto = false;
    s->canteirosComprados = 0;
    s->modoCompraCanteiro = false;

    s->logCount = 0;
    for (int i = 0; i < 5; i++)
        s->logMensagens[i][0] = '\0';

    sortearMissoesDiarias(s->missoesDiarias);
    s->timestampUltimoResetMissoes = 0;
    s->painelMissoesAberto = false;
    s->painelMissoesAbertura = 0.0f;
    s->lojaAbertura = 0.0f;
    s->depositoAbertura = 0.0f;
    s->ultimoDiaRecompensa = -1;
    s->recompensaDisponivel = true;

    s->animalAssets = carregarAnimalAssets(renderer);

    s->animais[0] = { GALINHA, 1331.0f, 542.0f, 1, 1331.0f, 542.0f, 0, 0, false, false, 0 };
    s->animais[1] = { VACA,    1331.0f, 542.0f, 1, 1331.0f, 542.0f, 0, 0, false, false, 0 };
    s->animais[2] = { OVELHA,  1331.0f, 542.0f, 1, 1331.0f, 542.0f, 0, 0, false, false, 0 };

    s->cachorro.raca = GOLDEN;
    s->cachorro.x = static_cast<float>(CACHORRO_AREA_X + CACHORRO_AREA_LARGURA / 2);
    s->cachorro.y = static_cast<float>(CACHORRO_AREA_Y + CACHORRO_AREA_ALTURA / 2);
    s->cachorro.direcao = 1;
    s->cachorro.targetX = s->cachorro.x;
    s->cachorro.targetY = s->cachorro.y;
    s->cachorro.timestampProximoTarget = 0;
    s->cachorro.emMovimento = false;

    s->inventarioRacao = RACAO_INICIAL;
    for (int i = 0; i < 3; i++) s->inventarioProdutos[i] = 0;
    s->animalHover = -1;
    s->debugAtivo = false;

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
            if (s->debugAtivo)
            {
                char dbg[64];
                snprintf(dbg, sizeof(dbg), "Click @ (%d, %d)", evento.button.x, evento.button.y);
                adicionarLog(s, dbg);
            }

            if (caixaRecompensaClicada(evento.button.x, evento.button.y, s->recompensaDisponivel))
            {
                char msg[96];
                sortearRecompensaDiaria(&s->ouro, &s->moedasVerdes,
                                         s->inventarioSementes, msg, sizeof(msg));
                adicionarLog(s, msg);
                s->recompensaDisponivel = false;
                s->ultimoDiaRecompensa = static_cast<int>(s->tempoJogoMs / RESET_DIARIO_MS);
                continue;
            }

            if (s->painelMissoesAberto)
            {
                int hit = painelMissoesHitTest(evento.button.x, evento.button.y, s->missoesDiarias);
                if (hit == -2)
                {
                    s->painelMissoesAberto = false;
                }
                else if (hit >= 0)
                {
                    Missao &m = s->missoesDiarias[hit];
                    if (m.concluida && !m.coletada)
                    {
                        s->ouro += m.recompensaOuro;
                        s->moedasVerdes += m.recompensaVerdes;
                        s->xp += m.recompensaXp;
                        m.coletada = true;
                        char msg[96];
                        snprintf(msg, sizeof(msg), "Missao coletada (+%d ouro, +%d verdes, +%d XP)",
                                 m.recompensaOuro, m.recompensaVerdes, m.recompensaXp);
                        adicionarLog(s, msg);
                    }
                }
                else if (hit == -1)
                {
                    s->painelMissoesAberto = false;
                }
                continue;
            }

            int botaoHud = hudDireitoHitTest(evento.button.x, evento.button.y);
            if (botaoHud >= 0)
            {
                switch (botaoHud)
                {
                case 0:
                    s->depositoAberto = !s->depositoAberto;
                    s->lojaAberta = false;
                    s->modoCompraCanteiro = false;
                    s->toolbar.painelAberto = false;
                    s->painelMissoesAberto = false;
                    break;
                case 1:
                    s->lojaAberta = !s->lojaAberta;
                    s->depositoAberto = false;
                    s->modoCompraCanteiro = false;
                    s->toolbar.painelAberto = false;
                    s->painelMissoesAberto = false;
                    break;
                case 2:
                    adicionarLog(s, "Amigos em breve (fase 16)");
                    break;
                case 3:
                    adicionarLog(s, "Ranking em breve");
                    break;
                case 4:
                    s->painelMissoesAberto = !s->painelMissoesAberto;
                    s->lojaAberta = false;
                    s->depositoAberto = false;
                    break;
                }
                continue;
            }

            if (s->lojaAberta)
            {
                int resLoja = lojaHitTest(evento.button.x, evento.button.y);
                if (resLoja == -2)
                {

                    s->lojaAberta = false;
                }
                else if (resLoja == 999)
                {
                    if (s->ouro >= PRECO_RACAO)
                    {
                        s->ouro -= PRECO_RACAO;
                        s->inventarioRacao++;
                        char msg[96];
                        snprintf(msg, sizeof(msg), "Comprou 1 Racao (-%d ouro)", PRECO_RACAO);
                        adicionarLog(s, msg);
                    }
                }
                else if (resLoja >= 0)
                {
                    if (!atingiuNivel(s->xp, TABELA_CROPS[resLoja].nivelDesbloqueio))
                    {
                        char msg[96];
                        snprintf(msg, sizeof(msg), "%s desbloqueia no Nv %d",
                                 TABELA_CROPS[resLoja].nome, TABELA_CROPS[resLoja].nivelDesbloqueio);
                        adicionarLog(s, msg);
                        continue;
                    }

                    int preco = TABELA_CROPS[resLoja].precoCompra;
                    if (s->ouro >= preco)
                    {
                        s->ouro -= preco;
                        s->inventarioSementes[resLoja]++;
                        char msg[96];
                        snprintf(msg, sizeof(msg), "Comprou 1 %s (-%d ouro)",
                                 TABELA_CROPS[resLoja].nome, preco);
                        adicionarLog(s, msg);
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
                                              char msg[96];
                        snprintf(msg, sizeof(msg), "Vendeu deposito (+%d ouro)", s->valorDeposito);
                        adicionarLog(s, msg);

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

            int animalClicado = animalHitTest(s->animais, evento.button.x, evento.button.y);
            if (animalClicado >= 0)
            {
                Animal &a = s->animais[animalClicado];

                if (a.produtoPronto)
                {
                    s->inventarioProdutos[a.tipo]++;
                    s->valorDeposito += precoProduto(a.tipo);
                    a.produtoPronto = false;
                    char msg[96];
                    snprintf(msg, sizeof(msg), "Recolheu %s (+%d no deposito)",
                             nomeProduto(a.tipo), precoProduto(a.tipo));
                    adicionarLog(s, msg);
                    continue;
                }

                if (a.comFome && s->inventarioRacao > 0)
                {
                    s->inventarioRacao--;
                    a.comFome = false;
                    a.timestampUltimaRefeicao = s->tempoJogoMs;
                    char msg[96];
                    snprintf(msg, sizeof(msg), "Alimentou %s",
                             a.tipo == GALINHA ? "galinha" :
                             a.tipo == VACA    ? "vaca"    : "ovelha");
                    adicionarLog(s, msg);
                    continue;
                }

                if (a.comFome && s->inventarioRacao == 0)
                {
                    adicionarLog(s, "Sem racao! Compra na Loja");
                    continue;
                }
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
                            char msg[96];
                            snprintf(msg, sizeof(msg), "Canteiro desbloqueado (-%d ouro)", preco);
                            adicionarLog(s, msg);
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
                        if (c.estado == VAZIO && !c.seca && s->toolbar.sementeSelecionada >= 0)
                        {
                            int sem = s->toolbar.sementeSelecionada;

                            if (!atingiuNivel(s->xp, TABELA_CROPS[sem].nivelDesbloqueio))
                            {
                                char msg[96];
                                snprintf(msg, sizeof(msg), "%s desbloqueia no Nv %d",
                                         TABELA_CROPS[sem].nome, TABELA_CROPS[sem].nivelDesbloqueio);
                                adicionarLog(s, msg);
                                break;
                            }

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

                                char msg[96];
                                snprintf(msg, sizeof(msg), "Plantou %s", TABELA_CROPS[sem].nome);
                                adicionarLog(s, msg);
                                incrementarProgressoMissao(s->missoesDiarias, MISSAO_PLANTAR);
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

                            {
                                char msg[96];
                                snprintf(msg, sizeof(msg), "Colheu %s (+%d no deposito)",
                                         TABELA_CROPS[c.tipoCrop].nome, ganho);
                                adicionarLog(s, msg);
                                incrementarProgressoMissao(s->missoesDiarias, MISSAO_COLHER);
                            }

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
                            c.ultimoSorteioEventoMs = s->tempoJogoMs;
                            s->xp += 2;
                            adicionarLog(s, "Regou canteiro (+2 XP)");
                            incrementarProgressoMissao(s->missoesDiarias, MISSAO_REGAR);
                        }
                        break;
                    case REMOVEDOR:
                        if (c.praga == 1)
                        {
                            c.praga = 0;
                            c.ultimoSorteioEventoMs = s->tempoJogoMs;
                            s->xp += 2;
                            adicionarLog(s, "Removeu erva daninha (+2 XP)");
                            incrementarProgressoMissao(s->missoesDiarias, MISSAO_REMOVER_PRAGA);
                        }
                        break;
                    case PESTICIDA:
                        if (c.praga == 2)
                        {
                            c.praga = 0;
                            c.ultimoSorteioEventoMs = s->tempoJogoMs;
                            s->xp += 2;
                            adicionarLog(s, "Aplicou pesticida (+2 XP)");
                            incrementarProgressoMissao(s->missoesDiarias, MISSAO_REMOVER_PRAGA);
                        }
                        break;
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
            if (evento.key.keysym.sym == SDLK_F3)
            {
                s->debugAtivo = !s->debugAtivo;
                adicionarLog(s, s->debugAtivo ? "Debug ON (F3)" : "Debug OFF (F3)");
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

    s->hudDireitoHover = hudDireitoHitTest(s->mouseX, s->mouseY);
    if (s->hudDireitoHover >= 0)
        s->canteiroHover = -1;
}

static void atualizarCrescimento(GameState *s)
{
    for (int i = 0; i < GRID_COLUNAS * GRID_LINHAS; i++)
    {
        Canteiro &c = s->canteiros[i];

        if (c.estado == PLANTADO && c.tipoCrop >= 0)
        {
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
        }

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
        if (c.seca && assets.tileTerraSeca)
            desenharTile(renderer, assets.tileTerraSeca, telaX, telaY);
        else if (assets.tileTerra)
            desenharTile(renderer, assets.tileTerra, telaX, telaY);
        else
            desenharLosangoPreenchido(renderer, telaX, telaY, 139, 100, 60);
        break;
    case PLANTADO:
        if (assets.tileTerra)
            desenharTile(renderer, assets.tileTerra, telaX, telaY);
        else
            desenharLosangoPreenchido(renderer, telaX, telaY, 120, 80, 45);
        break;
    case MADURO:
        if (assets.tileTerra)
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

        if ((cHov.estado == PLANTADO || cHov.estado == MADURO) && cHov.tipoCrop >= 0)
        {
            desenharTooltipPlanta(renderer, s->fonteTooltip, cHov, s->tempoJogoMs,
                                  tx, ty - TILE_ALTURA / 2 - 4);
        }
        else
        {
            desenharTooltipModerno(renderer, s->fonteTooltip,
                                   legendaCanteiro(cHov),
                                   tx, ty - TILE_ALTURA / 2 - 4);
        }
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

    desenharCachorro(renderer, s->cachorro, s->animalAssets, s->tempoJogoMs);
    desenharAnimais(renderer, s->animais, s->animalAssets, s->tempoJogoMs);

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

    desenharHudEsquerdo(renderer, s->fonte, s->fontePequena, s->fonteHud, s->hudAssets,
                        s->xp, s->ouro, s->moedasVerdes, s->popularidade, s->generoJogador);
    desenharHudDireito(renderer, s->fontePequena, s->hudAssets, s->hudDireitoHover);
    desenharCaixaRecompensa(renderer, s->hudAssets, s->recompensaDisponivel, s->tempoJogoMs);
    desenharLogEventos(renderer, s->fontePequena, *s);

    desenharPainelSementes(renderer, s->fonte, s->fontePequena, s->cropAssets, s->toolbar, s->xp);
    desenharPainelMissoes(renderer, s->fonte, s->fontePequena, s->hudAssets,
                           s->missoesDiarias, s->painelMissoesAbertura);
    bool forcarCursor = s->lojaAberta || s->depositoAberto || s->modoCompraCanteiro || s->painelMissoesAberto;
    desenharCursorFerramenta(renderer, s->toolbar, s->mouseX, s->mouseY, sementeIcone, forcarCursor);

    if (s->debugAtivo)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 220);
        int top_x  = CERCADO_CENTRO_X,                    top_y  = CERCADO_CENTRO_Y - CERCADO_RAIO_Y;
        int rig_x  = CERCADO_CENTRO_X + CERCADO_RAIO_X,   rig_y  = CERCADO_CENTRO_Y;
        int bot_x  = CERCADO_CENTRO_X,                    bot_y  = CERCADO_CENTRO_Y + CERCADO_RAIO_Y;
        int lef_x  = CERCADO_CENTRO_X - CERCADO_RAIO_X,   lef_y  = CERCADO_CENTRO_Y;
        SDL_RenderDrawLine(renderer, top_x, top_y, rig_x, rig_y);
        SDL_RenderDrawLine(renderer, rig_x, rig_y, bot_x, bot_y);
        SDL_RenderDrawLine(renderer, bot_x, bot_y, lef_x, lef_y);
        SDL_RenderDrawLine(renderer, lef_x, lef_y, top_x, top_y);

        int marg = ANIMAL_TAMANHO / 2;
        SDL_SetRenderDrawColor(renderer, 0, 200, 200, 160);
        int rx = CERCADO_RAIO_X - marg, ry = CERCADO_RAIO_Y - marg;
        if (rx > 0 && ry > 0)
        {
            int t_x = CERCADO_CENTRO_X,       t_y = CERCADO_CENTRO_Y - ry;
            int r_x = CERCADO_CENTRO_X + rx,  r_y = CERCADO_CENTRO_Y;
            int b_x = CERCADO_CENTRO_X,       b_y = CERCADO_CENTRO_Y + ry;
            int l_x = CERCADO_CENTRO_X - rx,  l_y = CERCADO_CENTRO_Y;
            SDL_RenderDrawLine(renderer, t_x, t_y, r_x, r_y);
            SDL_RenderDrawLine(renderer, r_x, r_y, b_x, b_y);
            SDL_RenderDrawLine(renderer, b_x, b_y, l_x, l_y);
            SDL_RenderDrawLine(renderer, l_x, l_y, t_x, t_y);
        }

        SDL_SetRenderDrawColor(renderer, 255, 180, 0, 220);
        SDL_Rect cachorro = {CACHORRO_AREA_X, CACHORRO_AREA_Y, CACHORRO_AREA_LARGURA, CACHORRO_AREA_ALTURA};
        SDL_RenderDrawRect(renderer, &cachorro);

        char coords[64];
        snprintf(coords, sizeof(coords), "(%d, %d)", s->mouseX, s->mouseY);
        int padX = 8, padY = 4;
        int tw = 0, th = 0;
        if (s->fontePequena) TTF_SizeUTF8(s->fontePequena, coords, &tw, &th);
        int boxW = tw + padX * 2;
        int boxH = th + padY * 2;
        int boxX = s->mouseX + 14;
        int boxY = s->mouseY + 14;
        if (boxX + boxW > LARGURA_JANELA) boxX = s->mouseX - boxW - 6;
        SDL_Rect coordBox = {boxX, boxY, boxW, boxH};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_RenderFillRect(renderer, &coordBox);
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 240);
        SDL_RenderDrawRect(renderer, &coordBox);
        SDL_Color corCoord = {0, 255, 255, 255};
        desenharTexto(renderer, s->fontePequena, coords, boxX + padX, boxY + padY, corCoord, false);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    SDL_RenderPresent(renderer);
}

GAME_API void game_frame(GameState *s, SDL_Renderer *renderer, float dt)
{
    s->tempoJogoMs += static_cast<Uint32>(dt * s->velocidadeTempo * 1000.0f);

    if (s->tempoJogoMs - s->timestampUltimoResetMissoes >= RESET_DIARIO_MS)
    {
        s->timestampUltimoResetMissoes = s->tempoJogoMs;
        sortearMissoesDiarias(s->missoesDiarias);
        adicionarLog(s, "Novas missoes diarias!");
    }

    int diaAtual = static_cast<int>(s->tempoJogoMs / RESET_DIARIO_MS);
    if (diaAtual != s->ultimoDiaRecompensa)
        s->recompensaDisponivel = true;

    {
        float velocidade = 9.0f;
        struct AnimAlvo { float *valor; bool aberto; };
        AnimAlvo alvos[] = {
            {&s->painelMissoesAbertura, s->painelMissoesAberto},
            {&s->lojaAbertura,          s->lojaAberta},
            {&s->depositoAbertura,      s->depositoAberto},
        };
        for (auto &a : alvos)
        {
            float alvo = a.aberto ? 1.0f : 0.0f;
            *a.valor += (alvo - *a.valor) * velocidade * dt;
            if (*a.valor < 0.005f) *a.valor = 0.0f;
            if (*a.valor > 0.995f) *a.valor = 1.0f;
        }
    }

    for (int i = 0; i < 3; i++)
        atualizarAnimal(s->animais[i], dt, s->tempoJogoMs);
    atualizarCachorro(s->cachorro, dt, s->tempoJogoMs);

    processarEventos(s);
    atualizarAnimacoes(s->toolbar, dt);
    atualizarHover(s);
    atualizarCrescimento(s);
    renderizar(s, renderer);
}
