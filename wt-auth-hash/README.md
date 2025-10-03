# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: Auth form with a placeholder hash check (teaching/demo only). Replace with Wt::Auth or a vetted crypto lib in production.


Build:
  cmake -S . -B build && cmake --build build -j
Run:
  ./build/wt_auth_hash --docroot . --http-address 0.0.0.0 --http-port 8100
