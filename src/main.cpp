#include "Constantes.h"
#include "Tipos.h"
#include "Iso.h"
#include "Desenho.h"
#include "Assets.h"
#include "Toolbar.h"

#include <array>

int main(int agrc, char *agrv[])
{
    (void)agrc;
    (void)agrv;

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    SDL_Window *janela = SDL_CreateWindow(
        "Colheita Feliz",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        LARGURA_JANELA,
        ALTURA_JANELA,
        0);

    SDL_Renderer *renderer = SDL_CreateRenderer(
        janela,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        std::cout << "Erro ao criar a render: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(janela);
        IMG_Quit();
        SDL_Quit();
    }

    Toolbar toolbar;
    carregarIconesToolbar(renderer, toolbar);

    Assets assets = carregarTodosAssets(renderer);

    std::array<Canteiro, GRID_COLUNAS * GRID_LINHAS> canteiros;

    for (int linha = 0; linha < GRID_LINHAS; linha++)
    {
        for (int coluna = 0; coluna < GRID_COLUNAS; coluna++)
        {
            int indice = linha * GRID_COLUNAS + coluna;
            canteiros[indice].coluna = coluna;
            canteiros[indice].linha = linha;
            canteiros[indice].estado = BLOQUEADO;
        }
    }

    int desbloqueados = 0;
    for (int linha = 0; linha < GRID_LINHAS && desbloqueados < CANTEIROS_INICIAIS; linha++)
    {
        for (int coluna = 0; coluna < GRID_COLUNAS && desbloqueados < CANTEIROS_INICIAIS; coluna += 1)
        {
            if (coluna < 2)
            {
                int indice = linha * GRID_COLUNAS + coluna;
                canteiros[indice].estado = VAZIO;
                desbloqueados++;
            }
        }
    }

    std::cout << "Fazenda: " << GRID_COLUNAS << "x" << GRID_LINHAS << " (" << CANTEIROS_INICIAIS << "desbloqueados)" << std::endl;

    bool rodando = true;

    SDL_Event evento;

    Uint32 tempo_anterior = SDL_GetTicks();

    float deltaTime = 0.0f;

    int canteiroHover = -1;
    int mouseX = 0;
    int mouseY = 0;

    int colheitas = 0;

    while (rodando)
    {
        Uint32 tempo_atual = SDL_GetTicks();

        deltaTime = (tempo_atual - tempo_anterior) / 1000.0f;

        tempo_anterior = tempo_atual;

        while (SDL_PollEvent(&evento))
        {

            if (evento.type == SDL_QUIT)
            {
                rodando = false;
            }

            if (evento.type == SDL_MOUSEMOTION)
            {
                mouseX = evento.motion.x;
                mouseY = evento.motion.y;
            }

            if (evento.type == SDL_MOUSEBUTTONDOWN && evento.button.button == SDL_BUTTON_LEFT)
            {

                int slotClicado = toolbarHitTest(evento.button.x, evento.button.y);

                if (slotClicado >= 0)
                {
                    toolbar.selecionada = static_cast<Ferramenta>(slotClicado);
                }
                else if (canteiroHover >= 0)
                {
                    Canteiro &c = canteiros[canteiroHover];
                    std::cout << "Clique (" << c.coluna << "," << c.linha << "): ";
                    switch (toolbar.selecionada)
                    {
                    case ENXADA:
                        if (c.estado == RESTOS)
                        {
                            c.estado = VAZIO;
                        }
                        break;
                    case SACOLA:
                        if (c.estado == VAZIO)
                        {
                            c.estado = PLANTADO;
                        }
                        break;
                    case MAO:
                        if (c.estado == MADURO)
                        {
                            c.estado = RESTOS;
                            colheitas++;
                        }
                        break;
                    case REGADOR:
                    case REMOVEDOR:
                    case PESTICIDA:
                        break;
                    case CURSOR:
                        break;
                    }
                }
            }

            if (evento.type == SDL_KEYDOWN)
            {

                if (evento.key.keysym.sym == SDLK_ESCAPE)
                {
                    rodando = false;
                }
            }
        }

        {
            float colF = telaParaGridColuna(mouseX, mouseY);
            float linF = telaParaGridLinha(mouseX, mouseY);

            int col = static_cast<int>(floor(colF));
            int lin = static_cast<int>(floor(linF));

            if (col >= 0 && col < GRID_COLUNAS && lin >= 0 && lin < GRID_LINHAS)
            {
                canteiroHover = lin * GRID_COLUNAS + col;
            }
            else
            {
                canteiroHover = -1;
            }
        }

        SDL_SetRenderDrawColor(renderer, 56, 142, 24, 255);
        SDL_RenderClear(renderer);
        if (assets.background)
        {
            SDL_Rect destBg = {0, -50, LARGURA_JANELA, ALTURA_JANELA + 50};
            SDL_RenderCopy(renderer, assets.background, nullptr, &destBg);
        }

        for (int linha = 0; linha < GRID_LINHAS; linha++)
        {
            for (int coluna = 0; coluna < GRID_COLUNAS; coluna++)
            {
                int telaX = isoParaTelaX(coluna, linha);
                int telaY = isoParaTelaY(coluna, linha);
                int indice = linha * GRID_COLUNAS + coluna;
                Canteiro &c = canteiros[indice];

                switch (c.estado)
                {
                case BLOQUEADO:
                    if (assets.tileGramaEscuro)
                    {
                        desenharTile(renderer, assets.tileGramaEscuro, telaX, telaY);
                    }
                    else if (assets.tileBloqueado)
                    {
                        desenharTile(renderer, assets.tileBloqueado, telaX, telaY);
                    }
                    else
                    {
                        desenharLosangoPreenchido(renderer, telaX, telaY, 80, 120, 50);
                    }
                    break;
                case VAZIO:
                    if (assets.tileTerra)
                    {
                        desenharTile(renderer, assets.tileTerra, telaX, telaY);
                    }
                    else
                    {
                        desenharLosangoPreenchido(renderer, telaX, telaY, 139, 100, 60);
                    }
                    break;
                case PLANTADO:
                    if (assets.tileTerra)
                    {
                        desenharTile(renderer, assets.tileTerra, telaX, telaY);
                    }
                    else
                    {
                        desenharLosangoPreenchido(renderer, telaX, telaY, 120, 80, 45);
                    }

                    SDL_SetRenderDrawColor(renderer, 50, 200, 50, 255);
                    SDL_RenderDrawLine(renderer, telaX, telaY - 8, telaX, telaY + 2);
                    SDL_RenderDrawLine(renderer, telaX - 3, telaY - 5, telaX, telaY - 8);
                    SDL_RenderDrawLine(renderer, telaX + 3, telaY - 5, telaX, telaY - 8);
                    break;
                case MADURO:
                {
                    if (assets.tileTerra)
                    {
                        desenharTile(renderer, assets.tileTerra, telaX, telaY);
                    }
                    else
                    {
                        desenharLosangoPreenchido(renderer, telaX, telaY, 120, 80, 45);
                    }

                    SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
                    SDL_Rect fruto = {telaX - 4, telaY - 10, 8, 8};
                    SDL_RenderFillRect(renderer, &fruto);
                    break;
                }
                case RESTOS:
                    if (assets.tileRestos)
                    {
                        desenharTile(renderer, assets.tileRestos, telaX, telaY);
                    }
                    else
                    {
                        desenharLosangoPreenchido(renderer, telaX, telaY, 120, 80, 45);
                        SDL_SetRenderDrawColor(renderer, 80, 70, 50, 255);
                        SDL_RenderDrawLine(renderer, telaX - 6, telaY, telaX + 6, telaY);
                    }

                    break;
                }

                if (indice == canteiroHover)
                {
                    desenharLosangoContorno(renderer, telaX, telaY, 255, 255, 0);
                    desenharLosangoContorno(renderer, telaX, telaY, 255, 255, 100, TILE_LARGURA - 4, TILE_ALTURA - 2);
                }
            }
        }

        if (assets.casa)
        {
            SDL_Rect destCasa = {580, 0, 200, 200};
            SDL_RenderCopyEx(renderer, assets.casa, nullptr, &destCasa,
                             0.0, nullptr, SDL_FLIP_HORIZONTAL);
        }

        if (assets.casaCachorro)
        {
            SDL_Rect destCasinha = {760, 150, 80, 80};
            SDL_RenderCopyEx(renderer, assets.casaCachorro, nullptr, &destCasinha,
                             0.0, nullptr, SDL_FLIP_HORIZONTAL);
        }

        desenharToolbar(renderer, toolbar);

        desenharCursorFerramenta(renderer, toolbar, mouseX, mouseY);

        SDL_RenderPresent(renderer);

        Uint32 tempo_frame = SDL_GetTicks() - tempo_atual;

        if (tempo_frame < TEMPO_FRAME_MS)
        {
            SDL_Delay(TEMPO_FRAME_MS - tempo_frame);
        }
    }

    liberarAssets(assets);
    liberarIconesToolbar(toolbar);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(janela);
    IMG_Quit();
    SDL_Quit();

    std::cout << "Jogo encerrado com sucesso!" << std::endl;
    return 0;
}
