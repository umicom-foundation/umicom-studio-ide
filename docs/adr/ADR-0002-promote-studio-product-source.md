# ADR-0002: Promote the existing Studio source into the active product tree

**Status:** Accepted  
**Date:** 8 August 2026

## Context

The Framework bootstrap release placed the complete existing Studio
implementation below `applications/studio/src/legacy` and compiled it through a
compatibility target.  That was useful as a reversible first bridge, but the
source is not disposable: it contains the current GTK4 application, editor,
build system, panes, search, settings, AI integration and platform utilities.

Deleting that implementation would delete the usable IDE.  Leaving it behind a
permanent compatibility label would also misrepresent its role.

## Decision

1. Promote the existing implementation into `applications/studio/src`.
2. Rename the compatibility target to `Umicom::StudioProduct`.
3. Rename `LegacySources.cmake` to `StudioSources.cmake`.
4. Keep explicit source lists and direct dependency declarations.
5. Continue extracting reusable capabilities into Umicom Framework in tested
   vertical slices.
6. Do not equate removal of the word “legacy” with completion of the Framework
   migration.

## Consequences

- Studio no longer depends on a target or path named legacy.
- Existing functionality remains available.
- Current coupling and duplicated utilities still require later extraction.
- Diagnostics and logging remain the next vertical migration slice.
