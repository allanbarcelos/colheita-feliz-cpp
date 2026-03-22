# 🌾 Colheita Feliz em C++

**Building a classic Brazilian farming game from scratch in C++ with SDL2**

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus)
![SDL2](https://img.shields.io/badge/SDL2-2.x-green?logo=data:image/svg+xml;base64,)
![License](https://img.shields.io/badge/License-MIT-yellow)
[![YouTube](https://img.shields.io/badge/YouTube-@harlleybastos-red?logo=youtube)](https://www.youtube.com/@harlleybastos)

---

## About

**Colheita Feliz** (Happy Harvest) is a recreation of the classic farming game from the Orkut era — one of the most beloved games among Brazilian internet users in the late 2000s. This project builds the game **entirely from scratch** in C++ using SDL2, with no game engine and no AI-generated code.

Every single line of code is written by hand and explained step-by-step in the [YouTube video series](https://www.youtube.com/@harlleybastos). The series is recorded in **Portuguese (pt-BR)**, and code comments are also in Portuguese.

This is a learning journey — from zero to a fully playable isometric farming game.

---

## 🎬 Video Series

| Episode | Title | Topics | Branch | YouTube |
|---------|-------|--------|--------|---------|
| EP01 | Configuração do Ambiente | MSYS2 setup, SDL2 + CMake, compilation test, library initialization | [`ep01-setup-sdl2`](../../tree/ep01-setup-sdl2) | [Watch](https://youtu.be/4BYEflGqU7o) |
| EP02 | Janela + Game Loop | SDL_CreateWindow, SDL_CreateRenderer, game loop, delta time, FPS control | [`ep02-janela-gameloop`](../../tree/ep02-janela-gameloop) | [Watch](#) |
| EP03 | Sprites + Movimento | Load textures, render sprites, keyboard movement with arrow keys | [`ep03-sprites-movimento`](../../tree/ep03-sprites-movimento) | [Watch](#) |
| EP04 | Isométrico | Isometric fundamentals, coordinate conversion, mouse picking | [`ep04-isometrico`](../../tree/ep04-isometrico) | *Coming Soon* |
| EP05 | Tilemap + Câmera | Tilemap rendering, camera system, scrolling | [`ep05-tilemap-camera`](../../tree/ep05-tilemap-camera) | *Coming Soon* |
| EP06 | Personagem + Animação | Character sprite, spritesheet animation, directional movement | [`ep06-personagem-animacao`](../../tree/ep06-personagem-animacao) | *Coming Soon* |

---

## 🛠 Tech Stack

| Tool / Library | Purpose |
|----------------|---------|
| **C++17** | Language standard |
| **SDL2** | Window, rendering, input, events |
| **SDL2_image** | PNG/image loading |
| **SDL2_mixer** | Audio playback |
| **SDL2_ttf** | TrueType font rendering |
| **nlohmann_json** | JSON parsing (save files, config) |
| **CMake** | Build system |
| **MinGW-w64** | Compiler toolchain (via MSYS2 UCRT64) |

---

## 📁 Project Structure

```
ColheitaFeliz/
├── src/
│   └── main.cpp
├── assets/
│   └── sprites/
│       ├── teste.png
│       ├── jogador.png
│       ├── jogador_spritesheet.png
│       └── tileset.png
├── build/
├── CMakeLists.txt
├── SETUP_GUIDE.md
└── README.md
```

---

## 🚀 Getting Started

### Prerequisites

1. Install [MSYS2](https://www.msys2.org/) on Windows.
2. Open the **UCRT64** terminal and install the required packages:

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

> 📄 A **free PDF setup guide** with screenshots is available — check the description of [EP01 on YouTube](https://youtu.be/4BYEflGqU7o).

### Clone & Build

```bash
git clone https://github.com/harlleybastos/ColheitaFeliz.git
cd ColheitaFeliz

# Configure
cmake -B build -G Ninja

# Build
cmake --build build

# Run
./build/ColheitaFeliz.exe
```

> See [SETUP_GUIDE.md](SETUP_GUIDE.md) for a detailed setup walkthrough in Portuguese.

---

## 🌿 Branch Strategy

Each branch corresponds to a specific episode of the video series. The code evolves incrementally — each episode builds on top of the previous one.

- **`main`** — Always contains the latest stable code (currently up to EP03).
- **`ep01-setup-sdl2`** — Code as it was at the end of Episode 01.
- **`ep02-janela-gameloop`** — Code as it was at the end of Episode 02.
- **`ep03-sprites-movimento`** — Code as it was at the end of Episode 03.
- **`ep04-isometrico`** through **`ep06-personagem-animacao`** — Upcoming episodes (branches created as placeholders).

To follow along with a specific episode, checkout its branch:

```bash
git checkout ep01-setup-sdl2
```

---

## 🎨 Assets

Sprites were generated using AI (**Google Gemini**) and manually adjusted in **Adobe Photoshop**.

- **Tileset:** 320×32px — 5 isometric tiles (64×32px each)
- **Character spritesheet:** 128×192px — 4 directions × 4 frames (32×48px per frame)

All assets are located in the `assets/` directory.

---

## 👤 Author

**Harlley Bastos** — Brazilian developer with 7+ years of experience in Ruby/Rails, TypeScript, Node.js, and React. Currently learning C++ game development from scratch and documenting the entire journey on YouTube.

- 🎥 [YouTube — @harlleybastos](https://www.youtube.com/@harlleybastos)
- 💻 [GitHub — harlleybastos](https://github.com/harlleybastos)

---

## 📄 License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.

---

⭐ **If you're following the series, consider starring the repo and subscribing to the channel!**
