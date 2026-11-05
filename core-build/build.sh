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

{
  echo "core: libretro/mgba"
  echo "ref: $MGBA_REF"
  echo "commit: $COMMIT"
  echo "built: $(date -u +%Y-%m-%dT%H:%M:%SZ)"
  echo "emcc: $(emcc --version | head -n 1)"
} > "$OUT/BUILD_INFO.txt"

echo "==> Xong"
ls -la "$OUT"
