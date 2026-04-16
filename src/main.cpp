#include "Constantes.h"
#include "Tipos.h"
#include "Iso.h"
#include "Desenho.h"
#include "Assets.h"
#include "Toolbar.h"
#include "Crops.h"

#include <SDL2/SDL_ttf.h>
#include <array>

int main(int agrc, char *agrv[])
{
    (void)agrc;
    (void)agrv;

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
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

    TTF_Font *fonte = TTF_OpenFont("assets/fonts/Nunito.ttf", 18);
    TTF_Font *fontePequena = TTF_OpenFont("assets/fonts/Nunito.ttf", 14);

    Toolbar toolbar;
    carregarIconesToolbar(renderer, toolbar);

    Assets assets = carregarTodosAssets(renderer);
    CropAssets cropAssets = carregarCropAssets(renderer);

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

    Uint32 tempoJogoMs = 0;
    float velocidadeTempo = VELOCIDADE_TEMPO_NORMAL;

    int canteiroHover = -1;
    int mouseX = 0;
    int mouseY = 0;

    int colheitas = 0;

    while (rodando)
    {
        Uint32 tempo_atual = SDL_GetTicks();

        deltaTime = (tempo_atual - tempo_anterior) / 1000.0f;

        tempo_anterior = tempo_atual;

        tempoJogoMs += static_cast<Uint32>(deltaTime * velocidadeTempo * 1000.0f);

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
                if (toolbar.painelAberto)
                {
                    int resultado = painelSementeHitTest(evento.button.x, evento.button.y);

                    if (resultado >= 0)
                    {
                        toolbar.sementeSelecionada = resultado;
                        toolbar.painelAberto = false;
                    }
                    else if (resultado == -2)
                    {
                        toolbar.painelAberto = false;
                    }
                    else if (resultado == -1)
                    {
                        toolbar.painelAberto = false;
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
                            toolbar.painelAberto = !toolbar.painelAberto;
                            toolbar.selecionada = SACOLA;
                        }
                        else
                        {
                            toolbar.selecionada = nova;
                            toolbar.painelAberto = false;
                        }
                    }
                    else if (canteiroHover >= 0)
                    {
                        Canteiro &c = canteiros[canteiroHover];
                        switch (toolbar.selecionada)
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
                            if (c.estado == VAZIO && toolbar.sementeSelecionada >= 0)
                            {
                                c.estado = PLANTADO;
                                c.tipoCrop = toolbar.sementeSelecionada;
                                c.estagioCrop = 1;
                                c.timestampPlantio = tempoJogoMs;
                                c.temporadaAtual = 1;
                            }
                            break;
                        case MAO:
                            if (c.estado == MADURO)
                            {
                                colheitas++;
                                c.temporadaAtual++;

                                int totalTemp = TABELA_CROPS[c.tipoCrop].temporadas;
                                if (c.temporadaAtual <= totalTemp)
                                {
                                    c.estado = PLANTADO;
                                    c.estagioCrop = 1;
                                    c.timestampPlantio = tempoJogoMs;
                                }
                                else
                                {
                                    c.estado = RESTOS;
                                    c.tipoCrop = -1;
                                    c.estagioCrop = 0;
                                    c.timestampPlantio = 0;
                                    c.temporadaAtual = 0;
                                }
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
            }

            if (evento.type == SDL_KEYDOWN)
            {
                if (evento.key.keysym.sym == SDLK_ESCAPE)
                {
                    if (toolbar.painelAberto)
                    {
                        toolbar.painelAberto = false;
                    }
                    else
                    {
                        rodando = false;
                    }
                }

                if (evento.key.keysym.sym == SDLK_t)
                {
                    velocidadeTempo = VELOCIDADE_TEMPO_DEBUG;
                }
            }

            if (evento.type == SDL_KEYUP)
            {
                if (evento.key.keysym.sym == SDLK_t)
                {
                    velocidadeTempo = VELOCIDADE_TEMPO_NORMAL;
                }
            }
        }

        atualizarAnimacoes(toolbar, deltaTime);

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

            for (int i = 0; i < GRID_COLUNAS * GRID_LINHAS; i++)
            {
                Canteiro &c = canteiros[i];

                if (c.estado != PLANTADO)
                    continue;
                if (c.tipoCrop < 0)
                    continue;

                Uint32 passadoMs = tempoJogoMs - c.timestampPlantio;

                Uint32 totalMs = static_cast<Uint32>(TABELA_CROPS[c.tipoCrop].tempoTotalSegundos * 1000);

                float fracao = static_cast<float>(passadoMs) / static_cast<float>(totalMs);
                if (fracao > 1.0f)
                    fracao = 1.0f;

                int novoEstagio = static_cast<int>(fracao * TOTAL_ESTAGIOS) + 1;
                if (novoEstagio > TOTAL_ESTAGIOS)
                    novoEstagio = TOTAL_ESTAGIOS;

                c.estagioCrop = novoEstagio;

                if (fracao >= 1.0f)
                {
                    c.estado = MADURO;
                }
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

                    desenharCrop(renderer, cropAssets, c.tipoCrop, c.estagioCrop, telaX, telaY);

                    if (c.tipoCrop >= 0)
                    {
                        Uint32 passado = tempoJogoMs - c.timestampPlantio;
                        Uint32 total = static_cast<Uint32>(TABELA_CROPS[c.tipoCrop].tempoTotalSegundos) * 1000;
                        float frac = static_cast<float>(passado) / static_cast<float>(total);
                        desenharBarraProgresso(renderer, telaX, telaY, frac);
                    }
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

                    desenharCrop(renderer, cropAssets, c.tipoCrop, TOTAL_ESTAGIOS, telaX, telaY);

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

        SDL_Texture *sementeIcone = nullptr;
        if (toolbar.sementeSelecionada >= 0 && toolbar.sementeSelecionada < TOTAL_CROPS)
        {
            sementeIcone = cropAssets.sementes[toolbar.sementeSelecionada];
        }

        desenharToolbar(renderer, toolbar, sementeIcone);

        desenharPainelSementes(renderer, fonte, fontePequena, cropAssets, toolbar);

        desenharCursorFerramenta(renderer, toolbar, mouseX, mouseY, sementeIcone);

        SDL_RenderPresent(renderer);

        Uint32 tempo_frame = SDL_GetTicks() - tempo_atual;

        if (tempo_frame < TEMPO_FRAME_MS)
        {
            SDL_Delay(TEMPO_FRAME_MS - tempo_frame);
        }
    }
    liberarCropAssets(cropAssets);
    liberarAssets(assets);
    liberarIconesToolbar(toolbar);
    if (fontePequena)
        TTF_CloseFont(fontePequena);
    if (fonte)
        TTF_CloseFont(fonte);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(janela);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    std::cout << "Jogo encerrado com sucesso!" << std::endl;
    return 0;
}
