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

static constexpr int CERCADO_CENTRO_X = 1331;
static constexpr int CERCADO_CENTRO_Y = 513;
static constexpr int CERCADO_RAIO_X = 205;
static constexpr int CERCADO_RAIO_Y = 114;
static constexpr int ANIMAL_TAMANHO = 128;
static constexpr int ANIMAL_SENTADO_TAM = 96;

static constexpr int CACHORRO_AREA_X = 820;
static constexpr int CACHORRO_AREA_Y = 320;
static constexpr int CACHORRO_AREA_LARGURA = 220;
static constexpr int CACHORRO_AREA_ALTURA = 140;
static constexpr Uint32 CACHORRO_TEMPO_PARADO_MS = 3000;

static constexpr Uint32 INTERVALO_PRODUCAO_MS = 8u * 3600u * 1000u;
static constexpr int RACAO_INICIAL = 3;
static constexpr int PRECO_RACAO = 30;

static constexpr int ANIMAL_FRAME_TEMPO_MS = 165;
static constexpr float ANIMAL_VELOCIDADE_PX_S = 18.0f;

static constexpr int CLICK_TOLERANCIA = 6;

static constexpr int LOJA_LARGURA = 1100;
static constexpr int LOJA_ALTURA = 720;
static constexpr int LOJA_X = (LARGURA_JANELA - LOJA_LARGURA) / 2;
static constexpr int LOJA_Y = (ALTURA_JANELA - LOJA_ALTURA) / 2;
static constexpr int TOTAL_TABS_LOJA = 7;

