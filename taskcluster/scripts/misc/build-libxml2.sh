#!/bin/bash
set -e -v -x

mkdir -p $UPLOAD_DIR

cd $MOZ_FETCHES_DIR/libxml2

export PATH="$MOZ_FETCHES_DIR/clang/bin:$PATH"

TARGET=${1?"First argument must be a valid value for CMAKE_C_COMPILER_TARGET"}

# Building the .rc file requires some extra work, but we don't
# actually need it, so just remove its creation.
sed -i /libxml2\\.rc/d CMakeLists.txt

case "$TARGET" in
*-pc-windows-msvc)
  EXTRA_CMAKE_FLAGS="
    -DCMAKE_C_COMPILER=clang-cl
    -DCMAKE_LINKER=lld-link
    -DCMAKE_MT=llvm-mt
    -DCMAKE_C_FLAGS=\"-fuse-ld=lld -Xclang -ivfsoverlay -Xclang $MOZ_FETCHES_DIR/vs/overlay.yaml -winsysroot $MOZ_FETCHES_DIR/vs\"
    -DCMAKE_EXE_LINKER_FLAGS=\"-winsysroot:$MOZ_FETCHES_DIR/vs\"
    -DCMAKE_SYSTEM_NAME=Windows
    -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded
  "
  ;;
*-unknown-linux-gnu)
  EXTRA_CMAKE_FLAGS="
    -DCMAKE_C_COMPILER=clang
    -DCMAKE_LINKER=clang
    -DCMAKE_C_FLAGS=\"-fuse-ld=lld --sysroot=$MOZ_FETCHES_DIR/sysroot\"
    -DCMAKE_EXE_LINKER_FLAGS=\"--sysroot=$MOZ_FETCHES_DIR/sysroot\"
  "
  ;;
esac

eval cmake \
  -GNinja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=Off \
  -DCMAKE_C_COMPILER_TARGET=$TARGET \
  $EXTRA_CMAKE_FLAGS \
  -DLIBXML2_WITH_C14N=OFF \
  -DLIBXML2_WITH_CATALOG=OFF \
  -DLIBXML2_WITH_DEBUG=OFF \
  -DLIBXML2_WITH_DOCB=OFF \
  -DLIBXML2_WITH_FTP=OFF \
  -DLIBXML2_WITH_HTML=OFF \
  -DLIBXML2_WITH_HTTP=OFF \
  -DLIBXML2_WITH_ICONV=OFF \
  -DLIBXML2_WITH_ICU=OFF \
  -DLIBXML2_WITH_ISO8859X=OFF \
  -DLIBXML2_WITH_LEGACY=OFF \
  -DLIBXML2_WITH_LZMA=OFF \
  -DLIBXML2_WITH_MEM_DEBUG=OFF \
  -DLIBXML2_WITH_MODULES=OFF \
  -DLIBXML2_WITH_PROGRAMS=OFF \
  -DLIBXML2_WITH_PUSH=OFF \
  -DLIBXML2_WITH_PYTHON=OFF \
  -DLIBXML2_WITH_READER=OFF \
  -DLIBXML2_WITH_RUN_DEBUG=OFF \
  -DLIBXML2_WITH_SCHEMATRON=OFF \
  -DLIBXML2_WITH_TESTS=OFF \
  -DLIBXML2_WITH_THREAD_ALLOC=OFF \
  -DLIBXML2_WITH_VALID=OFF \
  -DLIBXML2_WITH_WRITER=OFF \
  -DLIBXML2_WITH_XINCLUDE=OFF \
  -DLIBXML2_WITH_XPATH=OFF \
  -DLIBXML2_WITH_XPTR=OFF \
  -DLIBXML2_WITH_ZLIB=OFF \
  -DCMAKE_INSTALL_PREFIX=$PWD/libxml2 \
  -B build

ninja -C build -v install

tar -caf libxml2.tar.zst libxml2/
cp libxml2.tar.zst $UPLOAD_DIR/
