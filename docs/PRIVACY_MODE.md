
# Privacy Mode
/* Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT */

Set `UMI_PRIVACY_MODE=local-only` to block any network requests to non-local hosts.
- **Allowed**: `http://localhost:<port>/...`, `http://127.x.y.z:<port>/...`
- **Blocked**: any other host (e.g., `api.openai.com`, `api.z.ai`, public IPs)

This is enforced in the HTTP layer, so all providers respect it.

## Local providers
- **Ollama** (default base `http://127.0.0.1:11434`)
  - Env: `OLLAMA_API_BASE`, `OLLAMA_API_PATH` (default `/v1/chat/completions`)
- **llama.cpp** (server, default base `http://127.0.0.1:8080`)
  - Env: `LCPP_API_BASE`, `LCPP_API_PATH` (default `/v1/chat/completions`)

Set `LLM_PROVIDER=ollama` or `LLM_PROVIDER=llama.cpp` with `UMI_PRIVACY_MODE=local-only` to ensure offline operation.
