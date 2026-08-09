# Build and release — Framework 0.4.3 and Studio 0.12.0

## Framework gate

1. Configure Framework from an empty build directory.
2. Build with strict warnings.
3. Run five Framework tests.
4. Commit Framework source to `main`.
5. Correct the `framework-v0.4.3` tag so it points to the new commit.

Expected Framework tests:

- `framework.foundation`
- `framework.scaffold`
- `framework.diagnostic_hub`
- `framework.clock`
- `framework.diagnostic_store`

## Studio gate

1. Record the new Framework submodule commit.
2. Regenerate `VERSION_LOCK.json`.
3. Configure and build headless from an empty directory.
4. Run ten tests.
5. Run Studio Console, Doctor and Diagnostics.
6. Build and launch GTK4 on Windows.
7. Commit Studio source to `main`.
8. Require both GitHub workflows to pass.
9. Create `studio-framework-v0.12.0`.

Expected Studio tests:

- `studio.framework_bootstrap`
- `studio.version`
- `studio.services`
- `studio.doctor`
- `studio.diagnostics`
