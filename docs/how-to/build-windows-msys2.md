# Build on Windows (MSYS2)

Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT

1. Install **MSYS2** from <https://www.msys2.org/> and open *MSYS2 UCRT64*.
2. Install dependencies:

```bash
pacman -S --noconfirm mingw-w64-ucrt-x86_64-gtk4 mingw-w64-ucrt-x86_64-json-glib ripgrep meson ninja
```

3. Configure and build:

```bash
meson setup build -Dbuildtype=debug || meson setup build --reconfigure
meson compile -C build
./build/umicom-studio
```

If GTK cannot load, ensure you are using the **UCRT64** environment and not MSYS.
