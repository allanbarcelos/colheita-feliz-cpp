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
#include "PainelSettings.h"
#include "SaveGame.h"
#include "TelaTitulo.h"
#include "Player.h"

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>

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

static TTF_Font *abrirFonte(const char *primaria, const char *fallback, int tamanho)
{
    TTF_Font *f = TTF_OpenFont(resolverCaminho(primaria).c_str(), tamanho);
    if (f)
        return f;
    return TTF_OpenFont(resolverCaminho(fallback).c_str(), tamanho);
}

enum ModalAberto
{
    MODAL_NENHUM = 0,
    MODAL_SETTINGS,
    MODAL_MISSOES,
    MODAL_LOJA,
    MODAL_DEPOSITO,
    MODAL_SEMENTES
};

static ModalAberto modalAberto(const GameState *s)
{
    if (s->painelSettingsAberto) return MODAL_SETTINGS;
    if (s->painelMissoesAberto)  return MODAL_MISSOES;
    if (s->lojaAberta)           return MODAL_LOJA;
    if (s->depositoAberto)       return MODAL_DEPOSITO;
    if (s->toolbar.painelAberto) return MODAL_SEMENTES;
    return MODAL_NENHUM;
}

static void fecharModais(GameState *s)
{
    s->lojaAberta = false;
    s->depositoAberto = false;
    s->painelMissoesAberto = false;
    s->painelSettingsAberto = false;
    s->toolbar.painelAberto = false;
}

static void aplicarMouseLogico(GameState *s, SDL_Renderer *renderer, int wx, int wy)
{
    janelaParaLogico(renderer, wx, wy, &s->mouseX, &s->mouseY);
}

