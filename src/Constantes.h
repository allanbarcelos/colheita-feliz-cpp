#pragma once

static constexpr int LARGURA_JANELA = 1280;
static constexpr int ALTURA_JANELA = 720;
static constexpr int FPS_ALVO = 60;
static constexpr int TEMPO_FRAME_MS = 1000 / FPS_ALVO;

static constexpr int TILE_LARGURA = 120;
static constexpr int TILE_ALTURA = 60;

static constexpr int GRID_COLUNAS = 6;
static constexpr int GRID_LINHAS = 5;

static constexpr int OFFSET_X = 370;
static constexpr int OFFSET_Y = 360;

static constexpr int CANTEIROS_INICIAIS = 6;

static constexpr int ICONE_TAMANHO = 48;
static constexpr int SLOT_TAMANHO = 56;
static constexpr int SLOT_ESPACAMENTO = 4;
static constexpr int TOTAL_FERRAMENTAS = 7;
static constexpr int TOOLBAR_LARGURA = TOTAL_FERRAMENTAS * SLOT_TAMANHO + (TOTAL_FERRAMENTAS - 1) * SLOT_ESPACAMENTO;
static constexpr int TOOLBAR_X = (LARGURA_JANELA - TOOLBAR_LARGURA) / 2;
static constexpr int TOOLBAR_ALTURA = SLOT_TAMANHO + 16;
static constexpr int TOOLBAR_Y = ALTURA_JANELA - TOOLBAR_ALTURA;
static constexpr int TOOLBAR_PADDING = 8;
