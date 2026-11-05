#!/usr/bin/env bash
# Biên dịch libretro-mgba bằng Emscripten (chạy trong GitHub Actions).
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

echo "==> Biên dịch bằng Emscripten"
cd "$WORK/mgba"
emmake make -f Makefile.libretro platform=emscripten -j"$(nproc)"

echo "==> Tìm file kết quả"
shopt -s nullglob
artifacts=( ./mgba_libretro*.bc ./mgba_libretro*.a ./mgba_libretro*.wasm ./mgba_libretro*.js ./mgba_libretro*.so )
if [ ${#artifacts[@]} -eq 0 ]; then
  echo "Không thấy file mgba_libretro* sau khi build. Nội dung thư mục:"
  ls -la
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
