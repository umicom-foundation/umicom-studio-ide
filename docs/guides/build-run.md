<!-- Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT -->

# Build & Run

## CMake (default)

```bash
cmake -S . -B build -G Ninja
cmake --build build -j
./build/umicom_ide   # if your target is named this way
```

## Meson (optional)

```bash
meson setup build
meson compile -C build
```

## Code Style

- `clang-format` is enforced; see **Reference → Style Guide**.
- `.editorconfig` is present for cross-editor consistency.
