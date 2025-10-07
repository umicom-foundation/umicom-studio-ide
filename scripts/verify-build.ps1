param([switch]$Clean)

if ($Clean) { Remove-Item -Recurse -Force .\build-ninja -ErrorAction SilentlyContinue }

$env:Path = "C:\msys64\mingw64\bin;$env:Path"
$env:PKG_CONFIG = "C:\msys64\mingw64\bin\pkg-config.exe"
$env:PKG_CONFIG_PATH = "C:\msys64\mingw64\lib\pkgconfig;C:\msys64\mingw64\share\pkgconfig"

cmake -S . -B build-ninja -G Ninja -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_C_COMPILER="C:/Program Files/LLVM/bin/clang.exe" `
  -DCMAKE_C_FLAGS="--target=x86_64-w64-mingw32 --sysroot=C:/msys64/mingw64 -D__USE_MINGW_ANSI_STDIO=1" `
  -DCMAKE_EXE_LINKER_FLAGS="--target=x86_64-w64-mingw32 -fuse-ld=lld -L C:/msys64/mingw64/lib"

cmake --build build-ninja -v
