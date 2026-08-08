# Build on Windows with MSYS2 UCRT64

Umicom Studio IDE uses CMake and Ninja. Meson is no longer an active build
system.

Open PowerShell in the repository root:

```powershell
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
git submodule update --init --recursive
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug
ctest --preset windows-ucrt64-debug
& ".\build\windows-ucrt64-debug\bin\umicom-studio-ide.exe" --console
```