GAME_API void game_init(GameState *s, SDL_Renderer *renderer)
{
    carregarConfig(resolverCaminho("assets/config.ini").c_str());

    if (s->inicializado)
    {
        return;
    }

    srand(static_cast<unsigned>(time(nullptr)));

    s->fonte = abrirFonte("assets/fonts/Poppins-SemiBold.ttf", "assets/fonts/Nunito.ttf", 17);
    s->fontePequena = abrirFonte("assets/fonts/Poppins-Bold.ttf", "assets/fonts/Nunito.ttf", 14);
    s->fonteTooltip = abrirFonte("assets/fonts/Poppins-SemiBold.ttf", "assets/fonts/Nunito.ttf", 16);
    s->fonteHud = abrirFonte("assets/fonts/Poppins-Bold.ttf", "assets/fonts/Nunito.ttf", 20);
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
    s->moedasVerdes = 100;
    s->popularidade = 0;
    s->generoJogador = 0;

    s->tempoJogoMs = 0;
    s->velocidadeTempo = VELOCIDADE_TEMPO_NORMAL;
    s->colheitas = 0;
    s->xp = 0;
    s->ouro = 150;
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
    s->timestampUltimoResetMissoes = static_cast<Uint32>(diaCivilAtual());
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

    s->lojaTabAtiva = TAB_SEMENTES;
    s->decoracaoAssets = carregarDecoracaoAssets(renderer);
    for (int i = 0; i < TOTAL_DECORACOES; i++) s->inventarioDecoracoes[i] = 0;
    s->totalDecoracoesColocadas = 0;
    s->modoColocarItem = -1;
    s->rotacaoColocando = 0;
    s->decoracaoArrastando = -1;

    carregarSons(s->sons);
    iniciarMusicaFazenda(s->sons);
    s->painelSettingsAberto = false;
    s->painelSettingsAbertura = 0.0f;
    s->nivelAnteriorParaSom = nivelDoJogador(s->xp);

    s->estadoJogo = TELA_TITULO;
    s->saveExiste = saveExiste();
    s->botaoTituloHover = -1;
    s->rodapeIconeHover = -1;
    s->tipAtualIndex = 0;
    s->tipUltimaTrocaMs = SDL_GetTicks();
    s->passaroX1 = 0.0f;
    s->passaroY1 = 180.0f;
    s->passaroX2 = static_cast<float>(LARGURA_JANELA);
    s->passaroY2 = 280.0f;
    s->painelSettingsAbertoTitulo = false;

    s->tituloBackground   = carregarTextura(renderer, "assets/sprites/ui/tela_titulo_background.png");
    s->tituloLogo         = carregarTextura(renderer, "assets/sprites/ui/tela_titulo_logo.png");
    s->tituloGlow         = gerarGlowRadial(renderer, 512);
    s->tituloSparkles     = carregarTextura(renderer, "assets/sprites/ui/sparkles_sheet.png");
    s->tituloPassaros     = carregarTextura(renderer, "assets/sprites/ui/passaros_sheet.png");
    s->tituloIconeGithub  = carregarTexturaOpcional(renderer, "assets/sprites/ui/icone_github.png");
    s->tituloIconeLivepix = carregarTexturaOpcional(renderer, "assets/sprites/ui/icone_livepix.png");
    s->tituloIconeDiscord = carregarTexturaOpcional(renderer, "assets/sprites/ui/icone_discord.png");

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

static void acaoColher(GameState *s, Canteiro &c)
{
    s->colheitas++;
    int ganho = TABELA_CROPS[c.tipoCrop].precoVenda * c.saude / 100;
    s->valorDeposito += ganho;
    s->inventarioColhidos[c.tipoCrop]++;
    int xpGanho = xpPorColheita(c.tipoCrop);
    s->xp += xpGanho;
    ganharPopularidade(s->popularidade, 1);
    char msg[96];
    snprintf(msg, sizeof(msg), "Colheu %s (+%d deposito, +%d XP)", TABELA_CROPS[c.tipoCrop].nome, ganho, xpGanho);
    adicionarLog(s, msg);
    incrementarProgressoMissao(s->missoesDiarias, MISSAO_COLHER);
    tocarSfx(s->sons, s->sons.colher);

    c.temporadaAtual++;
    if (c.temporadaAtual <= TABELA_CROPS[c.tipoCrop].temporadas)
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

static void acaoRegar(GameState *s, Canteiro &c)
{
    c.seca = false;
    c.ultimoSorteioEventoMs = s->tempoJogoMs;
    s->xp += 2;
    adicionarLog(s, "Regou canteiro (+2 XP)");
    incrementarProgressoMissao(s->missoesDiarias, MISSAO_REGAR);
    tocarSfx(s->sons, s->sons.regar);
}

static void acaoRemoverErva(GameState *s, Canteiro &c)
{
    c.praga = 0;
    c.ultimoSorteioEventoMs = s->tempoJogoMs;
    s->xp += 2;
    adicionarLog(s, "Removeu erva daninha (+2 XP)");
    incrementarProgressoMissao(s->missoesDiarias, MISSAO_REMOVER_PRAGA);
    tocarSfx(s->sons, s->sons.removerPraga);
}

static void acaoPesticida(GameState *s, Canteiro &c)
{
    c.praga = 0;
    c.ultimoSorteioEventoMs = s->tempoJogoMs;
    s->xp += 2;
    adicionarLog(s, "Aplicou pesticida (+2 XP)");
    incrementarProgressoMissao(s->missoesDiarias, MISSAO_REMOVER_PRAGA);
    tocarSfx(s->sons, s->sons.pesticida);
}

static bool processarCliqueModal(GameState *s, int x, int y);
static bool processarCliqueMundo(GameState *s, int x, int y);

static void processarEventos(GameState *s, SDL_Renderer *renderer)
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
            aplicarMouseLogico(s, renderer, evento.motion.x, evento.motion.y);

            if (s->decoracaoArrastando >= 0 && s->decoracaoArrastando < s->totalDecoracoesColocadas)
            {
                s->decoracoesColocadas[s->decoracaoArrastando].x = s->mouseX;
                s->decoracoesColocadas[s->decoracaoArrastando].y = s->mouseY;
            }
        }

        if (evento.type == SDL_MOUSEBUTTONUP && evento.button.button == SDL_BUTTON_LEFT)
        {
            s->decoracaoArrastando = -1;
        }

        if (evento.type == SDL_MOUSEBUTTONDOWN && evento.button.button == SDL_BUTTON_RIGHT)
        {
            aplicarMouseLogico(s, renderer, evento.button.x, evento.button.y);
            int idx = decoracaoColocadaHitTest(s->decoracoesColocadas,
                                               s->totalDecoracoesColocadas,
                                               s->mouseX, s->mouseY);
            if (idx >= 0)
            {
                int item = s->decoracoesColocadas[idx].idItem;
                s->inventarioDecoracoes[item]++;
                s->decoracoesColocadas[idx] = s->decoracoesColocadas[--s->totalDecoracoesColocadas];
                char msg[96];
                snprintf(msg, sizeof(msg), "Removeu %s (volta pro inventario)",
                         TABELA_DECORACOES[item].nome);
                adicionarLog(s, msg);
            }
        }

        if (evento.type == SDL_MOUSEBUTTONDOWN && evento.button.button == SDL_BUTTON_LEFT)
        {
            aplicarMouseLogico(s, renderer, evento.button.x, evento.button.y);
            int x = s->mouseX;
            int y = s->mouseY;

            if (s->debugAtivo)
            {
                char dbg[64];
                snprintf(dbg, sizeof(dbg), "Click @ (%d, %d)", x, y);
                adicionarLog(s, dbg);
            }

            if (s->modoColocarItem >= 0 && s->totalDecoracoesColocadas < MAX_DECORACOES_COLOCADAS
                && modalAberto(s) == MODAL_NENHUM)
            {
                int idx = s->totalDecoracoesColocadas++;
                s->decoracoesColocadas[idx].idItem = s->modoColocarItem;
                s->decoracoesColocadas[idx].x = x;
                s->decoracoesColocadas[idx].y = y;
                s->decoracoesColocadas[idx].rotacao = s->rotacaoColocando;
                s->inventarioDecoracoes[s->modoColocarItem]--;
                ganharPopularidade(s->popularidade, 2);
                if (s->inventarioDecoracoes[s->modoColocarItem] <= 0)
                    s->modoColocarItem = primeiroItemNoInventario(s->inventarioDecoracoes);
                continue;
            }

            if (modalAberto(s) != MODAL_NENHUM)
            {
                processarCliqueModal(s, x, y);
                continue;
            }

            if (s->modoColocarItem < 0)
            {
                int idxDeco = decoracaoColocadaHitTest(s->decoracoesColocadas,
                                                       s->totalDecoracoesColocadas,
                                                       x, y);
                if (idxDeco >= 0)
                {
                    s->decoracaoArrastando = idxDeco;
                    continue;
                }
            }

            if (caixaRecompensaClicada(x, y, s->recompensaDisponivel))
            {
                char msg[96];
                sortearRecompensaDiaria(&s->ouro, &s->moedasVerdes,
                                         s->inventarioSementes, msg, sizeof(msg));
                adicionarLog(s, msg);
                tocarSfx(s->sons, s->sons.abrirCaixa);
                ganharPopularidade(s->popularidade, 1);
                s->recompensaDisponivel = false;
                s->ultimoDiaRecompensa = diaCivilAtual();
                continue;
            }

            if (hudAvatarHitTest(x, y))
            {
                s->generoJogador = s->generoJogador == 0 ? 1 : 0;
                adicionarLog(s, s->generoJogador == 1 ? "Avatar: feminino" : "Avatar: masculino");
                tocarSfx(s->sons, s->sons.clickBotao);
                continue;
            }

            int botaoHud = hudDireitoHitTest(x, y);
            if (botaoHud >= 0)
            {
                switch (botaoHud)
                {
                case 0:
                {
                    bool abrir = !s->depositoAberto;
                    fecharModais(s);
                    s->depositoAberto = abrir;
                    s->modoCompraCanteiro = false;
                    break;
                }
                case 1:
                {
                    bool abrir = !s->lojaAberta;
                    fecharModais(s);
                    s->lojaAberta = abrir;
                    s->modoCompraCanteiro = false;
                    break;
                }
                case 2:
                    adicionarLog(s, "Amigos em breve (fase 16)");
                    break;
                case 3:
                    adicionarLog(s, "Ranking em breve");
                    break;
                case 4:
                {
                    bool abrir = !s->painelMissoesAberto;
                    fecharModais(s);
                    s->painelMissoesAberto = abrir;
                    break;
                }
                case 5:
                {
                    bool abrir = !s->painelSettingsAberto;
                    fecharModais(s);
                    s->painelSettingsAberto = abrir;
                    break;
                }
                }
                tocarSfx(s->sons, s->sons.clickBotao);
                continue;
            }

            processarCliqueMundo(s, x, y);
        }

        if (evento.type == SDL_KEYDOWN)
        {
            if (evento.key.keysym.sym == SDLK_ESCAPE)
            {
                if (s->modoColocarItem >= 0)
                    s->modoColocarItem = -1;
                else if (modalAberto(s) != MODAL_NENHUM)
                    fecharModais(s);
                else
                    s->solicitouSair = true;
            }
            if (evento.key.keysym.sym == SDLK_p)
            {
                if (s->modoColocarItem >= 0)
                {
                    s->modoColocarItem = -1;
                    adicionarLog(s, "Modo colocar OFF");
                }
                else
                {
                    int item = primeiroItemNoInventario(s->inventarioDecoracoes);
                    if (item >= 0)
                    {
                        s->modoColocarItem = item;
                        char msg[96];
                        snprintf(msg, sizeof(msg), "Modo colocar ON (TAB cicla, click coloca, ESC sai)");
                        adicionarLog(s, msg);
                    }
                    else
                    {
                        adicionarLog(s, "Sem decoracoes — compra na Loja");
                    }
                }
            }
            if (evento.key.keysym.sym == SDLK_TAB && s->modoColocarItem >= 0)
            {
                s->modoColocarItem = proximoItemInventario(s->inventarioDecoracoes, s->modoColocarItem);
            }
            if (evento.key.keysym.sym == SDLK_r && s->modoColocarItem >= 0)
            {
                s->rotacaoColocando = s->rotacaoColocando ? 0 : 1;
            }
            if (evento.key.keysym.sym == SDLK_t)
            {
                s->velocidadeTempo = VELOCIDADE_TEMPO_DEBUG;
            }
            if (evento.key.keysym.sym == SDLK_F5)
            {
                carregarConfig(resolverCaminho("assets/config.ini").c_str());
                adicionarLog(s, "config.ini recarregado (F5)");
            }
            if (evento.key.keysym.sym == SDLK_F3)
            {
                s->debugAtivo = !s->debugAtivo;
                adicionarLog(s, s->debugAtivo ? "Debug ON (F3)" : "Debug OFF (F3)");
            }
            if (evento.key.keysym.sym == SDLK_s && (evento.key.keysym.mod & KMOD_CTRL))
            {
                if (salvarJogo(*s))
                {
                    adicionarLog(s, "Jogo salvo manualmente (Ctrl+S)");
                    s->saveExiste = true;
                }
            }

            if (evento.key.keysym.sym == SDLK_l)
            {
                bool abrir = !s->lojaAberta;
                fecharModais(s);
                s->lojaAberta = abrir;
            }

            if (evento.key.keysym.sym == SDLK_d)
            {
                bool abrir = !s->depositoAberto;
                fecharModais(s);
                s->depositoAberto = abrir;
            }

            if (evento.key.keysym.sym == SDLK_b)
            {
                s->modoCompraCanteiro = !s->modoCompraCanteiro;
                fecharModais(s);
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

static bool processarCliqueModal(GameState *s, int x, int y)
{
    switch (modalAberto(s))
    {
    case MODAL_SETTINGS:
    {
        int valor = 0;
        int hit = painelSettingsHitTest(x, y, valor);
        if (hit == -2 || hit == -1) s->painelSettingsAberto = false;
        else if (hit == 1) { s->sons.volumeMusica = valor; atualizarVolumeMusica(s->sons); }
        else if (hit == 2) { s->sons.volumeSfx = valor; }
        else if (hit == 3) { s->sons.mudo = !s->sons.mudo; atualizarVolumeMusica(s->sons); }
        return true;
    }
    case MODAL_MISSOES:
    {
        int hit = painelMissoesHitTest(x, y, s->missoesDiarias);
        if (hit == -2 || hit == -1)
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
                ganharPopularidade(s->popularidade, 3);
                m.coletada = true;
                char msg[96];
                snprintf(msg, sizeof(msg), "Missao coletada (+%d ouro, +%d verdes, +%d XP)",
                         m.recompensaOuro, m.recompensaVerdes, m.recompensaXp);
                adicionarLog(s, msg);
                tocarSfx(s->sons, s->sons.missaoCompleta);
            }
        }
        return true;
    }
    case MODAL_LOJA:
    {
        int hit = lojaHitTest(x, y, *s);
        if (hit == -2 || hit == -1)
        {
            s->lojaAberta = false;
        }
        else if (hit >= 100 && hit < 100 + TOTAL_TABS_LOJA)
        {
            int tab = hit - 100;
            bool habilitada = (tab == TAB_SEMENTES) || (tab == TAB_RACOES) || (tab == TAB_DECORACAO);
            if (habilitada)
                s->lojaTabAtiva = tab;
            else
                adicionarLog(s, "Em breve");
        }
        else if (hit >= 200 && hit < 200 + TOTAL_CROPS)
        {
            int slot = hit - 200;
            if (!atingiuNivel(s->xp, TABELA_CROPS[slot].nivelDesbloqueio))
            {
                char msg[96];
                snprintf(msg, sizeof(msg), "%s desbloqueia no Nv %d",
                         TABELA_CROPS[slot].nome, TABELA_CROPS[slot].nivelDesbloqueio);
                adicionarLog(s, msg);
            }
            else
            {
                int preco = TABELA_CROPS[slot].precoCompra;
                if (s->ouro >= preco)
                {
                    s->ouro -= preco;
                    s->inventarioSementes[slot]++;
                    char msg[96];
                    snprintf(msg, sizeof(msg), "Comprou 1 %s (-%d ouro)",
                             TABELA_CROPS[slot].nome, preco);
                    adicionarLog(s, msg);
                    tocarSfx(s->sons, s->sons.comprar);
                }
            }
        }
        else if (hit >= 300 && hit < 300 + TOTAL_DECORACOES)
        {
            int slot = hit - 300;
            const ItemDecoracao &d = TABELA_DECORACOES[slot];
            int nivelAtual = nivelDoJogador(s->xp);
            if (nivelAtual < d.requerLv)
            {
                char msg[96];
                snprintf(msg, sizeof(msg), "%s desbloqueia no Nv %d", d.nome, d.requerLv);
                adicionarLog(s, msg);
            }
            else if (s->ouro < d.precoOuro || s->moedasVerdes < d.precoVerdes)
            {
                adicionarLog(s, "Sem ouro ou moedas verdes suficientes");
            }
            else
            {
                s->ouro -= d.precoOuro;
                s->moedasVerdes -= d.precoVerdes;
                s->inventarioDecoracoes[slot]++;
                char msg[96];
                snprintf(msg, sizeof(msg), "Comprou %s (no inventario)", d.nome);
                adicionarLog(s, msg);
                tocarSfx(s->sons, s->sons.comprar);
            }
        }
        else if (hit == 999)
        {
            if (s->ouro >= PRECO_RACAO)
            {
                s->ouro -= PRECO_RACAO;
                s->inventarioRacao++;
                char msg[96];
                snprintf(msg, sizeof(msg), "Comprou 1 Racao (-%d ouro)", PRECO_RACAO);
                adicionarLog(s, msg);
                tocarSfx(s->sons, s->sons.comprar);
            }
        }
        return true;
    }
    case MODAL_DEPOSITO:
    {
        int resDep = depositoHitTest(x, y);
        if (resDep == -2 || resDep == -1)
        {
            s->depositoAberto = false;
        }
        else if (resDep == 100 && s->valorDeposito > 0)
        {
            char msg[96];
            snprintf(msg, sizeof(msg), "Vendeu deposito (+%d ouro)", s->valorDeposito);
            adicionarLog(s, msg);
            tocarSfx(s->sons, s->sons.vender);
            s->ouro += s->valorDeposito;
            s->valorDeposito = 0;
            for (int i = 0; i < TOTAL_CROPS; i++)
                s->inventarioColhidos[i] = 0;
        }
        return true;
    }
    case MODAL_SEMENTES:
    {
        int resultado = painelSementeHitTest(x, y);
        if (resultado >= 0)
            s->toolbar.sementeSelecionada = resultado;
        s->toolbar.painelAberto = false;
        return true;
    }
    default:
        return false;
    }
}

static bool processarCliqueMundo(GameState *s, int x, int y)
{
    int animalClicado = animalHitTest(s->animais, x, y);
    if (animalClicado >= 0)
    {
        Animal &a = s->animais[animalClicado];

        switch (a.tipo)
        {
        case GALINHA: tocarSfx(s->sons, s->sons.galinhaCluck); break;
        case VACA:    tocarSfx(s->sons, s->sons.vacaMoo);     break;
        case OVELHA:  tocarSfx(s->sons, s->sons.ovelhaBaa);   break;
        }

        if (a.produtoPronto)
        {
            s->inventarioProdutos[a.tipo]++;
            s->valorDeposito += precoProduto(a.tipo);
            a.produtoPronto = false;
            ganharPopularidade(s->popularidade, 1);
            char msg[96];
            snprintf(msg, sizeof(msg), "Recolheu %s (+%d no deposito)",
                     nomeProduto(a.tipo), precoProduto(a.tipo));
            adicionarLog(s, msg);
            return true;
        }

        if (a.comFome && s->inventarioRacao > 0)
        {
            s->inventarioRacao--;
            a.comFome = false;
            a.timestampUltimaRefeicao = s->tempoJogoMs;
            ganharPopularidade(s->popularidade, 1);
            char msg[96];
            snprintf(msg, sizeof(msg), "Alimentou %s",
                     a.tipo == GALINHA ? "galinha" :
                     a.tipo == VACA    ? "vaca"    : "ovelha");
            adicionarLog(s, msg);
            return true;
        }

        if (a.comFome && s->inventarioRacao == 0)
        {
            adicionarLog(s, "Sem racao! Compra na Loja");
            return true;
        }
    }

    if (cachorroHitTest(s->cachorro, x, y))
    {
        tocarSfx(s->sons, s->sons.cachorroBark);
        return true;
    }

    int slotClicado = toolbarHitTest(x, y);
    if (slotClicado >= 0)
    {
        Ferramenta nova = static_cast<Ferramenta>(slotClicado);
        if (nova == SACOLA)
        {
            bool abrir = !s->toolbar.painelAberto;
            fecharModais(s);
            s->toolbar.painelAberto = abrir;
            s->toolbar.selecionada = SACOLA;
        }
        else
        {
            s->toolbar.selecionada = nova;
            s->toolbar.painelAberto = false;
        }
        return true;
    }

    if (s->canteiroHover < 0)
        return false;

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
        return true;
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
                tocarSfx(s->sons, s->sons.plantar);
            }
        }
        break;
    case MAO:
        if (c.estado == MADURO)
            acaoColher(s, c);
        break;
    case REGADOR:
        if (c.seca)
            acaoRegar(s, c);
        break;
    case REMOVEDOR:
        if (c.praga == 1)
            acaoRemoverErva(s, c);
        break;
    case PESTICIDA:
        if (c.praga == 2)
            acaoPesticida(s, c);
        break;
    case CURSOR:
        if (c.estado == MADURO)
            acaoColher(s, c);
        else if (c.estado == RESTOS)
        {
            c.estado = VAZIO;
            c.tipoCrop = -1;
            c.estagioCrop = 0;
        }
        else if (c.seca)
            acaoRegar(s, c);
        else if (c.praga == 1)
            acaoRemoverErva(s, c);
        else if (c.praga == 2)
            acaoPesticida(s, c);
        break;
    }
    return true;
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
        if (c.saude < 50 && assets.tileTerraVermelha)
            desenharTile(renderer, assets.tileTerraVermelha, telaX, telaY);
        else if (assets.tileTerra)
            desenharTile(renderer, assets.tileTerra, telaX, telaY);
        else
            desenharLosangoPreenchido(renderer, telaX, telaY, 120, 80, 45);
        break;
    case MADURO:
        if (c.saude < 50 && assets.tileTerraVermelha)
            desenharTile(renderer, assets.tileTerraVermelha, telaX, telaY);
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

    desenharCercaCercado(renderer, s->assets.cerca);

    SDL_Texture *sementeIcone = nullptr;
    if (s->toolbar.sementeSelecionada >= 0 && s->toolbar.sementeSelecionada < TOTAL_CROPS)
    {
        sementeIcone = s->cropAssets.sementes[s->toolbar.sementeSelecionada];
    }

    desenharDecoracoesColocadas(renderer, s->decoracaoAssets,
                                 s->decoracoesColocadas, s->totalDecoracoesColocadas,
                                 s->tempoJogoMs);

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
    desenharHudEsquerdo(renderer, s->fonte, s->fontePequena, s->fonteHud, s->hudAssets,
                        s->xp, s->ouro, s->moedasVerdes, s->popularidade, s->generoJogador);
    desenharHudDireito(renderer, s->fontePequena, s->hudAssets, s->hudDireitoHover);
    desenharCaixaRecompensa(renderer, s->hudAssets, s->recompensaDisponivel, s->tempoJogoMs);
    desenharLogEventos(renderer, s->fontePequena, *s);

    desenharPainelSementes(renderer, s->fonte, s->fontePequena, s->cropAssets, s->toolbar, s->xp);
    desenharDeposito(renderer, s->fonte, s->fontePequena, s->cropAssets, s->hudAssets, *s);
    desenharLoja(renderer, s->fonte, s->fontePequena, s->cropAssets, s->hudAssets, *s);
    desenharPainelMissoes(renderer, s->fonte, s->fontePequena, s->hudAssets,
                           s->missoesDiarias, s->painelMissoesAbertura);
    desenharPainelSettings(renderer, s->fonte, s->fontePequena, s->sons, s->painelSettingsAbertura);
    bool forcarCursor = s->lojaAberta || s->depositoAberto || s->modoCompraCanteiro || s->painelMissoesAberto;
    int tipoCursor = CURSOR_NORMAL;
    bool sobreInterativo = (s->hudDireitoHover >= 0) || (s->toolbarHover >= 0) ||
                           caixaRecompensaClicada(s->mouseX, s->mouseY, s->recompensaDisponivel) ||
                           (s->lojaAberta && lojaHitTest(s->mouseX, s->mouseY, *s) != -1) ||
                           (s->depositoAberto && depositoHitTest(s->mouseX, s->mouseY) != -1) ||
                           (s->painelMissoesAberto && painelMissoesHitTest(s->mouseX, s->mouseY, s->missoesDiarias) != -1);
    bool sobreColher = false;
    SDL_Texture *iconeContextual = nullptr;
    if (s->canteiroHover >= 0 && s->toolbar.selecionada == CURSOR)
    {
        const Canteiro &c = s->canteiros[s->canteiroHover];
        if (c.estado == MADURO)        { sobreColher = true; iconeContextual = s->toolbar.icones[MAO]; }
        else if (c.estado == RESTOS)   { sobreColher = true; iconeContextual = s->toolbar.icones[ENXADA]; }
        else if (c.seca)               { sobreColher = true; iconeContextual = s->toolbar.icones[REGADOR]; }
        else if (c.praga == 1)         { sobreColher = true; iconeContextual = s->toolbar.icones[REMOVEDOR]; }
        else if (c.praga == 2)         { sobreColher = true; iconeContextual = s->toolbar.icones[PESTICIDA]; }
    }
    if (animalHitTest(s->animais, s->mouseX, s->mouseY) >= 0) sobreColher = true;
    if (decoracaoColocadaHitTest(s->decoracoesColocadas, s->totalDecoracoesColocadas, s->mouseX, s->mouseY) >= 0)
        sobreColher = true;

    if (iconeContextual)
    {
        SDL_Rect destino = {s->mouseX - ICONE_TAMANHO / 2, s->mouseY - ICONE_TAMANHO / 2,
                            ICONE_TAMANHO, ICONE_TAMANHO};
        SDL_RenderCopy(renderer, iconeContextual, nullptr, &destino);
        SDL_ShowCursor(SDL_DISABLE);
    }
    else
    {
        if (sobreColher) tipoCursor = CURSOR_PEGANDO;
        else if (sobreInterativo) tipoCursor = CURSOR_APONTANDO;
        desenharCursorFerramenta(renderer, s->toolbar, s->mouseX, s->mouseY, sementeIcone, forcarCursor, tipoCursor);
    }

    if (s->modoColocarItem >= 0)
        desenharFantasmaDecoracao(renderer, s->decoracaoAssets, s->modoColocarItem,
                                   s->mouseX, s->mouseY, s->tempoJogoMs, s->rotacaoColocando);

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
    if (s->estadoJogo == TELA_TITULO)
    {
        atualizarTelaTitulo(*s, dt);

        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT) s->solicitouSair = true;
            if (e.type == SDL_MOUSEMOTION)
            {
                aplicarMouseLogico(s, renderer, e.motion.x, e.motion.y);
                int hit = telaTituloHitTest(s->mouseX, s->mouseY, s->saveExiste);
                s->botaoTituloHover = (hit >= 100 && hit <= 102) ? hit - 100
                                      : (hit >= 300 && hit <= 301) ? 10 + (hit - 300)
                                      : -1;
                s->rodapeIconeHover = (hit >= 200 && hit <= 202) ? hit - 200 : -1;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
            {
                aplicarMouseLogico(s, renderer, e.button.x, e.button.y);
                if (s->painelSettingsAbertoTitulo)
                {
                    int valor = 0;
                    int hit = painelSettingsHitTest(s->mouseX, s->mouseY, valor);
                    if (hit == -2) s->painelSettingsAbertoTitulo = false;
                    else if (hit == 1) { s->sons.volumeMusica = valor; atualizarVolumeMusica(s->sons); }
                    else if (hit == 2) { s->sons.volumeSfx = valor; }
                    else if (hit == 3) { s->sons.mudo = !s->sons.mudo; atualizarVolumeMusica(s->sons); }
                    else if (hit == -1) s->painelSettingsAbertoTitulo = false;
                    continue;
                }

                int hit = telaTituloHitTest(s->mouseX, s->mouseY, s->saveExiste);
                if (hit > 0) tocarSfx(s->sons, s->sons.clickBotao);

                switch (hit)
                {
                case 100:
                    s->estadoJogo = JOGANDO;
                    s->nivelAnteriorParaSom = nivelDoJogador(s->xp);
                    break;
                case 101:
                    if (carregarJogo(*s))
                    {
                        s->estadoJogo = JOGANDO;
                        s->nivelAnteriorParaSom = nivelDoJogador(s->xp);
                    }
                    break;
                case 102:
                    s->painelSettingsAbertoTitulo = true;
                    break;
                case 200:
                    abrirUrl(URL_GITHUB);
                    break;
                case 201:
                    abrirUrl(URL_SITE);
                    break;
                case 202:
                    abrirUrl(URL_DISCORD);
                    break;
                case 300:
                    s->generoJogador = 0;
                    break;
                case 301:
                    s->generoJogador = 1;
                    break;
                }
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
            {
                if (s->painelSettingsAbertoTitulo) s->painelSettingsAbertoTitulo = false;
                else s->solicitouSair = true;
            }
        }

        desenharTelaTitulo(renderer, s->fonte, s->fonteHud, s->fontePequena, *s);
        if (s->painelSettingsAbertoTitulo)
            desenharPainelSettings(renderer, s->fonte, s->fontePequena, s->sons, 1.0f);

        bool sobreBtn = s->botaoTituloHover >= 0 || s->rodapeIconeHover >= 0;
        int tipoCursor = sobreBtn ? CURSOR_APONTANDO : CURSOR_NORMAL;
        desenharCursorFerramenta(renderer, s->toolbar, s->mouseX, s->mouseY, nullptr, true, tipoCursor);

        SDL_RenderPresent(renderer);
        return;
    }

    s->tempoJogoMs += static_cast<Uint32>(dt * s->velocidadeTempo * 1000.0f);

    int hoje = diaCivilAtual();
    if (static_cast<int>(s->timestampUltimoResetMissoes) != hoje)
    {
        s->timestampUltimoResetMissoes = static_cast<Uint32>(hoje);
        sortearMissoesDiarias(s->missoesDiarias);
        adicionarLog(s, "Novas missoes diarias!");
    }

    if (hoje != s->ultimoDiaRecompensa)
        s->recompensaDisponivel = true;

    {
        float velocidade = 9.0f;
        struct AnimAlvo { float *valor; bool aberto; };
        AnimAlvo alvos[] = {
            {&s->painelMissoesAbertura, s->painelMissoesAberto},
            {&s->lojaAbertura,          s->lojaAberta},
            {&s->depositoAbertura,      s->depositoAberto},
            {&s->painelSettingsAbertura, s->painelSettingsAberto},
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

    {
        int nivelAtual = nivelDoJogador(s->xp);
        if (nivelAtual > s->nivelAnteriorParaSom)
        {
            tocarSfx(s->sons, s->sons.levelUp);
            s->nivelAnteriorParaSom = nivelAtual;
        }
    }

    processarEventos(s, renderer);
    atualizarAnimacoes(s->toolbar, dt);
    atualizarHover(s);
    atualizarCrescimento(s);
    renderizar(s, renderer);
}
