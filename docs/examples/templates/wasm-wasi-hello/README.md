# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
clang --target=wasm32-wasi -O2 hello.c -o hello.wasm
wasmtime hello.wasm
