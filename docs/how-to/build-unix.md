# Build on Linux/macOS

Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT

Install dependencies using your package manager:

```bash
# Debian/Ubuntu
sudo apt update && sudo apt install -y libgtk-4-dev libjson-glib-dev libglib2.0-dev ripgrep meson ninja-build

# Fedora
sudo dnf install -y gtk4-devel json-glib-devel glib2-devel ripgrep meson ninja-build

# macOS (Homebrew)
brew install gtk4 json-glib glib ripgrep meson ninja
```

Then build:

```bash
meson setup build -Dbuildtype=debug || meson setup build --reconfigure
meson compile -C build
./build/umicom-studio
```