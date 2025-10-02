# Providers (matrix)
/* Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT */

| Provider      | Streaming | Logprobs | Privacy mode compatible |
|---------------|-----------|----------|--------------------------|
| Z.AI (remote) | Yes       | TBD      | **Blocked** in local-only |
| OpenAI        | Yes       | Yes      | **Blocked** in local-only |
| Ollama (local)| Yes*      | Engine-dependent | **Allowed** |
| llama.cpp     | Yes*      | Via logits      | **Allowed** |

\* Local servers often emit **OpenAI-compatible** streaming frames (`data: {...}`); we parse those.
