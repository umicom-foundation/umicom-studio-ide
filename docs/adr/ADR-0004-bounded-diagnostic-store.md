# ADR-0004: Bounded retained diagnostics

**Status:** Accepted  
**Framework release:** 0.4.3  
**Studio release:** 0.12.0

## Context

The diagnostic hub can fan records to several sinks, but it does not retain a
history. Studio needs a toolkit-neutral record history for command-line
inspection, future GTK4 panes, automated tests and later persistence adapters.

## Decision

Umicom Framework owns an opaque `UmiDiagnosticStore` implemented as a bounded
ring buffer. The store:

- owns copies of source and message text;
- preserves chronological order;
- tracks retained severity counts;
- tracks lifetime received and overwritten counts;
- is safe for concurrent sink writes through C atomic locking;
- contains no GTK4 or Studio type.

Studio owns one 512-record store through `UmiStudioServices`. The store is the
first sink registered with the diagnostic hub so Framework lifecycle messages
are retained before presentation sinks receive them.

## Consequences

- Diagnostic history remains bounded and predictable.
- Long messages are deliberately truncated to the public record capacity.
- The first release retains records only in memory.
- File or database persistence can be added later behind a separate adapter.
- Existing product-specific logging remains scheduled for bounded migration.
