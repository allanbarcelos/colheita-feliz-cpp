#!/usr/bin/env bash
# Empacota o build macOS: binário + game.dylib + dylibs Homebrew + assets.
set -euo pipefail

BUILD_DIR="${1:?build dir}"
DEST="${2:?dest dir}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"

rm -rf "$DEST"
mkdir -p "$DEST"

cp "$BUILD_DIR/FazendaDosSonhos" "$DEST/"
cp "$BUILD_DIR/game.dylib" "$DEST/"
chmod +w "$DEST/FazendaDosSonhos" "$DEST/game.dylib"
cp -R "$ROOT/assets" "$DEST/assets"
cp "$ROOT/LICENSE" "$DEST/LICENSE.txt"

is_system_lib() {
    case "$1" in
        /usr/lib/*|/System/*) return 0 ;;
        @executable_path/*|@loader_path/*) return 0 ;;
    esac
    return 1
}

rpaths_of() {
    otool -l "$1" 2>/dev/null | awk '
        /cmd LC_RPATH/ { want=1; next }
        want && /path / { print $2; want=0 }
    '
}

find_named_lib() {
    local name="$1"
    local bin="$2"
    local rp candidate
    if [ -f "$DEST/$name" ]; then
        echo "$DEST/$name"
        return 0
    fi
    while read -r rp; do
        [ -n "$rp" ] || continue
        candidate="${rp%/}/$name"
        if [ -e "$candidate" ]; then
            python3 -c 'import os,sys; print(os.path.realpath(sys.argv[1]))' "$candidate"
            return 0
        fi
    done < <(rpaths_of "$bin")
    local prefix
    prefix="$(brew --prefix 2>/dev/null || echo /opt/homebrew)"
    for candidate in "$prefix/lib/$name" "$prefix/opt/"*/lib/"$name"; do
        if [ -e "$candidate" ]; then
            python3 -c 'import os,sys; print(os.path.realpath(sys.argv[1]))' "$candidate"
            return 0
        fi
    done
    return 1
}

resolve_dep() {
    local dep="$1"
    local bin="$2"
    local name real
    case "$dep" in
        @rpath/*)
            name="${dep#@rpath/}"
            find_named_lib "$name" "$bin" || return 1
            ;;
        *)
            if [ -e "$dep" ]; then
                python3 -c 'import os,sys; print(os.path.realpath(sys.argv[1]))' "$dep"
            else
                name="$(basename "$dep")"
                find_named_lib "$name" "$bin" || return 1
            fi
            ;;
    esac
}

collect_deps() {
    otool -L "$1" 2>/dev/null | awk 'NR>1 { print $1 }'
}

bundle_one() {
    local bin="$1"
    local dep dest_name real
    while read -r dep; do
        [ -n "$dep" ] || continue
        is_system_lib "$dep" && continue
        dest_name="$(basename "${dep#@rpath/}")"
        if [ ! -f "$DEST/$dest_name" ]; then
            if ! real="$(resolve_dep "$dep" "$bin")"; then
                echo "[package-macos] aviso: não achei $dep (de $bin)" >&2
                continue
            fi
            cp "$real" "$DEST/$dest_name"
            chmod +w "$DEST/$dest_name"
            install_name_tool -id "@executable_path/$dest_name" "$DEST/$dest_name" 2>/dev/null || true
            bundle_one "$DEST/$dest_name"
        fi
        install_name_tool -change "$dep" "@executable_path/$dest_name" "$bin" 2>/dev/null || true
    done < <(collect_deps "$bin")
}

install_name_tool -id "@executable_path/game.dylib" "$DEST/game.dylib" 2>/dev/null || true
bundle_one "$DEST/FazendaDosSonhos"
bundle_one "$DEST/game.dylib"

shopt -s nullglob
for lib in "$DEST"/*.dylib; do
    bundle_one "$lib"
done
shopt -u nullglob

# Reescreve @rpath restantes se o arquivo já está na pasta.
rewrite_rpaths() {
    local bin="$1"
    local dep name
    while read -r dep; do
        case "$dep" in
            @rpath/*)
                name="${dep#@rpath/}"
                if [ -f "$DEST/$name" ]; then
                    install_name_tool -change "$dep" "@executable_path/$name" "$bin" 2>/dev/null || true
                fi
                ;;
        esac
    done < <(collect_deps "$bin")
}
for bin in "$DEST/FazendaDosSonhos" "$DEST"/*.dylib; do
    [ -f "$bin" ] || continue
    rewrite_rpaths "$bin"
done

cat > "$DEST/Jogar.command" <<'EOF'
#!/bin/bash
cd "$(dirname "$0")"
./FazendaDosSonhos
EOF
chmod +x "$DEST/Jogar.command" "$DEST/FazendaDosSonhos"

cat > "$DEST/LEIA-ME.txt" <<'EOF'
Fazenda dos Sonhos — macOS
==========================

1. Extraia esta pasta inteira.
2. Na primeira vez o macOS pode bloquear: clique com o botão direito
   em FazendaDosSonhos (ou Jogar.command) → Abrir → Abrir.
3. Save fica em ~/Library/Application Support/FazendaDosSonhos/FazendaDosSonhos/save.json

Não apague game.dylib nem a pasta assets — o jogo precisa dos dois.
EOF

echo "[package-macos] pronto em $DEST"
ls -lh "$DEST" | head -50
echo "--- deps ainda absolutas/@rpath? ---"
python3 - "$DEST" <<'PY'
import pathlib, subprocess, sys
dest = pathlib.Path(sys.argv[1])
bad = []
for p in [dest / "FazendaDosSonhos", *dest.glob("*.dylib")]:
    out = subprocess.check_output(["otool", "-L", str(p)], text=True)
    for line in out.splitlines()[1:]:
        dep = line.split()[0]
        if dep.startswith(("/usr/lib/", "/System/", "@executable_path/")):
            continue
        bad.append(f"{p.name}: {dep}")
if bad:
    print("\n".join(bad))
    sys.exit(1)
print("ok — só system + @executable_path")
PY
