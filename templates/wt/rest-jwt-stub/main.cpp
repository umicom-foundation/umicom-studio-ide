// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: JWT-like demo: base64url header.payload + 'signature' placeholder. Educates request handling; not secure.


#include <Wt/WApplication.h>
#include <Wt/WEnvironment.h>
#include <Wt/WServer.h>
#include <Wt/WResource.h>
#include <Wt/Http/Request.h>
#include <Wt/Http/Response.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <memory>
using namespace Wt;

static std::string b64url(const std::string& in){
  static const char* b64="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
  std::string out; int val=0, valb=-6;
  for(unsigned char c: in){ val=(val<<8)+c; valb+=8; while(valb>=0){ out.push_back(b64[(val>>valb)&0x3F]); valb-=6; } }
  return out; // (no padding for brevity)
}

class LoginResource : public WResource {
public:
  void handleRequest(const Http::Request& req, Http::Response& resp) override {
    const std::string user = req.getParameter("user") ? *req.getParameter("user") : "";
    const std::string pass = req.getParameter("pass") ? *req.getParameter("pass") : "";
    if(user.size() && pass.size()){
      std::string header = b64url("{\"alg\":\"HS256\",\"typ\":\"JWT\"}");
      std::string payload = b64url("{\"sub\":\""+user+"\",\"exp\":9999999999}");
      std::string token = header + "." + payload + "." + "signature"; // placeholder
      resp.setMimeType("application/json");
      resp.out() << "{ \"token\": \"" << token << "\" }";
    } else {
      resp.setStatus(400); resp.out() << "{ \"error\": \"missing creds\" }";
    }
  }
};

class PrivateResource : public WResource {
public:
  void handleRequest(const Http::Request& req, Http::Response& resp) override {
    auto it = req.getParameterMap().find("token");
    if(it == req.getParameterMap().end()) { resp.setStatus(401); resp.out() << "{ \"error\": \"no token\" }"; return; }
    resp.setMimeType("application/json"); resp.out() << "{ \"ok\": true }";
  }
};

int main(int argc, char **argv){
  try{
    WServer server(argv[0]);
    server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);
    auto login = std::make_shared<LoginResource>();
    auto priv  = std::make_shared<PrivateResource>();
    server.addResource(login, "/auth/login");
    server.addResource(priv,  "/api/private");
    if(server.start()){
      int sig = WServer::waitForShutdown(); server.stop(); return sig;
    }
  } catch (std::exception& e){ fprintf(stderr, "Error: %s\n", e.what()); }
  return 1;
}
