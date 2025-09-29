# Umicom Studio IDE — Codestral FIM Backend (Skeleton)

This is a **minimal** backend demonstrating Codestral Fill‑In‑the‑Middle (FIM) calls.
It’s suitable for wiring into your GTK4 front-end later.

## Build

Requires libcurl and CMake:

```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j
```

## Run

```bash
./studio_fim_demo "<<prefix here>>" "<<suffix here>>"
```

It will print the completion JSON (or you can parse it and print the token text only).

## Next Steps

- Integrate with your GTK4 app: capture editor buffer as (prefix, suffix).
- Add language picker and auto-detect based on file extension.
- Implement streaming and JSON parsing library for resilience.
