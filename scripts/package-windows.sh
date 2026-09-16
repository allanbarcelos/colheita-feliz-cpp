#!/usr/bin/env bash
# Empacota o build MinGW/UCRT64: exe + game.dll + DLLs transitivas + assets.
set -euo pipefail

BUILD_DIR="${1:?build dir}"
DEST="${2:?dest dir}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"

rm -rf "$DEST"
mkdir -p "$DEST"

cp "$BUILD_DIR/FazendaDosSonhos.exe" "$DEST/"
cp "$BUILD_DIR/game.dll" "$DEST/"
cp -R "$ROOT/assets" "$DEST/assets"
cp "$ROOT/LICENSE" "$DEST/LICENSE.txt"

list_ucrt_deps() {
    local bin="$1"
    if command -v ntldd >/dev/null 2>&1; then
        ntldd -R "$bin" 2>/dev/null | tr '\\' '/' | awk '{
            for (i = 1; i <= NF; i++)
                if ($i ~ /[Uu][Cc][Rr][Tt]64/) print $i
        }'
    else
        ldd "$bin" 2>/dev/null | awk '/\/ucrt64\// { print $3 }'
    fi
}

copy_ucrt_deps() {
    local need_more=yes
    while [ "$need_more" = yes ]; do
        need_more=no
        shopt -s nullglob
        for bin in "$DEST"/*.exe "$DEST"/*.dll; do
            while read -r lib; do
                lib="${lib%$'\r'}"
                [ -n "$lib" ] && [ -f "$lib" ] || continue
                local base
                base="$(basename "$lib")"
                if [ ! -f "$DEST/$base" ]; then
                    cp "$lib" "$DEST/$base"
                    need_more=yes
                fi
            done < <(list_ucrt_deps "$bin")
        done
        shopt -u nullglob
    done
}

copy_ucrt_deps

cat > "$DEST/LEIA-ME.txt" <<'EOF'
Fazenda dos Sonhos — Windows
============================

1. Extraia esta pasta inteira (não rode o .exe de dentro do zip).
2. Dê dois cliques em FazendaDosSonhos.exe.
3. Save fica em %APPDATA%\FazendaDosSonhos\FazendaDosSonhos\save.json

Não apague game.dll nem a pasta assets — o jogo precisa dos dois.
EOF

echo "[package-windows] pronto em $DEST"
ls -lh "$DEST" | head -40
