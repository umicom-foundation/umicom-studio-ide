# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: Expose a JSON endpoint using Wt’s HTTP primitives.


Run server:
  ./build/wt_rest --docroot . --http-address 0.0.0.0 --http-port 8084
Test:
  curl http://localhost:8084/api/hello
