# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: JWT-like stub using base64url (no real signature). For learning only; replace with a proper JWT lib in production.


Run:
  ./build/wt_rest_jwt --docroot . --http-address 0.0.0.0 --http-port 8101
Test:
  curl -X POST http://localhost:8101/auth/login -d user=demo -d pass=demo
  curl http://localhost:8101/api/private?token=RECEIVED_TOKEN
