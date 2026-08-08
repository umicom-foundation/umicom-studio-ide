# Build and release 0.11.1

## Headless

```powershell
cmake --preset headless-debug
cmake --build --preset headless-debug
ctest --preset headless-debug
& ".\build\headless-debug\bin\umicom-studio-doctor.exe" "."
```

## Windows GTK4

```powershell
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug
ctest --preset windows-ucrt64-debug
& ".\build\windows-ucrt64-debug\bin\umicom-studio-doctor.exe" "."
& ".\build\windows-ucrt64-debug\bin\umicom-studio-ide.exe" --console
```

## Record dependency commits before a release

```powershell
& ".\scripts\update-version-lock.ps1" -RepositoryRoot "."
```
