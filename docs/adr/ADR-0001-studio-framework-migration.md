# ADR-0001: Incrementally migrate Umicom Studio IDE to Umicom Framework

**Status:** Superseded for source layout by ADR-0002  
**Original decision:** Accepted  
**Studio baseline:** `studio-pre-framework-v0.9.0`

## Decision retained

The existing `umicom-studio-ide` repository remains the official Studio
repository. Useful GTK4 source and Git history are retained rather than
rewritten from zero.

Migration proceeds in bounded vertical slices. Each slice must:

1. define the Framework or Studio contract;
2. add automated tests;
3. adapt callers to the new contract;
4. build and launch Studio;
5. compare behaviour with the preserved baseline; and
6. remove the replaced duplicate implementation only after validation.

## Superseding layout decision

ADR-0002 promotes the working product source into the normal
`applications/studio/src` tree. The temporary compatibility layout and target
names are no longer active.
