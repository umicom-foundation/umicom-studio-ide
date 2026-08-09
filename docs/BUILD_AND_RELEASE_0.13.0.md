# Build and release — Studio 0.13.0

## Windows PowerShell environment

```powershell
Set-Location "C:\Dev\umicom\umicom-studio"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
. ".\scripts\enter-umicom-ucrt64.ps1"
```

## Windows headless build

```powershell
Remove-Item -Recurse -Force ".\build\windows-ucrt64-headless-debug" -ErrorAction SilentlyContinue
cmake --preset windows-ucrt64-headless-debug
cmake --build --preset windows-ucrt64-headless-debug
ctest --preset windows-ucrt64-headless-debug
```

## Windows GTK4 build

```powershell
Remove-Item -Recurse -Force ".\build\windows-ucrt64-debug" -ErrorAction SilentlyContinue
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug
ctest --preset windows-ucrt64-debug
```

## Runtime checks

```powershell
& ".\build\windows-ucrt64-debug\bin\umicom-studio-doctor.exe" "."
& ".\build\windows-ucrt64-debug\bin\umicom-studio-settings.exe" --file ".\config\studio.settings" validate
& ".\build\windows-ucrt64-debug\bin\umicom-studio-ide.exe" --console
```
