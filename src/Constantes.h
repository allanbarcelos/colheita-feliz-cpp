#pragma once

static constexpr int LARGURA_JANELA = 1600;
static constexpr int ALTURA_JANELA = 900;
static constexpr int FPS_ALVO = 60;
static constexpr int TEMPO_FRAME_MS = 1000 / FPS_ALVO;

static constexpr int TILE_LARGURA = 130;
static constexpr int TILE_ALTURA = 70;

static constexpr int GRID_COLUNAS = 6;
static constexpr int GRID_LINHAS = 5;

static constexpr int OFFSET_X = 470;
static constexpr int OFFSET_Y = 460;

static constexpr int CANTEIROS_INICIAIS = 6;

static constexpr int ICONE_TAMANHO = 64;
static constexpr int SLOT_TAMANHO = 76;
static constexpr int SLOT_ESPACAMENTO = 6;
static constexpr int TOTAL_FERRAMENTAS = 7;
static constexpr int TOOLBAR_LARGURA = TOTAL_FERRAMENTAS * SLOT_TAMANHO + (TOTAL_FERRAMENTAS - 1) * SLOT_ESPACAMENTO;
static constexpr int TOOLBAR_X = (LARGURA_JANELA - TOOLBAR_LARGURA) / 2;
static constexpr int TOOLBAR_ALTURA = SLOT_TAMANHO + 16;
static constexpr int TOOLBAR_Y = ALTURA_JANELA - TOOLBAR_ALTURA;
static constexpr int TOOLBAR_PADDING = 8;

static constexpr int TOTAL_CROPS = 15;
static constexpr int TOTAL_ESTAGIOS = 5;

static constexpr int SEMENTE_ICONE = 64;
static constexpr int SEMENTE_SLOT_LARGURA = 80;
static constexpr int SEMENTE_SLOT_ALTURA = 96;
static constexpr int SEMENTE_ESPACO = 8;
static constexpr int SEMENTE_LABEL_ALTURA = 18;

static constexpr int PAINEL_COLUNAS_SEMENTE = 5;
static constexpr int PAINEL_LINHAS_SEMENTE = 3;
static constexpr int PAINEL_PADDING = 16;
static constexpr int PAINEL_TITULO_ALTURA = 36;

static constexpr int PAINEL_LARGURA = PAINEL_COLUNAS_SEMENTE * SEMENTE_SLOT_LARGURA + (PAINEL_COLUNAS_SEMENTE - 1) * SEMENTE_ESPACO + 2 * PAINEL_PADDING;
static constexpr int PAINEL_ALTURA = PAINEL_TITULO_ALTURA + PAINEL_LINHAS_SEMENTE * SEMENTE_SLOT_ALTURA + (PAINEL_LINHAS_SEMENTE - 1) * SEMENTE_ESPACO + 2 * PAINEL_PADDING;

static constexpr int PAINEL_X = (LARGURA_JANELA - PAINEL_LARGURA) / 2;
static constexpr int PAINEL_Y = (ALTURA_JANELA - PAINEL_ALTURA) / 2;

static constexpr int VELOCIDADE_TEMPO_NORMAL = 1.0f;
static constexpr int VELOCIDADE_TEMPO_DEBUG = 60.0f;

static constexpr Uint32 INTERVALO_SORTEIO_EVENTO_MS = 15000;
static constexpr int CHANCE_EVENTO_PCT = 30;
static constexpr int PENALIDADE_EVENTO = 20;

static constexpr int HUD_MARGEM = 16;
static constexpr int HUD_ESPACO_Y = 4;

static constexpr int PRECO_CANTEIRO_BASE = 100;
static constexpr int PRECO_CANTEIRO_INCREMENTO = 30;

static constexpr int XP_BASE_POR_NIVEL = 100;
static constexpr int XP_INCREMENTO_POR_NIVEL = 50;
static constexpr int POPULARIDADE_MAX = 100;

