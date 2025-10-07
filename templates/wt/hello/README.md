# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: Smallest Wt app showing a greeting.


Build:
  cmake -S . -B build && cmake --build build -j
Run:
  ./build/wt_hello --docroot . --http-address 0.0.0.0 --http-port 8080
