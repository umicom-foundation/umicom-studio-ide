# Build and run

## Headless Framework validation

```bash
cmake --preset headless-debug
cmake --build --preset headless-debug
ctest --preset headless-debug
```

## Windows GTK4 development

```powershell
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug
ctest --preset windows-ucrt64-debug
& ".\build\windows-ucrt64-debug\bin\umicom-studio-ide.exe" --console
```

CMake and Ninja are the only active build route for this repository.
