# ADR-0006: Native Framework platform and major release train

## Status

Accepted for local development of Studio 0.14.0.

## Decision

Studio consumes the native Framework toolchain, build, repository, scaffolding,
capability, policy, and suite services.  Studio does not create a second copy of
those mechanisms.  The product exposes a thin platform adapter and native
platform inspection command while GTK4 remains in the product presentation
layer.

Minor corrections are retained locally without a public version change.  A
coherent feature train is validated before one coordinated Framework and Studio
release.

## Consequences

- Studio can create Umicom Designer, Trader, TMS, and other repositories using
  the same native Framework service.
- PowerShell execution policy is no longer part of the normal workflow after
  the native `umicom` command is bootstrapped.
- Framework remains independently testable and installable.
- Application suites can share a Framework without giving up independent
  executables and repositories.
