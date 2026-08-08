# ADR-0001: Incrementally migrate Umicom Studio IDE to Umicom Framework

**Status:** Accepted  
**Migration stage:** Framework bootstrap  
**Studio baseline:** `studio-pre-framework-v0.9.0`

## Decision

The existing `umicom-studio-ide` repository remains the official Studio repository.  The useful GTK4 source and Git history are retained.

The old monolithic internal build architecture is deprecated, but the Studio product is not rewritten from zero.  Existing source moves beneath `applications/studio/src/legacy`, while new Framework-based composition code is introduced beside it.

During the first bootstrap stage, root-level assets, configuration, Windows resources, scripts and legacy tests remain where they are.  They move only when the corresponding Framework service has been introduced and verified.

Each later vertical slice must:

1. define a Framework contract;
2. add Framework tests;
3. adapt Studio to the contract;
4. build and launch Studio;
5. compare behaviour with the baseline; and
6. remove the duplicate legacy implementation only after the replacement passes.

## Next slice

Diagnostics and logging.
