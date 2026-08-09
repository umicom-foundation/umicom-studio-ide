# Umicom Studio IDE

Umicom Studio IDE 0.13.0 is the C23 and GTK4 development environment for
building, inspecting and operating applications based on Umicom Framework.

## Architecture

The repository builds Umicom Framework first and then composes Studio from
explicit application targets:

- `Umicom::Framework` — reusable runtime and platform capabilities.
- `Umicom::StudioCore` — Studio's Framework composition root and shared services.
- `Umicom::StudioProduct` — the active Studio product implementation.
- `umicom-studio-console` — headless lifecycle frontend.
- `umicom-studio-doctor` — native project-health validation.
- `umicom-studio-diagnostics` — retained diagnostic inspection.
- `umicom-studio-settings` — typed settings inspection and editing.
- `umicom-studio-ide` — GTK4 desktop frontend.

## Windows PowerShell start

Every new PowerShell process must use the MSYS2 UCRT64 toolchain before manual
build commands are entered:

```powershell
Set-Location "C:\Dev\umicom\umicom-studio"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
```

The helper below performs the same setup and checks the required tools:

```powershell
. ".\scripts\enter-umicom-ucrt64.ps1"
```

The Windows presets use absolute UCRT64 compiler and Ninja paths, which prevents
CMake from selecting standalone LLVM accidentally.

## Windows headless validation

```powershell
Remove-Item -Recurse -Force ".\build\windows-ucrt64-headless-debug" `
    -ErrorAction SilentlyContinue

cmake --preset windows-ucrt64-headless-debug
cmake --build --preset windows-ucrt64-headless-debug
ctest --preset windows-ucrt64-headless-debug
```

## Windows GTK4 build

```powershell
Remove-Item -Recurse -Force ".\build\windows-ucrt64-debug" `
    -ErrorAction SilentlyContinue

cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug
ctest --preset windows-ucrt64-debug

& ".\build\windows-ucrt64-debug\bin\umicom-studio-ide.exe" --console
```

## Typed settings feature slice

Framework 0.4.4 provides a schema-driven settings repository supporting string,
boolean, integer and real values, defaults, numeric ranges, reset, load and save.
Studio 0.13.0 defines its application schema in one place and exposes a native
command:

```powershell
& ".\build\windows-ucrt64-debug\bin\umicom-studio-settings.exe" `
    --file ".\config\studio.settings" list

& ".\build\windows-ucrt64-debug\bin\umicom-studio-settings.exe" `
    --file ".\config\studio.settings" set studio.editor.tab_width 8

& ".\build\windows-ucrt64-debug\bin\umicom-studio-settings.exe" `
    --file ".\config\studio.settings" validate
```

## Repository structure

```text
applications/studio/
├── include/umicom/studio/   Public Studio contracts
├── src/app/                 Framework composition root
├── src/console/             Console frontend
├── src/gtk/                 GTK4 executable entry point
├── src/build/               Build and task features
├── src/core/                Studio product services
├── src/editor/              Editor implementation
├── src/gui/                 GTK4 product presentation
├── src/llm/                 Studio AI integration
├── src/panes/               Output, Problems and Chat panes
├── src/plugins/             Studio plug-ins
├── src/search/              Search and ripgrep integration
├── src/ui/                  Product resources and branding
├── src/util/                Product utility implementations
└── tests/                   Framework integration tests

framework/                    Pinned Umicom Framework submodule
third_party/umicom/uengine/   Pinned AuthorEngine dependency
```

## Development direction

The current product source is active and must remain buildable.  Reusable
capabilities are migrated into Umicom Framework one tested vertical slice at a
time.  The next slice is diagnostics and logging.

## Author and organisation

- Author: Sammy Hegab
- Organisation: Umicom Foundation
- Licence: MIT
