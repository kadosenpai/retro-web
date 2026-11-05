#!/usr/bin/env bash
# Biên dịch libretro-mgba bằng Emscripten + CMake (chạy trong GitHub Actions).
set -euo pipefail

MGBA_REPO="https://github.com/libretro/mgba.git"
MGBA_REF="${MGBA_REF:-master}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORK="$ROOT/work"
OUT="$ROOT/out"

rm -rf "$WORK" "$OUT"
mkdir -p "$WORK" "$OUT"

echo "==> Tải mGBA ($MGBA_REF)"
git clone "$MGBA_REPO" "$WORK/mgba"
git -C "$WORK/mgba" checkout "$MGBA_REF"
git -C "$WORK/mgba" submodule update --init --recursive
COMMIT="$(git -C "$WORK/mgba" rev-parse HEAD)"

echo "==> Cấu hình CMake (Emscripten, libretro tĩnh)"
emcmake cmake -S "$WORK/mgba" -B "$WORK/build" -G Ninja \
  -DLIBMGBA_ONLY=ON \
  -DBUILD_LIBRETRO=ON \
  -DLIBRETRO_STATIC=ON \
  -DCMAKE_BUILD_TYPE=Release

echo "==> Biên dịch"
cmake --build "$WORK/build" --target mgba_libretro -j"$(nproc)"

echo "==> Tìm file kết quả"
shopt -s nullglob
artifacts=( "$WORK"/build/mgba_libretro*.bc "$WORK"/build/mgba_libretro*.a )
if [ ${#artifacts[@]} -eq 0 ]; then
  echo "Không thấy mgba_libretro.bc hoặc .a. Nội dung thư mục build:"
  ls -la "$WORK/build"
  exit 1
fi
cp -v "${artifacts[@]}" "$OUT/"
CORE_LIB="$WORK/libmgba_core.a"
cp "${artifacts[0]}" "$CORE_LIB"

echo "==> Ghép shim + nhân thành Wasm"
emcc "$ROOT/frontend/libretro_shim.c" "$CORE_LIB" \
  -I "$WORK/mgba/src/platform/libretro" \
  -O3 \
  --no-entry \
  -sMODULARIZE=1 \
  -sEXPORT_ES6=1 \
  -sEXPORT_NAME=createCore \
  -sENVIRONMENT=web,worker \
  -sALLOW_MEMORY_GROWTH=1 \
  -sINITIAL_MEMORY=33554432 \
  -sSTACK_SIZE=1048576 \
  -sFORCE_FILESYSTEM=1 \
  -sEXPORTED_FUNCTIONS=_malloc,_free \
  -sEXPORTED_RUNTIME_METHODS=HEAPU8,HEAP16,HEAPU32,FS \
  -o "$OUT/mgba_libretro.js"
{
  echo "core: libretro/mgba"
  echo "ref: $MGBA_REF"
  echo "commit: $COMMIT"
  echo "built: $(date -u +%Y-%m-%dT%H:%M:%SZ)"
  echo "emcc: $(emcc --version | head -n 1)"
} > "$OUT/BUILD_INFO.txt"

echo "==> Xong"
ls -la "$OUT"
