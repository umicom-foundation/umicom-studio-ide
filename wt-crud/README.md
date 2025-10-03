# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: CRUD demo with an in-memory vector of items.


Build:
  cmake -S . -B build && cmake --build build -j
Run:
  ./build/wt_crud --docroot . --http-address 0.0.0.0 --http-port 8081
