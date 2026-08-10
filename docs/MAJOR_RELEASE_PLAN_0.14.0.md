# Umicom Studio IDE 0.14.0 — Major Native Platform Release Plan

## Purpose

Studio 0.14.0 is the reference integration release for Umicom Framework 0.5.0.
It proves that a product can consume Framework lifecycle, diagnostics, settings,
filesystem, process, security, capability, toolchain, repository, scaffolding,
and suite services without duplicating their implementations.

## Major feature train

1. Native Environment and Toolchain Manager.
2. Native build, test, repair, and prepared-shell workflows.
3. Framework Capability Catalogue and Studio platform report.
4. Local repository and optional GitHub repository creation.
5. Console, GTK4, and web application scaffolding.
6. Independent application and suite manifests.
7. Capability-based module permissions.
8. Framework ABI 2 module lifecycle and dependency validation.
9. Updated Studio Doctor and platform CLI validation.
10. Major-release documentation and acceptance evidence.

## Product boundary

Studio owns IDE composition, GTK4 windows, menus, editor workflows, panes,
branding, and product-specific behaviour.  Framework owns the reusable native
services and public contracts.

## Local-first release policy

The complete feature train is developed and validated locally.  Minor fixes do
not change versions and are not pushed individually.  Framework 0.5.0 and Studio
0.14.0 are pushed and tagged only after all acceptance gates pass.
