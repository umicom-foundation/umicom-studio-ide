---
title: Umicom Studio IDE — Developer Documentation
---

# Umicom Studio IDE — Developer Documentation

> Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT

Welcome to the official documentation for **Umicom Studio IDE**.

**Start here:**  
- 👉 **[Getting started](guide/getting-started.md)** — install dependencies and run the app.  
- 👉 **[Build on Windows (MSYS2)](how-to/build-windows-msys2.md)** / **[Linux & macOS](how-to/build-unix.md)**  
- 👉 **[C API Reference](reference/api/index.html)** (generated with Doxygen; see instructions below).

## Documentation model
We use a simple structure inspired by the Divio system:

- **Guide** — tutorials to get from zero to productive.
- **How‑to** — short, task‑oriented recipes.
- **Explanations** — architecture, design decisions, trade‑offs.
- **Reference** — authoritative details (CLI, config, C API).

## Generating the C API (Doxygen)
The **C API HTML** lives under `docs/reference/api/`. To regenerate it locally:

```bash
# from repo root
pip install mkdocs mkdocs-material
# install doxygen using your package manager
scripts/docs_build.sh --api
# then serve docs locally
mkdocs serve
```

> The website is published via GitHub Pages on every merge to `main`.
