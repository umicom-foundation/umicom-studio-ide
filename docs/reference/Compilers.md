# Umicom Studio IDE — Compiler Support (Part 32)
# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT

This package adds a manifest (`tools/lang/compilers.v1.json`), generic tasks
(`tasks/auto_tasks.json`), and small **hello** templates for multiple languages.

- **Native/AOT**: GCC/Clang, Rust, Go, Swift, Zig, D(LDC), OCaml, GHC, C# (.NET)
- **C via C/C++**: Nim, V, Vala, ATS2, Gambit, CHICKEN

## Use
1. Unzip at the root of `umicom-studio-ide`.
2. Ensure compilers are on PATH (e.g., gcc, clang, rustc, go, swiftc, zig, ldc2, ocamlopt, ghc, dotnet, nim, v, valac, patscc, gsc, csc).
3. In the IDE, choose a template under **File → New Project**, then use **Build** / **Run**.
