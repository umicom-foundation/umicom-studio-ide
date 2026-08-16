# Architecture Note — Workspace Search and Replace

Author: Sammy Hegab  
Organisation: Umicom Foundation  
Batch: 93

## Decision

Workspace search and multi-file replacement are reusable Umicom Framework capabilities. Umicom Studio is a consuming application and placement host. It must not own a private regex engine, private exclusion rules, private replacement expansion or a second edit-transaction implementation.

This decision follows the Framework-first rule established for the Umicom product family. Search is useful to Studio, AuthorEngine, code analysis, migration tools, future TMS configuration workbenches and other applications. Its contracts therefore belong in the shared editor platform.

## Relationship to existing capabilities

Batch 93 deliberately composes rather than duplicates:

- `UmiEditorSourceLocation` for navigation-compatible locations.
- `UmiEditorTextBuffer` for toolkit-neutral working copies.
- `UmiEditorWorkspaceEditSet` for canonical expected/replacement edits.
- `UmiEditorEditTransaction` for atomic preflight, commit and rollback.
- Studio command/view contributions for product placement only.

The new platform does not replace the older platform file-index/search helper immediately. That older helper remains a lightweight platform utility. Batch 93 introduces the richer editor-domain contract needed for regex captures, selected previews and atomic replacement. Later consolidation can adapt the lightweight helper into the new index without breaking callers.

## Component flow

```text
Workspace providers
    |
    | copied document snapshots
    v
WorkspaceSearchIndex
    |
    | resource metadata
    +----> WorkspaceSearchExclusionSet
    |
    | included documents
    v
WorkspaceSearchPattern ----> WorkspaceSearchQuery
                                  |
                                  | stable matches, captures, revisions
                                  v
                         WorkspaceReplacementPreview
                                  |
                                  | selected applicable changes
                                  v
                         WorkspaceReplacePlan
                                  |
                                  | canonical WorkspaceEditSet
                                  v
                         WorkspaceReplaceTransaction
                                  |
                                  | preflight / commit / rollback
                                  v
                         Framework Text Buffers
```

## Authoritative ownership

| State | Authoritative owner |
|---|---|
| Indexed document snapshot | `UmiEditorWorkspaceSearchIndex` |
| Search inclusion decision | `UmiEditorWorkspaceSearchExclusionSet` |
| Compiled expression | `UmiEditorWorkspaceSearchPattern` |
| Result selection | `UmiEditorWorkspaceSearchQuery` |
| Replacement selection and expansion | `UmiEditorWorkspaceReplacementPreview` |
| Canonical edit set | `UmiEditorWorkspaceReplacePlan` |
| Atomic mutation lifecycle | `UmiEditorWorkspaceReplaceTransaction` / existing edit transaction |
| Studio menu and view location | Studio contribution |

The Master Controller may compose this capability and a relevant Slave Controller may coordinate its use case, but neither bypasses these state owners.

## Stable boundary rules

1. Public structures start with `struct_size` and `api_version`.
2. Cross-module values use fixed-capacity C strings or explicit pointer lifetime rules.
3. No GTK, Qt, Wt, GLib, PCRE or platform file-system type appears in a public contract.
4. Index inputs are copied.
5. Index document-view content is explicitly borrowed until the next index mutation.
6. Query results copy bounded matched text and preserve source fingerprint/revision evidence.
7. Replacement preview never mutates a source document.
8. Replace planning never writes a source document.
9. Only transaction commit changes bound text buffers.
10. A conflict or revision mismatch fails closed.

## Regex execution model

The initial engine compiles expressions to a Thompson-style instruction program. Execution advances instruction states rather than recursively exploring a backtracking tree. This gives predictable control over:

- Maximum program size.
- Maximum steps per find operation.
- Stack usage.
- Cross-platform behaviour.
- Failure diagnostics.

Instruction and thread order is semantic. Alternation is leftmost-first, greedy quantifiers prefer the consuming path and lazy quantifiers prefer the continuation path. A lower-priority match is retained as a candidate only while higher-priority paths remain viable, which provides familiar editor regular-expression behaviour without recursive backtracking.

The feature set covers common professional workspace searches while intentionally excluding constructs that would require backtracking or more complex capture semantics in this first release.

## Search bounds

Three independent boundaries protect the host:

- Index limits bound documents, bytes per document and total copied bytes.
- Pattern limits bound compiled instructions and execution steps.
- Query limits bound total results and results per document.

A cancellation callback lets a task service or UI cancel between documents and matches. The API reports whether a result set is complete, truncated or cancelled.

## Exclusion precedence

Exclusion rules are resolved by:

1. Enabled matching rules only.
2. Highest numeric priority.
3. Most recently upserted rule when priorities tie.
4. Built-in hidden/generated/binary policy at a low implicit priority.

This model allows a deliberate include rule to override a broad generated or build-directory exclusion without introducing order-sensitive undocumented behaviour.

## Replacement syntax

Replacement expansion is deliberately compact and auditable:

- `$0` or `$&`: complete match.
- `$1`–`$8`: capture groups.
- `$$`: literal dollar.
- Optional escaped newline, carriage return, tab and backslash.
- Optional ASCII preserve-case transformation.

The preview records the expanded replacement, original context, resulting context and a typed applicability state.

## Conflict model

A preview is non-applicable when:

- The document cannot be found.
- The current fingerprint differs from the search result.
- A non-zero document revision differs.
- The document is read-only.
- Matched text was truncated for display/storage safety.
- Expanded replacement exceeds the canonical edit capacity.
- The template cannot be expanded safely.

A plan revalidates the same evidence before emitting edits. Finalisation then detects overlapping edit ranges. Transaction preflight validates actual working buffers again. This repeated validation is intentional: search, preview, planning and commit may be separated by user think time or asynchronous activity.

## Atomicity

Batch 93 does not invent another transaction engine. It builds a canonical `UmiEditorWorkspaceEditSet` and delegates atomicity to the existing `UmiEditorEditTransaction`:

- Capture original documents.
- Validate every expected range.
- Apply each document's edits from highest offset to lowest offset.
- Roll back previously changed documents if a later application fails.
- Record applied and rollback counts.

This creates one consistent mutation boundary for refactoring, code actions and Replace All.

## Studio boundary

Studio's contribution contains static placement metadata only. It does not:

- Compile patterns.
- Search documents.
- Evaluate exclusions.
- Expand replacements.
- Create edits.
- Apply or roll back documents.

This keeps Studio replaceable as a frontend and permits future GTK4, Qt6, Wt, native web and headless presentations over the same Framework state.

## Future extension points

Later batches may add providers without changing this ownership model:

- Incremental file-system indexing.
- Remote-workspace streaming indexes.
- Source-control ignore adapters.
- Unicode-aware case folding and word boundaries.
- Persisted result history.
- Search-result export formats.
- Structural or syntax-tree search providers.
- AI-proposed search and replacement templates under approval policy.
- Distributed indexing workers.

Each extension must preserve the stable C boundary and transactional replacement path.
