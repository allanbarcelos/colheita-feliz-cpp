# 🛠 Guia de Configuração — Colheita Feliz em C++

Guia completo para configurar o ambiente de desenvolvimento no Windows usando MSYS2 e VS Code.

---

## 1. Instalando o MSYS2

1. Baixe o instalador em [msys2.org](https://www.msys2.org/).
2. Execute o instalador e siga os passos padrão (pode manter o diretório `C:\msys64`).
3. Ao finalizar, o terminal MSYS2 vai abrir automaticamente. Feche-o.
4. Abra o terminal **UCRT64** (procure por "MSYS2 UCRT64" no menu Iniciar).

> ⚠️ **Importante:** Sempre use o terminal **UCRT64**, não o MSYS2 padrão. O UCRT64 usa o Universal C Runtime, que é o mais compatível com aplicações Windows modernas.

---

## 2. Instalando os Pacotes Necessários

No terminal UCRT64, atualize o sistema e instale todas as dependências:

```bash
# Atualizar o sistema (pode pedir para fechar e reabrir o terminal)
pacman -Syu

# Compilador, CMake, Ninja e depurador
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-cmake
pacman -S mingw-w64-ucrt-x86_64-ninja
pacman -S mingw-w64-ucrt-x86_64-gdb

# Bibliotecas SDL2
pacman -S mingw-w64-ucrt-x86_64-SDL2
pacman -S mingw-w64-ucrt-x86_64-SDL2_image
pacman -S mingw-w64-ucrt-x86_64-SDL2_mixer
pacman -S mingw-w64-ucrt-x86_64-SDL2_ttf

# JSON
pacman -S mingw-w64-ucrt-x86_64-nlohmann-json
```

### Verificando a instalação

```bash
gcc --version      # Deve mostrar a versão do GCC
cmake --version    # Deve mostrar a versão do CMake
ninja --version    # Deve mostrar a versão do Ninja
```

---

## 3. Configurando o VS Code

### Extensões recomendadas

Instale as seguintes extensões no VS Code:

| Extensão | ID | Finalidade |
|----------|----|------------|
| C/C++ | `ms-vscode.cpptools` | IntelliSense, debugging, navegação de código |
| CMake Tools | `ms-vscode.cmake-tools` | Integração com CMake (configurar, compilar, rodar) |
| CMake Language Support | `twxs.cmake` | Syntax highlighting para CMakeLists.txt |

### Configurando o PATH

Para que o VS Code encontre o compilador e as ferramentas, adicione o diretório do UCRT64 ao PATH do Windows:

1. Abra **Configurações do Sistema** → **Variáveis de Ambiente**.
2. Na variável `Path` do **usuário**, adicione:
   ```
   C:\msys64\ucrt64\bin
   ```
3. Reinicie o VS Code após a alteração.

### IntelliSense (c_cpp_properties.json)

Se o IntelliSense não estiver encontrando os headers do SDL2, crie ou edite o arquivo `.vscode/c_cpp_properties.json`:

```json
{
    "configurations": [
        {
            "name": "UCRT64",
            "includePath": [
                "${workspaceFolder}/**",
                "C:/msys64/ucrt64/include/**"
            ],
            "defines": [],
            "compilerPath": "C:/msys64/ucrt64/bin/gcc.exe",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "gcc-x64",
            "configurationProvider": "ms-vscode.cmake-tools"
        }
    ],
    "version": 4
}
```

---

## 4. Compilando e Executando o Projeto

### Pelo terminal

```bash
# Abra o terminal UCRT64 e navegue até a pasta do projeto
cd /home/SeuUsuario/ColheitaFeliz

# Configurar o build (só precisa fazer uma vez)
cmake -B build -G Ninja

# Compilar
cmake --build build

# Executar
./build/ColheitaFeliz.exe
```

### Pelo VS Code (CMake Tools)

1. Abra a pasta do projeto no VS Code.
2. Pressione `Ctrl+Shift+P` e selecione **CMake: Configure**.
3. Escolha o kit **GCC (UCRT64)**.
4. Pressione `Ctrl+Shift+P` e selecione **CMake: Build**.
5. Para rodar, pressione `Ctrl+Shift+P` e selecione **CMake: Run Without Debugging**.

---

## 5. Solução de Problemas

### "SDL.h: No such file or directory"

O compilador não encontra os headers do SDL2. Verifique:

- Você instalou os pacotes `mingw-w64-ucrt-x86_64-SDL2` (e os demais)?
- O terminal que você está usando é o **UCRT64** (e não o MSYS2 padrão)?
- O caminho `C:\msys64\ucrt64\include\SDL2\` contém os arquivos `.h`?

### IntelliSense mostrando erros, mas o projeto compila

O IntelliSense pode não estar configurado corretamente. Verifique:

- O arquivo `.vscode/c_cpp_properties.json` está configurado conforme a seção 3.
- O `compilerPath` aponta para `C:/msys64/ucrt64/bin/gcc.exe`.
- Reinicie o VS Code após criar/editar o arquivo.

### "The program can't start because SDL2.dll is missing"

Ao rodar o `.exe` fora do terminal UCRT64, o Windows pode não encontrar as DLLs. Soluções:

1. **Recomendado:** Execute pelo terminal UCRT64 (que já tem as DLLs no PATH).
2. **Alternativa:** Copie as DLLs necessárias para a pasta `build/`:
   ```bash
   cp /ucrt64/bin/SDL2.dll build/
   cp /ucrt64/bin/SDL2_image.dll build/
   cp /ucrt64/bin/SDL2_mixer.dll build/
   cp /ucrt64/bin/SDL2_ttf.dll build/
   ```
3. **Alternativa permanente:** Adicione `C:\msys64\ucrt64\bin` ao PATH do Windows (veja seção 3).

### CMake não encontra o Ninja

Certifique-se de que o pacote `mingw-w64-ucrt-x86_64-ninja` está instalado:

```bash
pacman -S mingw-w64-ucrt-x86_64-ninja
```

E que `C:\msys64\ucrt64\bin` está no PATH do Windows.

### O executável roda mas a janela não aparece

- Verifique se não há erro no console (execute pelo terminal para ver as mensagens).
- Certifique-se de que o `SDL_WINDOW_SHOWN` está sendo passado na criação da janela.
- No CMakeLists.txt, o `WIN32` em `add_executable` esconde o console. Remova temporariamente para debug:
  ```cmake
  add_executable(ColheitaFeliz src/main.cpp)  # sem WIN32
  ```

### Sprites não aparecem na tela

- O caminho dos assets é **relativo ao diretório de execução**. Execute o jogo a partir da raiz do projeto:
  ```bash
  cd /home/SeuUsuario/ColheitaFeliz
  ./build/ColheitaFeliz.exe
  ```
- Verifique se o arquivo `assets/sprites/teste.png` existe.

---

## 📚 Recursos Adicionais

- [Documentação oficial do SDL2](https://wiki.libsdl.org/)
- [MSYS2 — Guia de pacotes](https://packages.msys2.org/)
- [Série no YouTube — @harlleybastos](https://www.youtube.com/@harlleybastos)
- [PDF de configuração gratuito](https://youtu.be/CXxo-gZYqQM) (link na descrição do EP01)
