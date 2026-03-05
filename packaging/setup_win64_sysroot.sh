#!/bin/bash
# Sets up the win64 cross-compilation sysroot for SoulFu.
# Downloads SDL2/SDL2_net mingw dev packages from GitHub,
# and cross-compiles libogg, libvorbis, libjpeg-turbo from source.
#
# Run from project root: ./packaging/setup_win64_sysroot.sh

set -e

SYSROOT="/tmp/mingw64-sysroot"
BUILDDIR="$(mktemp -d)"
HOST="x86_64-w64-mingw32"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
DLL_DIR="$SCRIPT_DIR/win64"

SDL2_VERSION="2.30.12"
SDL2_NET_VERSION="2.2.0"
LIBOGG_VERSION="1.3.6"
LIBVORBIS_VERSION="1.3.7"
LIBJPEG_VERSION="3.1.0"

cleanup() { rm -rf "$BUILDDIR"; }
trap cleanup EXIT

echo "=== SoulFu win64 sysroot setup ==="
echo "Sysroot: $SYSROOT"
echo "Build dir: $BUILDDIR"
mkdir -p "$SYSROOT/include" "$SYSROOT/lib" "$DLL_DIR"

# ---------------------------------------------------------------------------
echo ""
echo "[1/5] SDL2 $SDL2_VERSION (mingw dev package from GitHub)..."
SDL2_URL="https://github.com/libsdl-org/SDL/releases/download/release-$SDL2_VERSION/SDL2-devel-$SDL2_VERSION-mingw.tar.gz"
wget -q --show-progress -O "$BUILDDIR/sdl2.tar.gz" "$SDL2_URL"
tar -xzf "$BUILDDIR/sdl2.tar.gz" -C "$BUILDDIR"
SDL2_DIR="$BUILDDIR/SDL2-$SDL2_VERSION/$HOST"
cp -r "$SDL2_DIR/include/SDL2" "$SYSROOT/include/"
cp "$SDL2_DIR/lib/libSDL2.dll.a" "$SYSROOT/lib/"
cp "$SDL2_DIR/lib/libSDL2main.a" "$SYSROOT/lib/"
cp "$SDL2_DIR/bin/SDL2.dll" "$DLL_DIR/"
echo "  done."

# ---------------------------------------------------------------------------
echo ""
echo "[2/5] SDL2_net $SDL2_NET_VERSION (mingw dev package from GitHub)..."
SDL2NET_URL="https://github.com/libsdl-org/SDL_net/releases/download/release-$SDL2_NET_VERSION/SDL2_net-devel-$SDL2_NET_VERSION-mingw.tar.gz"
wget -q --show-progress -O "$BUILDDIR/sdl2_net.tar.gz" "$SDL2NET_URL"
tar -xzf "$BUILDDIR/sdl2_net.tar.gz" -C "$BUILDDIR"
SDL2NET_DIR="$BUILDDIR/SDL2_net-$SDL2_NET_VERSION/$HOST"
cp "$SDL2NET_DIR/include/SDL2/SDL_net.h" "$SYSROOT/include/SDL2/"
cp "$SDL2NET_DIR/lib/libSDL2_net.dll.a" "$SYSROOT/lib/"
cp "$SDL2NET_DIR/bin/SDL2_net.dll" "$DLL_DIR/"
echo "  done."

# ---------------------------------------------------------------------------
echo ""
echo "[3/5] libogg $LIBOGG_VERSION (cross-compile from source)..."
OGG_URL="https://downloads.xiph.org/releases/ogg/libogg-$LIBOGG_VERSION.tar.xz"
wget -q --show-progress -O "$BUILDDIR/libogg.tar.xz" "$OGG_URL"
tar -xf "$BUILDDIR/libogg.tar.xz" -C "$BUILDDIR"
(
    cd "$BUILDDIR/libogg-$LIBOGG_VERSION"
    ./configure --host=$HOST --prefix="$SYSROOT" --disable-shared --enable-static --quiet
    make -j"$(nproc)" --quiet
    make install --quiet
)
cp "$BUILDDIR/libogg-$LIBOGG_VERSION/src/.libs/libogg-0.dll" "$DLL_DIR/" 2>/dev/null || true
echo "  done."

# ---------------------------------------------------------------------------
echo ""
echo "[4/5] libvorbis $LIBVORBIS_VERSION (cross-compile from source)..."
VORBIS_URL="https://downloads.xiph.org/releases/vorbis/libvorbis-$LIBVORBIS_VERSION.tar.xz"
wget -q --show-progress -O "$BUILDDIR/libvorbis.tar.xz" "$VORBIS_URL"
tar -xf "$BUILDDIR/libvorbis.tar.xz" -C "$BUILDDIR"
(
    cd "$BUILDDIR/libvorbis-$LIBVORBIS_VERSION"
    CFLAGS="-I$SYSROOT/include" LDFLAGS="-L$SYSROOT/lib" \
    ./configure --host=$HOST --prefix="$SYSROOT" --disable-shared --enable-static \
        --with-ogg="$SYSROOT" --quiet
    make -j"$(nproc)" --quiet
    make install --quiet
)
cp "$BUILDDIR/libvorbis-$LIBVORBIS_VERSION/lib/.libs/libvorbis-0.dll" "$DLL_DIR/" 2>/dev/null || true
echo "  done."

# ---------------------------------------------------------------------------
echo ""
echo "[5/5] libjpeg-turbo $LIBJPEG_VERSION (cross-compile from source)..."
JPEG_URL="https://github.com/libjpeg-turbo/libjpeg-turbo/releases/download/$LIBJPEG_VERSION/libjpeg-turbo-$LIBJPEG_VERSION.tar.gz"
wget -q --show-progress -O "$BUILDDIR/libjpeg-turbo.tar.gz" "$JPEG_URL"
tar -xzf "$BUILDDIR/libjpeg-turbo.tar.gz" -C "$BUILDDIR"
(
    cd "$BUILDDIR"
    mkdir jpeg-build && cd jpeg-build
    cmake "../libjpeg-turbo-$LIBJPEG_VERSION" \
        -DCMAKE_SYSTEM_NAME=Windows \
        -DCMAKE_SYSTEM_PROCESSOR=AMD64 \
        -DCMAKE_C_COMPILER=$HOST-gcc \
        -DCMAKE_RC_COMPILER=$HOST-windres \
        -DCMAKE_INSTALL_PREFIX="$SYSROOT" \
        -DENABLE_SHARED=OFF \
        -DENABLE_STATIC=ON \
        -DWITH_TURBOJPEG=OFF \
        -DWITH_SIMD=OFF \
        -DCMAKE_INSTALL_LIBDIR=lib \
        > /dev/null
    make -j"$(nproc)" --quiet
    make install --quiet
)
echo "  done."

# ---------------------------------------------------------------------------
echo ""
echo "=== Sysroot setup complete! ==="
echo ""
echo "Sysroot libs:"
ls "$SYSROOT/lib/"*.a 2>/dev/null
echo ""
echo "DLLs in $DLL_DIR/:"
ls "$DLL_DIR/"
echo ""
echo "Build with:  make -f Makefile.mingw64"
