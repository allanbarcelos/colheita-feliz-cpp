#!/usr/bin/env bash
# Hot-reload watcher: rebuilda a lib do jogo quando qualquer src/*.cpp ou src/*.h muda.
# O executável detecta o novo mtime e recarrega sozinho (Windows: game.dll, Linux: game.so, macOS: game.dylib).

set -u

PROJETO_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$PROJETO_DIR/build"
TARGET="game"
INTERVALO=0.5

ultimo_hash=""

hash_stdin() {
    if command -v md5sum >/dev/null 2>&1; then
        md5sum | awk '{print $1}'
    else
        md5 -q
    fi
}

calcular_hash() {
    if find "$PROJETO_DIR/src" -type f \( -name "*.cpp" -o -name "*.h" \) -printf "%T@ %p\n" >/dev/null 2>&1; then
        find "$PROJETO_DIR/src" -type f \( -name "*.cpp" -o -name "*.h" \) -printf "%T@ %p\n" | hash_stdin
    else
        find "$PROJETO_DIR/src" -type f \( -name "*.cpp" -o -name "*.h" \) -exec stat -f "%m %N" {} \; | hash_stdin
    fi
}

echo "[watch] monitorando src/ — ctrl+c pra sair"
echo "[watch] build dir: $BUILD_DIR"
echo "[watch] target: $TARGET"

while true; do
    hash_atual=$(calcular_hash)

    if [ "$hash_atual" != "$ultimo_hash" ]; then
        if [ -n "$ultimo_hash" ]; then
            echo "[watch] mudança detectada — rebuilding..."
            if cmake --build "$BUILD_DIR" --target "$TARGET" 2>&1 | tail -20; then
                echo "[watch] ✔ game lib atualizada"
            else
                echo "[watch] ✗ build falhou"
            fi
        fi
        ultimo_hash="$hash_atual"
    fi

    sleep "$INTERVALO"
done
