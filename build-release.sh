#!/bin/sh
# Undermountain Enhanced - build the release zip (Windows x64) on Linux.
#
# The zip contains no code of the game: DTU-Enhanced.exe translates the
# player's own DTUGAME.EXE at its first start (runtime/selfbuild.c), with the
# Windows builds of SR and nasm that come with it in the folder "enhanced".
#
# Needs: clang, clang++, lld, llvm-ar, llvm-strip, nasm, python3, perl,
# autoconf, make, unzip, patch, zip, and in $TOOLS:
#   mingw64.zip                      MSYS2 mingw64 tree (headers, CRT, libgcc, libstdc++, zlib)
#   SDL3-devel-3.4.20-mingw.tar.gz   SDL3 for mingw
#   SR-master.zip                    M-HT/SR static recompiler
#   nasm-2.16.03.tar.xz              nasm source (or NASMSRC = a folder with it)
# Usage: TOOLS=/path/to/tools sh build-release.sh
# Result: $PREFIX/release/Undermountain-Enhanced-<version>.zip
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
TOOLS=${TOOLS:?set TOOLS to the folder with the toolchain archives (see README.md)}
PREFIX=${PREFIX:-$HOME/dtu-build}
VERSION=$(sed -n 's/#define DTU_VERSION "\(.*\)"/\1/p' "$HERE/runtime/version.h")
mkdir -p "$PREFIX" && cd "$PREFIX"

[ -d mingw64 ] || { mkdir mingw64 && unzip -q "$TOOLS/mingw64.zip" -d mingw64; }
[ -d SDL3-3.4.20 ] || tar xzf "$TOOLS/SDL3-devel-3.4.20-mingw.tar.gz"
MW="$PREFIX/mingw64"
SDL3="$PREFIX/SDL3-3.4.20/x86_64-w64-mingw32"
GCCLIB=$(ls -d "$MW"/lib/gcc/x86_64-w64-mingw32/* | head -1)
GCCVER=$(basename "$GCCLIB")
cat > wcc.sh <<EOF
#!/bin/sh
exec clang -Wno-unused-command-line-argument --target=x86_64-w64-mingw32 -fuse-ld=lld -isystem $MW/include -L$MW/lib -L$GCCLIB -B$MW/lib --sysroot=$MW "\$@"
EOF
chmod +x wcc.sh
WCC="$PREFIX/wcc.sh"

# SR for Windows (with the DTU patch)
rm -rf sr-win && mkdir sr-win && (cd sr-win && unzip -q "$TOOLS/SR-master.zip" && mv SR-master/SR SR && rm -rf SR-master)
(cd sr-win/SR && patch -p0 < "$HERE/third_party/SR/sr-dtu.patch" >/dev/null && mkdir -p obj &&
 for f in *.c udis86-1.6/libudis86/*.c; do "$WCC" -O2 -w -Iudis86-1.6 -c "$f" -o obj/$(basename "$f" .c).o; done &&
 clang++ --target=x86_64-w64-mingw32 -nostdinc++ -isystem "$MW/include/c++/$GCCVER" -isystem "$MW/include/c++/$GCCVER/x86_64-w64-mingw32" \
   -isystem "$MW/include" --sysroot="$MW" -O2 -std=c++20 -w -Iudis86-1.6 -c container_helpers.cpp -o obj/container_helpers.o &&
 "$WCC" -static obj/*.o -o SR.exe -lstdc++ -lpthread && llvm-strip SR.exe)

# nasm for Windows
if [ -z "$NASMSRC" ]; then
  rm -rf nasm-src && mkdir nasm-src && tar xJf "$TOOLS/nasm-2.16.03.tar.xz" -C nasm-src --strip-components=1
  NASMSRC="$PREFIX/nasm-src"
fi
rm -rf nasm-win && cp -r "$NASMSRC" nasm-win
(cd nasm-win && { [ -x configure ] || sh autogen.sh >/dev/null; } &&
 ./configure --host=x86_64-w64-mingw32 CC="$WCC" AR=llvm-ar RANLIB=llvm-ranlib CFLAGS=-O2 >/dev/null &&
 make nasm.exe >/dev/null && llvm-strip nasm.exe)

# the program (no game code: SELFBUILD)
RECOMP="$HERE/recomp" OUT="$PREFIX/self" MW="$MW" SDL3="$SDL3" SELFBUILD=1 sh "$HERE/runtime/build-windows.sh"

# the zip
R="$PREFIX/release/Undermountain-Enhanced-$VERSION"
rm -rf "$R" && mkdir -p "$R/enhanced/recomp/x64" "$R/enhanced/licenses"
cp self/DTU-Enhanced.exe self/SDL3.dll "$R/"
cp "$HERE/release/README.txt" "$R/README.txt"
cp "$HERE/LICENSE" "$R/LICENSE.txt"
cp sr-win/SR/SR.exe nasm-win/nasm.exe "$R/enhanced/"
cp "$HERE"/recomp/SR.cfg "$HERE"/recomp/*.sci "$HERE"/recomp/data_in_code.txt "$R/enhanced/recomp/"
cp "$HERE"/recomp/x64/*.inc "$R/enhanced/recomp/x64/"
L="$R/enhanced/licenses"
cp "$HERE/release/SR-LICENSE.txt" "$L/SR.txt"
cp sr-win/SR/udis86-1.6/LICENSE "$L/udis86.txt"
cp nasm-win/LICENSE "$L/nasm.txt"
cp "$SDL3/share/licenses/SDL3/LICENSE.txt" "$L/SDL3.txt"
cp "$HERE/third_party/dosbox-staging-shaders/README.md" "$L/dosbox-staging-shaders.txt"
for d in crt libwinpthread libstdc++ zlib; do
  [ -d "$MW/share/licenses/$d" ] && for f in "$MW/share/licenses/$d"/*; do cp "$f" "$L/mingw-w64-$d-$(basename "$f" .txt).txt"; done
done
# text files with Windows line ends
for f in "$R"/*.txt "$L"/*.txt; do sed -i 's/\r*$/\r/' "$f"; done
(cd "$R" && rm -f "../Undermountain-Enhanced-$VERSION.zip" && zip -qr "../Undermountain-Enhanced-$VERSION.zip" .)   # files at the top: unpack into the game folder
echo "built $PREFIX/release/Undermountain-Enhanced-$VERSION.zip"
