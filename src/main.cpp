#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <nlohmann/json.hpp>
#include <iostream>

constexpr int LARGURA_JANELA = 1280;
constexpr int ALTURA_JANELA = 720;
constexpr int FPS_ALVO = 60;
constexpr int TEMPO_FRAME_MS = 1000 / FPS_ALVO;

constexpr int TILE_LARGURA = 64;
constexpr int TILE_ALTURA = 32;

constexpr int GRID_COLUNAS = 7;
constexpr int GRID_LINHAS = 5;

constexpr int OFFSET_X = LARGURA_JANELA / 2;
constexpr int OFFSET_Y = 200;

constexpr int CANTEIROS_INICIAIS = 6;

int isoParaTelaX(int coluna, int linha)
{
    return (coluna - linha) * (TILE_LARGURA / 2) + OFFSET_X;
}

int isoParaTelaY(int coluna, int linha)
{
    return (coluna + linha) * (TILE_ALTURA / 2) + OFFSET_Y;
}

float telaParaGridColuna(int mouseX, int mouseY)
{
    float relX = (float)(mouseX - OFFSET_X);
    float relY = (float)(mouseY - OFFSET_Y);

    return (relX / (TILE_LARGURA / 2.0f) + relY / (TILE_ALTURA / 2.0f)) / 2.0f;
}

float telaParaGridLinha(int mouseX, int mouseY)
{
    float relX = (float)(mouseX - OFFSET_X);
    float relY = (float)(mouseY - OFFSET_Y);

    return (relY / (TILE_ALTURA / 2.0f) - relX / (TILE_LARGURA / 2.0f)) / 2.0f;
}

enum EstadoCanteiro
{
    BLOQUEADO = 0,
    VAZIO = 1,
    PLANTADO = 2,
    MADURO = 3,
    RESTOS = 4
};

struct Canteiro
{
    int coluna;
    int linha;
    EstadoCanteiro estado;
};

void desenharLosangoPreenchido(SDL_Renderer *renderer, int centerX, int centerY, int r, int g, int b, int altura = TILE_ALTURA, int largura = TILE_LARGURA)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    for (int dy = -altura / 2; dy <= altura / 2; dy++)
    {
        int meiaLargura = (altura / 2 - abs(dy)) * largura / altura;

        SDL_RenderDrawLine(renderer, centerX - meiaLargura, centerY + dy, centerX + meiaLargura, centerY + dy);
    }
}

void desenharLosangoContorno(SDL_Renderer *renderer, int centerX, int centerY, int r, int g, int b, int altura = TILE_ALTURA, int largura = TILE_LARGURA)
{
    int topoX = centerX;
    int topoY = centerY - altura / 2;

    int direitaX = centerX + largura / 2;
    int direitaY = centerY;

    int baixoX = centerX;
    int baixoY = centerY + altura / 2;

    int esquerdaX = centerX - largura / 2;
    int esquerdaY = centerY;

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    SDL_RenderDrawLine(renderer, topoX, topoY, direitaX, direitaY);

    SDL_RenderDrawLine(renderer, direitaX, direitaY, baixoX, baixoY);

    SDL_RenderDrawLine(renderer, baixoX, baixoY, esquerdaX, esquerdaY);

    SDL_RenderDrawLine(renderer, esquerdaX, esquerdaY, topoX, topoY);
}

