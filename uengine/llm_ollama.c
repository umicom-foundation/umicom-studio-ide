/*-----------------------------------------------------------------------------
 * Umicom AuthorEngine AI (uaengine)
 * File: src/llm_ollama.c
 * PURPOSE: Placeholder/stub translation unit for the Ollama backend.
 *
 * Why this file exists:
 *   - CMakeLists.txt enumerates this source file so the build system
 *     stays stable across platforms even when a backend’s implementation
 *     is not yet wired up.
 *   - We intentionally DO NOT define ueng_llm_open/ueng_llm_prompt/
 *     ueng_llm_close here to avoid duplicate symbol collisions with the
 *     current facade (implemented in other compilation units).
 *
 * How to implement the real backend later:
 *   1) Provide provider‑specific helpers (e.g. ueng_llm_open_<suffix>,
 *      ueng_llm_prompt_<suffix>, ueng_llm_close_<suffix>).
 *   2) Have the facade dispatch to one active backend based on
 *      build flags and/or environment (UENG_LLM_PROVIDER).
 *   3) Keep all public API symbols defined in exactly one place.
 *
 * Notes for contributors:
 *   - This file compiles cleanly and is intentionally minimal.
 *   - It keeps the project linkable while we stage the full backend.
 *   - Please keep Umicom credit header and add rich comments if you expand.
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 25-09-2025
 * License: MIT
 *---------------------------------------------------------------------------*/
#include "ueng/llm.h"
#include <stddef.h>

// No public API symbols defined here on purpose.
// See notes above. Real implementation will live behind provider-specific
// entry points to keep linkage unambiguous on all toolchains.
