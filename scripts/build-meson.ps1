
$ErrorActionPreference="Stop"
meson setup build
meson compile -C build
.\build\ustudio.exe
