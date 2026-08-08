# ADR-0003: Extract reusable Studio services through Framework contracts

**Status:** Accepted

Studio remains a working product while reusable capabilities move into Umicom
Framework one bounded vertical slice at a time.  A source file is deleted only
after the replacement contract, tests, adapter and visible application behavior
have been verified.  GTK-specific presentation remains in Studio.

The first shared service foundation is the Framework diagnostic hub and the
Studio service container.  The next extraction is diagnostics and logging.
