# LLM Integration (Z.AI GLM‑4.6)

/* Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT */

This adds a small, vendor‑neutral LLM facade and a first provider: **Z.AI GLM‑4.6**.

## Why this design?
- Keep the GUI/editor decoupled from any single vendor.
- Provide a stable C API we can call from actions/commands.
- Start with **non‑streaming** requests, add SSE streaming later.

## Files
- `include/umicom/llm.h` — public API
- `include/umicom/llm_http.h` — tiny HTTP helper
- `src/llm/llm_core.c` — config + JSON build + call + parse
- `src/llm/llm_http.c` — libcurl POST
- `src/llm/providers/llm_zai.c` — Z.AI provider shim

## Build (CMake + Ninja)
We require **libcurl**, **GLib**, and **json‑glib**.

```bash
cmake --preset default
cmake --build --preset default -j
```

## Configure (environment)
```bash
export ZAI_API_KEY=sk-...
export ZAI_API_BASE=https://api.z.ai          # optional (default shown)
export ZAI_MODEL=glm-4.6                      # optional
export ZAI_TIMEOUTMS=45000                    # optional
```

## Usage (one‑shot chat)
In C code:

```c
#include "umicom/llm.h"

void demo() {
  UmiLlmCfg cfg; umi_llm_cfg_init_from_env(&cfg);
  gchar *out = NULL; gchar err[256] = {0};
  if (umi_llm_chat_simple(&cfg, "You are a helpful assistant.", "Hello!", &out, err, sizeof err)) {
    g_print("Assistant: %s\n", out);
    g_free(out);
  } else {
    g_printerr("LLM error: %s\n", err);
  }
}
```

## Security
- Prefer reading tokens from **environment variables** or the user’s private settings file (never commit tokens).
- Consider adding a redaction layer for logs.
