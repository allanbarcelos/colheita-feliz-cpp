#pragma once

#include <SDL2/SDL_mixer.h>
#include <iostream>

struct Sons
{
    Mix_Chunk *plantar = nullptr;
    Mix_Chunk *colher = nullptr;
    Mix_Chunk *regar = nullptr;
    Mix_Chunk *removerPraga = nullptr;
    Mix_Chunk *pesticida = nullptr;
    Mix_Chunk *comprar = nullptr;
    Mix_Chunk *vender = nullptr;
    Mix_Chunk *clickPainel = nullptr;
    Mix_Chunk *clickBotao = nullptr;
    Mix_Chunk *levelUp = nullptr;
    Mix_Chunk *missaoCompleta = nullptr;
    Mix_Chunk *abrirCaixa = nullptr;

    Mix_Chunk *galinhaCluck = nullptr;
    Mix_Chunk *vacaMoo = nullptr;
    Mix_Chunk *ovelhaBaa = nullptr;
    Mix_Chunk *cachorroBark = nullptr;

    Mix_Music *musicaFazenda = nullptr;

    int volumeMusica = 64;
    int volumeSfx = 96;
    bool mudo = false;
};

inline Mix_Chunk *carregarSfx(const char *caminho)
{
    Mix_Chunk *c = Mix_LoadWAV(caminho);
    if (!c) std::cout << "[audio] falha SFX " << caminho << ": " << Mix_GetError() << std::endl;
    return c;
}

inline void tocarSfx(const Sons &s, Mix_Chunk *chunk)
{
    if (s.mudo || !chunk) return;
    Mix_VolumeChunk(chunk, s.volumeSfx);
    Mix_PlayChannel(-1, chunk, 0);
}

inline void carregarSons(Sons &s)
{
    s.plantar         = carregarSfx("assets/sounds/plant.wav");
    s.colher          = carregarSfx("assets/sounds/harvest.wav");
    s.regar           = carregarSfx("assets/sounds/water.wav");
    s.removerPraga    = carregarSfx("assets/sounds/weed.wav");
    s.pesticida       = carregarSfx("assets/sounds/pesticide.wav");
    s.comprar         = carregarSfx("assets/sounds/buy.wav");
    s.vender          = carregarSfx("assets/sounds/sell.wav");
    s.clickPainel     = carregarSfx("assets/sounds/click_panel.wav");
    s.clickBotao      = carregarSfx("assets/sounds/click_button.wav");
    s.levelUp         = carregarSfx("assets/sounds/levelup.wav");
    s.missaoCompleta  = carregarSfx("assets/sounds/mission_complete.wav");
    s.abrirCaixa      = carregarSfx("assets/sounds/gift_open.wav");

    s.galinhaCluck    = carregarSfx("assets/sounds/chicken.wav");
    s.vacaMoo         = carregarSfx("assets/sounds/cow.wav");
    s.ovelhaBaa       = carregarSfx("assets/sounds/sheep.wav");
    s.cachorroBark    = carregarSfx("assets/sounds/dog_bark.wav");

    s.musicaFazenda = Mix_LoadMUS("assets/sounds/farm_loop.mp3");
    if (!s.musicaFazenda) std::cout << "[audio] musica falhou: " << Mix_GetError() << std::endl;
}

inline void liberarSons(Sons &s)
{
    if (s.plantar)        Mix_FreeChunk(s.plantar);
    if (s.colher)         Mix_FreeChunk(s.colher);
    if (s.regar)          Mix_FreeChunk(s.regar);
    if (s.removerPraga)   Mix_FreeChunk(s.removerPraga);
    if (s.pesticida)      Mix_FreeChunk(s.pesticida);
    if (s.comprar)        Mix_FreeChunk(s.comprar);
    if (s.vender)         Mix_FreeChunk(s.vender);
    if (s.clickPainel)    Mix_FreeChunk(s.clickPainel);
    if (s.clickBotao)     Mix_FreeChunk(s.clickBotao);
    if (s.levelUp)        Mix_FreeChunk(s.levelUp);
    if (s.missaoCompleta) Mix_FreeChunk(s.missaoCompleta);
    if (s.abrirCaixa)     Mix_FreeChunk(s.abrirCaixa);
    if (s.galinhaCluck)   Mix_FreeChunk(s.galinhaCluck);
    if (s.vacaMoo)        Mix_FreeChunk(s.vacaMoo);
    if (s.ovelhaBaa)      Mix_FreeChunk(s.ovelhaBaa);
    if (s.cachorroBark)   Mix_FreeChunk(s.cachorroBark);
    if (s.musicaFazenda)  Mix_FreeMusic(s.musicaFazenda);
}

inline void iniciarMusicaFazenda(Sons &s)
{
    if (!s.musicaFazenda || s.mudo) return;
    Mix_VolumeMusic(s.volumeMusica);
    Mix_PlayMusic(s.musicaFazenda, -1);
}

inline void atualizarVolumeMusica(Sons &s)
{
    Mix_VolumeMusic(s.mudo ? 0 : s.volumeMusica);
}
