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

// O fluxo é: arquivo PNG → SDL_Surface (memória RAM) → SDL_Texture (memória GPU) → desenha na tela.

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

    SDL_Texture *texturaTeste = carregarTextura(rendeder, "assets/sprites/teste.png");

    if (!texturaTeste)
    {
        std::cerr << "AVISO: Sprite de teste não encontrado. Coloque um PNG em assets/sprites/teste.png" << std::endl;
        std::cerr << "O jogo vai rodar sem sprite por enquanto." << std::endl;
    }

    int larguraSprite = 0;
    int alturaSprite = 0;

    if (texturaTeste)
    {
        SDL_QueryTexture(texturaTeste, NULL, NULL, &larguraSprite, &alturaSprite);
        std::cerr << "Sprite carregado: " << larguraSprite << "x" << alturaSprite << "pixels" << std::endl;
    }

    float spriteX = (LARGURA_JANELA - larguraSprite) / 2.0f;
    float spriteY = (ALTURA_JANELA - larguraSprite) / 2.0f;

    float velocidadeSprite = 200.0f;

    bool rodando = true;

    SDL_Event evento;

    Uint32 tempo_anterior = SDL_GetTicks();

    float deltaTime = 0.0f;

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

            if (evento.type == SDL_KEYDOWN)
            {

                if (evento.key.keysym.sym == SDLK_ESCAPE)
                {
                    rodando = true;
                }
            }
        }

        const Uint8 *teclas = SDL_GetKeyboardState(NULL);

        if (teclas[SDL_SCANCODE_UP])
        {
            spriteY -= velocidadeSprite * deltaTime;
        }

        if (teclas[SDL_SCANCODE_DOWN])
        {
            spriteY += velocidadeSprite * deltaTime;
        }

        if (teclas[SDL_SCANCODE_LEFT])
        {
            spriteX -= velocidadeSprite * deltaTime;
        }

        if (teclas[SDL_SCANCODE_RIGHT])
        {
            spriteX += velocidadeSprite * deltaTime;
        }

        SDL_SetRenderDrawColor(rendeder, 34, 139, 34, 255);

        SDL_RenderClear(rendeder);

        if (texturaTeste)
        {

            SDL_Rect destino;
            destino.x = static_cast<int>(spriteX);
            destino.y = static_cast<int>(spriteY);
            destino.w = larguraSprite;
            destino.h = alturaSprite;

            SDL_RenderCopy(rendeder, texturaTeste, NULL, &destino);
        }

        SDL_RenderPresent(rendeder);

        Uint32 tempo_frame = SDL_GetTicks() - tempo_atual;

        if (tempo_frame < TEMPO_FRAME_MS)
        {
            SDL_Delay(TEMPO_FRAME_MS - tempo_frame);
        }
    }

    if(texturaTeste){
        SDL_DestroyTexture(texturaTeste);
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
