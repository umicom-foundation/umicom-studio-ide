# Token Inspector — Part 31 Phase 3
/* Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT */

**What’s new**
- Robust **OpenAI** SSE parsing for streaming tokens.
- Optional **top-k alternatives** via `LLM_LOGPROBS_TOPK` (default 0 = off).
- **Entropy** readout for each step (derived from top-k logprobs).

**Config**
```
export LLM_PROVIDER=openai    # or zai
export OPENAI_API_KEY=sk-...  # for OpenAI
export LLM_LOGPROBS_TOPK=5    # set >0 to enable alternatives in Inspector
```
