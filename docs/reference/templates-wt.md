<!-- Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT -->

# Wt C++ Templates

- Hello: `templates/wt/hello`
- CRUD (in-memory): `templates/wt/crud`
- CRUD + SQLite (Wt::Dbo): `templates/wt/crud-sqlite`
- Routing: `templates/wt/routing`
- Auth (stub): `templates/wt/auth-stub`
- REST JSON: `templates/wt/rest-json`
- File Upload: `templates/wt/file-upload`
- Charts: `templates/wt/charts`

**Build (example):**
```bash
cmake -S . -B build && cmake --build build -j
```
Run with `--http-port` and `--docroot` as needed.
