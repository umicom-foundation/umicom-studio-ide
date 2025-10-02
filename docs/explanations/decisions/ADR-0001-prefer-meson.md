# ADR‑0001: Prefer Meson as the primary build system

Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT

**Context.** We need fast, cross‑platform builds and simple CI integration.

**Decision.** Use **Meson** + **Ninja** as the primary build system. Provide CMake files later only if needed by downstreams.

**Consequences.** Cleaner CI, easier contributor onboarding, and consistent build flags.
