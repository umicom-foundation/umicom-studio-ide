
# Umicom Studio IDE — Phase‑1 LLM Sidecar

Minimal local HTTP service the IDE can call for LLM chat.
- **Endpoint:** `POST /chat` — body: `{ "prompt": "text", "model": "optional-model" }`
- **Response:** `{ "ok": true, "reply": "model text" }`
- **Backends:** `openai` (remote), `ollama` (local)
- **Default port:** `8750`

## Quick Start (Windows PowerShell)

```powershell
Set-ExecutionPolicy -Scope Process Bypass -Force
# Choose backend:
$env:OPENAI_API_KEY = "sk-..."   # only if using OpenAI
.\scripts\ai-chat-server.ps1 -Backend openai -Port 8750
# or
.\scripts\ai-chat-server.ps1 -Backend ollama -Port 8750
```

Open: http://localhost:8750/ to test the web UI.
The Studio Chat Pane posts to: `http://127.0.0.1:8750/chat`

## Environment Variables
- `OPENAI_API_KEY` (required for backend `openai`)
- `OPENAI_MODEL` (default: `gpt-4o-mini`)
- `OLLAMA_HOST`  (default: `http://127.0.0.1:11434`)
- `OLLAMA_MODEL` (default: `llama3.1:8b`)

## Notes
- Uses .NET `HttpListener` (Windows friendly; no extra install).
- Scripts are MIT licensed. Ship with the repo under `scripts/sidecar/` if preferred.
