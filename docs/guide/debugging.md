# Debugging tips

Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT

- Use `UMICOM_LOG` levels via the `--log` CLI flag (0=DEBUG..3=ERROR).
- Launch with `--headless` for CLI‑only smoke tests.
- On Windows/MSYS2, verify PATH hints (see `msys_env.c`).