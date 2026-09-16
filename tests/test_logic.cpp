#define SDL_MAIN_HANDLED
#include "Player.h"
#include "Iso.h"
#include "Crops.h"

#include <cmath>
#include <cstdio>

static int falhas = 0;

#define CHECK(cond)                                                                            \
    do                                                                                         \
    {                                                                                          \
        if (!(cond))                                                                           \
        {                                                                                      \
            std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);                        \
            falhas++;                                                                          \
        }                                                                                      \
    } while (0)

int main()
{
    CHECK(nivelDoJogador(0) == 1);
    CHECK(nivelDoJogador(149) == 1);
    CHECK(nivelDoJogador(150) == 2);
    CHECK(atingiuNivel(0, 1));
    CHECK(!atingiuNivel(0, 2));
    CHECK(xpAtualNoNivel(150) == 0);

    int pop = 98;
    ganharPopularidade(pop, 5);
    CHECK(pop == POPULARIDADE_MAX);
    ganharPopularidade(pop, -250);
    CHECK(pop == 0);

    g_config.offsetX = OFFSET_X;
    g_config.offsetY = OFFSET_Y;
    int x = isoParaTelaX(2, 3);
    int y = isoParaTelaY(2, 3);
    CHECK(std::fabs(telaParaGridColuna(x, y) - 2.0f) < 0.001f);
    CHECK(std::fabs(telaParaGridLinha(x, y) - 3.0f) < 0.001f);

    g_config.offsetX = 500;
    g_config.offsetY = 400;
    int x2 = isoParaTelaX(0, 0);
    CHECK(x2 == 500);
    CHECK(isoParaTelaY(0, 0) == 400);

    CHECK(TOTAL_CROPS == 15);
    CHECK(xpPorColheita(NABO) == 7);
    CHECK(xpPorColheita(UVA) == 21);

    if (falhas)
    {
        std::printf("%d falha(s)\n", falhas);
        return 1;
    }
    std::printf("ok\n");
    return 0;
}
