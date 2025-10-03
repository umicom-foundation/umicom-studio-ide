<!-- Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT -->

# Getting Started

This guide helps you set up the development environment and run the IDE and example templates.

## Prerequisites

- Git, CMake **or** Meson, Ninja
- A C/C++ compiler (clang or gcc)
- Node.js (for web templates), Python 3 (for helper scripts)
- Optional: Android Studio, Java 17, Kotlin toolchain

## Clone

```bash
git clone https://github.com/umicom-foundation/umicom-studio-ide.git
cd umicom-studio-ide
```

## Build the IDE (CMake + Ninja)

```bash
cmake -S . -B build -G Ninja
cmake --build build -j
```

## Run a Template

See **Reference → Templates** for per-template instructions.
