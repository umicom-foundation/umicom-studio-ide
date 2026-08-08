# Quick instructions

The authoritative commands are in the root `README.md` and `CMakePresets.json`.
Do not use the archived Meson scripts.

Windows:

```powershell
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug
ctest --preset windows-ucrt64-debug
```
