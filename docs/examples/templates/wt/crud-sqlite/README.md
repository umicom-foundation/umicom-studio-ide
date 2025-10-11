# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: CRUD demo backed by SQLite using Wt::Dbo. Creates a local db file on first run.


Build:
  cmake -S . -B build && cmake --build build -j
Run:
  ./build/wt_crud_sqlite --docroot . --http-address 0.0.0.0 --http-port 8090
