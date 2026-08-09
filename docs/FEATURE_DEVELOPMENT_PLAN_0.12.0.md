# Umicom Framework and Studio feature development plan — 0.12.0

## Purpose

This plan turns the clean Framework-based Studio foundation into a sequence of
small, independently testable capabilities. Each feature slice must leave both
Framework and Studio buildable and must preserve the GTK4 product application.

## Release 0.12.0 — retained diagnostics

Framework 0.4.3 introduces `UmiDiagnosticStore`, a bounded chronological ring
buffer that can be registered as a normal diagnostic sink. Studio 0.12.0 owns
one store in `UmiStudioServices` and exposes it through the native
`umicom-studio-diagnostics` command.

Delivered capabilities:

1. Bounded retained diagnostic records.
2. Owned source and message text.
3. Chronological reads.
4. Severity, received and overwrite counters.
5. Thread-safe sink writes using C atomic operations.
6. Studio service-container integration.
7. Native severity filtering and output limits.
8. Framework and Studio contract tests.
9. CI smoke testing of the diagnostics command.

## Planned sequence

### 0.13.0 — configuration and typed settings

- typed settings values and schemas;
- layered defaults, user, workspace and application settings;
- validation and diagnostics;
- migration of Studio JSON settings behind Framework contracts.

### 0.14.0 — filesystem and file watching

- path normalisation;
- directory enumeration;
- file-change events;
- Windows and POSIX watcher adapters;
- removal of duplicate Studio watcher helpers.

### 0.15.0 — process, build and task execution

- cancellable process contract;
- structured stdout, stderr and exit events;
- build-task scheduling;
- compiler diagnostic publication.

### 0.16.0 — workspace and document models

- Framework-owned workspace identity;
- document lifecycle and dirty state;
- recent workspace and session restoration;
- toolkit-neutral models with GTK4 adapters in Studio.

### 0.17.0 — problems, commands, queries and events

- diagnostic collection model;
- problem aggregation and filtering;
- Studio command schemas;
- application event contracts.

### 0.18.0 — Git provider gateway

- provider-neutral version-control contract;
- repository status, diff, branch and commit services;
- Studio Git presentation adapter.

### 0.19.0 — AI provider gateway

- provider-neutral chat, completion and streaming contracts;
- local and remote provider adapters;
- tool approval and privacy policy.

### 0.20.0 — plug-in lifecycle and permissions

- manifests;
- capability requests;
- discovery and activation;
- stable C ABI validation;
- isolation and audit evidence.

## Completion rule

A capability is considered migrated only when the Framework contract exists,
Framework tests pass, Studio uses the contract, the duplicate implementation is
removed, headless and Windows GTK builds pass, and the release is documented.
