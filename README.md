# 🌾 Fazenda dos Sonhos (Colheita Feliz em C++)

**A classic Brazilian farming game, rebuilt from scratch in C++ with SDL2. No engine. No AI-generated code.**

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus)
![SDL2](https://img.shields.io/badge/SDL2-2.x-green)
![Release](https://img.shields.io/badge/release-v1.0.0--desktop-brightgreen)
![License](https://img.shields.io/badge/License-Source_Available_NC-yellow)
[![YouTube](https://img.shields.io/badge/YouTube-@harlleybastos-red?logo=youtube)](https://www.youtube.com/@harlleybastos)
[![Discord](https://img.shields.io/badge/Discord-HB_Insiders-5865F2?logo=discord)](https://discord.gg/tHMnVajhq5)

---

## 🎮 Joga agora — grátis

> O jogo agora se chama **Fazenda dos Sonhos**. Roda 100% offline no Windows e online no navegador.

| Plataforma | Como |
|------------|------|
| **🖥️ Desktop (Windows)** | Baixa o `.zip` em [Releases → v1.0.0-desktop](https://github.com/harlleybastos/colheita-feliz-cpp/releases/tag/v1.0.0-desktop) → descompacta → executa `FazendaDosSonhos.exe`. Sem instalação, sem dependência. |
| **🌐 Browser (qualquer device)** | Acessa [**fazendadossonhos.app**](https://fazendadossonhos.app) — login Google, joga solo ou com amigos via multiplayer real-time. |

---

## About

**Fazenda dos Sonhos** (originalmente **Colheita Feliz em C++**) é uma recriação do clássico jogo de fazenda da era Orkut — um dos jogos mais queridos pela internet brasileira no fim dos anos 2000. Construído **inteiramente do zero** em C++ com SDL2, sem engine de jogo e sem código gerado por IA.

Cada linha de código foi escrita à mão e explicada passo-a-passo na [série de vídeos do YouTube](https://www.youtube.com/@harlleybastos), do EP01 ao EP09. As fases finais (HUD completo, save/load, missões, animais, decorações, som) foram desenvolvidas em silêncio após decisão editorial documentada no vídeo finale — retenção das fases técnicas estava abaixo do padrão da série, e o conteúdo denso de game dev migrou para o curso pago [**A Forja**](https://harlleybastos.com.br/forge).

Esse repositório fica como **artefato pedagógico permanente** — fork à vontade, estuda à vontade, abre PR se quiser. Versão `v1.0.0-desktop` é o jogo solo completo. Versão multiplayer + web (Fazenda dos Sonhos online) tem código privado, mas é grátis pra jogar em [fazendadossonhos.app](https://fazendadossonhos.app).

---

## 🎬 Série de vídeos

| Episódio | Título | Tópicos | Branch | YouTube |
|---|---|---|---|---|
| EP01 | Comecei a Programar a Colheita Feliz | MSYS2 setup, MinGW-w64, SDL2 + extensões, CMake, compilation test | [`ep01-setup-sdl2`](https://github.com/harlleybastos/colheita-feliz-cpp/tree/ep01-setup-sdl2) | [Watch](https://youtu.be/CXxo-gZYqQM) |
| EP02 | Criei a Janela e Game Loop | SDL_CreateWindow 1280×720, SDL_CreateRenderer, game loop, delta time, FPS control | [`ep02-janela-gameloop`](https://github.com/harlleybastos/colheita-feliz-cpp/tree/ep02-janela-gameloop) | [Watch](https://youtu.be/rf4Eg0WMPH0) |
| EP03 | Coloquei o Primeiro Sprite | carregarTextura(), IMG_Load, SDL_RenderCopy, keyboard movement | [`ep03-sprites-movimento`](https://github.com/harlleybastos/colheita-feliz-cpp/tree/ep03-sprites-movimento) | [Watch](https://youtu.be/egJIi6eT0dc) |
| EP04 | Projeção Isométrica da Fazenda | Iso conversion (isoParaTelaX/Y), inverse formula, mouse picking, farm grid 7×5 | [`ep04-isometrico`](https://github.com/harlleybastos/colheita-feliz-cpp/tree/ep04-isometrico) | [Watch](https://youtu.be/qL7-9T1GlBM) |
| EP05 | Coloquei Sprites Reais na Fazenda | Tileset PNG, background, decorações, code reorganization (headers separados) | [`ep05-tileset-sprites`](https://github.com/harlleybastos/colheita-feliz-cpp/tree/ep05-tileset-sprites) | [Watch](https://youtu.be/D4Fu3haQnF4) |
| EP06 | Toolbar + Sistema de Ferramentas | Barra de ferramentas com 7 ícones, hit testing, ações por estado × ferramenta | [`ep06-toolbar-system`](https://github.com/harlleybastos/colheita-feliz-cpp/tree/ep06-toolbar-system) | [Watch](https://youtu.be/BdXGabPxh04) |
| EP07 | Sementes, Plantio e Estágios Visuais | 15 tipos de sementes (TABELA_CROPS), 5 estágios de crescimento, painel grade 5×3, SDL2_ttf + Google Fonts | [`ep07-crop-system`](https://github.com/harlleybastos/colheita-feliz-cpp/tree/ep07-crop-system) | [Watch](https://youtu.be/Ii06q8NsmSY) |
| EP08 | A Fazenda Agora Cresce Sozinha | Game speed control com tecla T (modo turbo), debug speed multiplier, fix de SDL_KEYDOWN/KEYUP | [`ep08-accelerated-time-system`](https://github.com/harlleybastos/colheita-feliz-cpp/tree/ep08-accelerated-time-system) | [Watch](https://youtu.be/nzBNxl-gdrk) |
| EP09 | TRANSFORMEI minha FAZENDA em NEGÓCIO | Hot-reload com DLL (Handmade Hero pattern), config.ini runtime via F5, eventos aleatórios (seca, ervas, minhocas), Loja, Depósito, modo compra de canteiros, sistema de inventário e ouro | [`ep09-economia-eventos`](https://github.com/harlleybastos/colheita-feliz-cpp/tree/ep09-economia-eventos) | [Watch](https://youtu.be/oWTvi2W3TXg) |
| **EP10–EP11** | Implementadas em silêncio | Animais (galinha/vaca/ovelha/cachorro com IA de patrol), decorações fixas, sistema de áudio completo (SDL2_mixer + painel Settings), save/load JSON, tela título com glow + tips, polish e balanceamento, build de release multiplataforma | `main` | (não publicadas — ver [vídeo finale](https://www.youtube.com/@harlleybastos)) |

> **Decisão editorial 2026-04-26:** retenção média das fases técnicas (EP07-EP09) caiu pra 11-18% vs 30%+ dos vídeos de carreira/gringa. Em vez de continuar o passo-a-passo e ver o canal definhar, fechei as fases 6-11 em silêncio e mostrei tudo num **vídeo finale único** lançado em 2026-05-04. O conteúdo denso de game dev (C++ → OpenGL → Vulkan) migrou pro curso pago [**A Forja**](https://harlleybastos.com.br/forge), onde profundidade faz sentido.

---

## 🛠 Tech Stack

| Tool / Library | Purpose |
|---|---|
| **C++17** | Linguagem |
| **SDL2** | Window, rendering, input, eventos |
| **SDL2_image** | PNG/image loading |
| **SDL2_mixer** | Audio playback (SFX + música tema) |
| **SDL2_ttf** | TrueType font rendering |
| **nlohmann_json** | Save files + config parsing |
| **CMake** (3.20+) | Build system |
| **MinGW-w64 (UCRT64)** | Compiler toolchain via MSYS2 |

Para a versão multiplayer + web (`fazendadossonhos.app`, repo privado): Emscripten WebAssembly · Next.js 16 · Auth.js v5 · WebSocket · PostgreSQL + Prisma · Redis · Asaas · Cloudflare · Hetzner CX22 · Docker Swarm · Sentry · Uptime Kuma.

---

## 📁 Estrutura do projeto

```
ColheitaFeliz/
├── src/
│   ├── main.cpp          ← game loop, inicialização, eventos
│   ├── game.cpp          ← lógica do jogo (hot-reload DLL)
│   ├── Constantes.h      ← TODAS as constantes
│   ├── Tipos.h           ← enums + structs
│   ├── Iso.h             ← funções de conversão isométrica
│   ├── Desenho.h         ← funções de desenho
│   ├── Assets.h          ← sprite loading + cleanup
│   ├── Toolbar.h         ← toolbar + painel de sementes
│   ├── Crops.h           ← TABELA_CROPS (15 sementes)
│   ├── Eventos.h         ← seca / pragas / sistema de saúde
│   ├── Animais.h         ← galinha / vaca / ovelha / cachorro
│   ├── Audio.h           ← SDL2_mixer SFX + música
│   ├── Save.h            ← serialize/deserialize JSON
│   ├── TelaTitulo.h      ← logo / tips / botões
│   └── app.rc            ← Windows resource (icon)
├── assets/
│   ├── sprites/          ← 177 PNGs (tiles, crops, animais, UI, decorações)
│   ├── sounds/           ← 18 SFX + música tema
│   ├── backgrounds/
│   └── fonts/            ← Nunito (Google Fonts)
├── dist/
│   └── Fazenda-dos-Sonhos-v1.0.0-desktop-windows.zip   ← release pronto (45MB)
├── build-release/        ← binários de release (FazendaDosSonhos.exe + game.dll)
├── CMakeLists.txt
├── SETUP_GUIDE.md
└── README.md
```

---

## 🚀 Getting Started (build do código)

### Pré-requisitos

1. Instalar [MSYS2](https://www.msys2.org/) no Windows.
2. Abrir terminal **UCRT64** e instalar dependências:

```bash
pacman -Syu
pacman -S mingw-w64-ucrt-x86_64-gcc \
          mingw-w64-ucrt-x86_64-cmake \
          mingw-w64-ucrt-x86_64-ninja \
          mingw-w64-ucrt-x86_64-gdb \
          mingw-w64-ucrt-x86_64-SDL2 \
          mingw-w64-ucrt-x86_64-SDL2_image \
          mingw-w64-ucrt-x86_64-SDL2_mixer \
          mingw-w64-ucrt-x86_64-SDL2_ttf \
          mingw-w64-ucrt-x86_64-nlohmann-json
```

> 📄 PDF de setup grátis com screenshots disponível na descrição do [EP01 no YouTube](https://youtu.be/CXxo-gZYqQM).

### Clone & Build

```bash
git clone https://github.com/harlleybastos/colheita-feliz-cpp.git
cd colheita-feliz-cpp

cmake -S . -B build -G Ninja
cmake --build build
./build/FazendaDosSonhos.exe
```

> Veja [SETUP_GUIDE.md](SETUP_GUIDE.md) pra walkthrough detalhado em PT-BR.

---

## 🌿 Branch Strategy

Cada branch corresponde a um episódio específico da série. O código evolui incrementalmente — cada episódio constrói em cima do anterior.

- **`main`** — código completo + estável até `v1.0.0-desktop` (15 fases — EP01-EP11).
- **`ep01-setup-sdl2`** até **`ep09-economia-eventos`** — snapshots por episódio publicado.
- EP10 e EP11 não têm branch dedicada (foram desenvolvidas em silêncio na `main`).

Pra acompanhar um episódio específico:

```bash
git checkout ep01-setup-sdl2
```

---

## 🎨 Assets

Sprites gerados com **Google Gemini (Nano Banana 2)** + ajustes manuais em **Adobe Photoshop**. 177 sprites totais — tiles isométricos, 15 plantas em 5 estágios cada, animais (galinha/vaca/ovelha/cachorro com 2-3 poses cada), decorações fixas (casa, casinha do cachorro, cerca, espantalho, banco, churrasqueira), UI elements.

Áudio: 18 SFX (cá-ching, swoosh, bip, click) + 1 música tema. Todos os assets ficam em `assets/`.

---

## 📚 Quer aprender game dev de verdade?

O passo-a-passo desse projeto é só a porta de entrada. Pra fundamentos profundos de **C++ → OpenGL → Vulkan** com profundidade que não cabe em vídeo de YouTube, considera A Forja:

> 🔗 **[harlleybastos.com.br/forge](https://harlleybastos.com.br/forge)** — lista de espera com 50% off no early bird (R$ 397 vs R$ 797 padrão). Lançamento set/out 2026, primeiro lote 50-100 alunos. Bônus: PDF "Roadmap de Arquitetura Gráfica em C++" (5 páginas) direto no email ao entrar na lista.

E pra fazer parte da galera que joga primeiro:

> 💬 **[Discord HB Insiders](https://discord.gg/tHMnVajhq5)** — comunidade do canal. Game dev em C++, carreira em gringa, inglês pra dev. Quem entra ajuda quem entra depois.

---

## 👤 Autor

**Harlley Bastos** — dev brasileiro com 7+ anos de Ruby/Rails, TypeScript, Node.js, React. Aprendeu C++ game dev do zero documentando a jornada inteira no YouTube.

- 🎥 [YouTube — @harlleybastos](https://www.youtube.com/@harlleybastos)
- 💻 [GitHub — harlleybastos](https://github.com/harlleybastos)
- 🌐 [harlleybastos.com.br](https://harlleybastos.com.br)

---

## 📄 License

Source Available — Non-Commercial. Livre pra uso, estudo, modificação em projetos pessoais e de aprendizado. **Uso comercial não permitido** sem autorização escrita do autor. Veja [LICENSE](LICENSE).

---

## 🎬 Vídeo finale da saga

> [PAREI de GRAVAR pra TERMINAR esse JOGO em C++ (e DEU CERTO)](https://www.youtube.com/@harlleybastos) — 2026-05-04 · 28-35min · último vídeo da série.

⭐ Se você seguiu a saga inteira, considera dar star no repo e se inscrever no canal. Foi um ano de jornada — obrigado por estar do outro lado da tela.
