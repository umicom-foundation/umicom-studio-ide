# Local LLMs (Ollama & llama.cpp)
/* Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT */

## Ollama
- Run server: `ollama serve`
- Set env:
  ```bash
  export LLM_PROVIDER=ollama
  export OLLAMA_MODEL=qwen2.5:latest
  export UMI_PRIVACY_MODE=local-only
  ```

## llama.cpp server
- Start server (OpenAI-compatible mode), often at `http://127.0.0.1:8080`.
- Set env:
  ```bash
  export LLM_PROVIDER=llama.cpp
  export LCPP_MODEL=llama-3.1-8b-instruct
  export UMI_PRIVACY_MODE=local-only
  ```