SDL_Texture *carregarTextura(SDL_Renderer *renderer, const char *caminho)
{

    SDL_Surface *superficie = IMG_Load(caminho);

    if (!superficie)
    {
        std::cerr << "Erro ao carregar imagem: " << caminho << ":" << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture *textura = SDL_CreateTextureFromSurface(renderer, superficie);

    SDL_FreeSurface(superficie);

    if (!textura)
    {
        std::cerr << "Erro ao criar textura de: " << caminho << ":" << SDL_GetError() << std::endl;
        return nullptr;
    }

    return textura;
}

int main(int agrc, char *agrv[])
{
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0)
    {
        std::cout << "Erro ao inicializar SDL2: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        std::cout << "Erro ao inicializar o SDL2_Image: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    if (TTF_Init() == -1)
    {
        std::cout << "Erro ao inicializar SDL2_ttf: " << TTF_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cout << "Erro ao inicializar SDL2_mixer: " << Mix_GetError() << std::endl;
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Window *janela = SDL_CreateWindow(
        "Colheita Feliz",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        LARGURA_JANELA,
        ALTURA_JANELA,
        SDL_WINDOW_SHOWN);

    if (!janela)
    {
        std::cout << "Erro ao criar a janela: " << SDL_GetError() << std::endl;
        Mix_CloseAudio();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *rendeder = SDL_CreateRenderer(
        janela,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!rendeder)
    {
        std::cout << "Erro ao criar a render: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(janela);
        Mix_CloseAudio();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    std::array<Canteiro, GRID_COLUNAS * GRID_LINHAS> canteiros;

    int desbloqueados = 0;

    for (int linha = 0; linha < GRID_LINHAS; linha++)
    {
        for (int coluna = 0; coluna < GRID_COLUNAS; coluna++)
        {
            int indice = linha * GRID_COLUNAS + coluna;
            canteiros[indice].coluna = coluna;
            canteiros[indice].linha = linha;
            canteiros[indice].estado = (desbloqueados < CANTEIROS_INICIAIS) ? VAZIO : BLOQUEADO;
            if (desbloqueados < CANTEIROS_INICIAIS)
                desbloqueados++;
        }
    }

    std::cout << "Fazenda: " << GRID_COLUNAS << "x" << GRID_LINHAS << " (" << CANTEIROS_INICIAIS << "desbloqueados)" << std::endl;

    int larguraSprite = 0;
    int alturaSprite = 0;

    float spriteX = (LARGURA_JANELA - larguraSprite) / 2.0f;
    float spriteY = (ALTURA_JANELA - larguraSprite) / 2.0f;

    float velocidadeSprite = 200.0f;

    bool rodando = true;

    SDL_Event evento;

    Uint32 tempo_anterior = SDL_GetTicks();

    float deltaTime = 0.0f;

    int canteiroHover = -1;
    int mouseX = 0;
    int mouseY = 0;

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
                if (canteiroHover >= 0)
                {
                    Canteiro &c = canteiros[canteiroHover];
                    std::cout << "Clique (" << c.coluna << "," << c.linha << "): ";
                    switch (c.estado)
                    {
                    case BLOQUEADO:
                        std::cout << "BLOQUEADO" << std::endl;
                        break;
                    case VAZIO:
                        std::cout << "VAZIO" << std::endl;
                        break;
                    case PLANTADO:
                        std::cout << "PLANTADO" << std::endl;
                        break;
                    case MADURO:
                        std::cout << "MADURO" << std::endl;
                        break;
                    case RESTOS:
                        std::cout << "RESTOS" << std::endl;
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

        SDL_SetRenderDrawColor(rendeder, 34, 139, 34, 255);
        SDL_RenderClear(rendeder);

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
                    desenharLosangoPreenchido(rendeder, telaX, telaY, 90, 90, 90);
                    SDL_SetRenderDrawColor(rendeder, 60, 60, 50, 255);
                    SDL_RenderDrawLine(rendeder, telaX - 8, telaY - 4, telaX + 8, telaY + 4);
                    SDL_RenderDrawLine(rendeder, telaX + 8, telaY - 4, telaX - 8, telaY + 4);
                    break;
                case VAZIO:
                    desenharLosangoPreenchido(rendeder, telaX, telaY, 139, 100, 60);
                    break;
                case PLANTADO:
                    desenharLosangoPreenchido(rendeder, telaX, telaY, 120, 80, 45);
                    SDL_SetRenderDrawColor(rendeder, 50, 200, 50, 255);
                    SDL_RenderDrawLine(rendeder, telaX - 8, telaY - 4, telaX + 8, telaY + 4);
                    break;
                case MADURO:
                {
                    desenharLosangoPreenchido(rendeder, telaX, telaY, 120, 80, 45);
                    SDL_SetRenderDrawColor(rendeder, 255, 80, 80, 255);
                    SDL_Rect fruto = {telaX - 4, telaY - 6, 8, 8};
                    SDL_RenderFillRect(rendeder, &fruto);
                    break;
                }
                case RESTOS:
                    desenharLosangoPreenchido(rendeder, telaX, telaY, 110, 95, 70);
                    SDL_SetRenderDrawColor(rendeder, 80, 70, 50, 255);
                    SDL_RenderDrawLine(rendeder, telaX - 6, telaY, telaX + 6, telaY);
                    break;
                }

                if (c.estado != BLOQUEADO)
                {
                    desenharLosangoContorno(rendeder, telaX, telaY, 100, 70, 40);
                }

                if (indice == canteiroHover)
                {
                    desenharLosangoContorno(rendeder, telaX, telaY, 255, 255, 0);
                    desenharLosangoContorno(rendeder, telaX, telaY, 255, 255, 100, TILE_LARGURA - 4, TILE_ALTURA - 2);
                }
            }
        }

        SDL_RenderPresent(rendeder);

        Uint32 tempo_frame = SDL_GetTicks() - tempo_atual;

        if (tempo_frame < TEMPO_FRAME_MS)
        {
            SDL_Delay(TEMPO_FRAME_MS - tempo_frame);
        }
    }

    SDL_DestroyRenderer(rendeder);

    SDL_DestroyWindow(janela);

    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    std::cout << "Jogo encerrado com sucesso!" << std::endl;
    return 0;
}
