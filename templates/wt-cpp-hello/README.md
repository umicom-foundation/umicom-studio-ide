# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
cmake -S . -B build && cmake --build build -j
./build/wt_hello --docroot . --http-address 0.0.0.0 --http-port 8080
