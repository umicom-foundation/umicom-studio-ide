# Build on Linux or macOS

Install Clang, CMake, Ninja, pkg-config, GTK4, GtkSourceView 5, JSON-GLib,
libsoup 3, libcurl and SQLite development packages.

```bash
git submodule update --init --recursive
cmake -S . -B build/linux-gtk -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DBUILD_TESTING=ON \
  -DUMICOM_STUDIO_BUILD_GTK=ON
cmake --build build/linux-gtk
ctest --test-dir build/linux-gtk --output-on-failure
./build/linux-gtk/bin/umicom-studio-ide --console
```
