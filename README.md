# Umicom Studio IDE

Umicom Studio IDE is the C23 and GTK4 development environment for building,
inspecting and operating applications based on Umicom Framework.

## Architecture

The repository builds Umicom Framework first and then composes Studio from
explicit application targets:

- `Umicom::Framework` — reusable runtime and platform capabilities.
- `Umicom::StudioCore` — Studio's Framework composition root.
- `Umicom::StudioProduct` — the active Studio product implementation.
- `umicom-studio-console` — headless lifecycle and diagnostic frontend.
- `umicom-studio-ide` — GTK4 desktop frontend.

## Windows development profile

The canonical Windows profile is MSYS2 UCRT64 with Clang, CMake and Ninja.

```powershell
Set-Location C:\Dev\umicom\umicom-studio-ide
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"

git submodule update --init --recursive

cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug
ctest --preset windows-ucrt64-debug

& ".\build\windows-ucrt64-debug\bin\umicom-studio-ide.exe" --console
```

## Headless validation

```powershell
cmake --preset headless-debug
cmake --build --preset headless-debug
ctest --preset headless-debug

& ".\build\headless-debug\bin\umicom-studio-console.exe"
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

## Licence

MIT. See `LICENSE` and the relevant third-party notices.
