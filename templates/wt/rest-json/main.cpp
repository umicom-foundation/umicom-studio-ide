// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: Use WServer and WResource to serve JSON at /api/hello.


#include <Wt/WApplication.h>
#include <Wt/WEnvironment.h>
#include <Wt/WResource.h>
#include <Wt/Http/Response.h>
#include <Wt/WServer.h>
#include <string>
using namespace Wt;

// A WResource that replies with a small JSON document.
class HelloApi : public WResource {
public:
  HelloApi(){ suggestFileName("hello.json"); }
  ~HelloApi(){ beingDeleted(); }
  void handleRequest(const Http::Request& req, Http::Response& resp) override {
    (void)req;
    resp.setMimeType("application/json");
    resp.out() << "{ \"ok\": true, \"message\": \"Hello from Umicom Wt REST API\" }";
  }
};

// Optional GUI app (not needed for pure API, but kept small).
class ApiApp : public WApplication {
public:
  ApiApp(const WEnvironment& env) : WApplication(env) { setTitle("Umicom Wt REST API"); }
};

WApplication* createApp(const WEnvironment& env){ return new ApiApp(env); }

int main(int argc, char **argv){
  try{
    WServer server(argv[0]);
    server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);

    // Mount the /api/hello endpoint using our resource.
    auto *hello = std::make_shared<HelloApi>();
    server.addResource(hello, "/api/hello");

    // Also serve the minimal GUI app at /
    server.addEntryPoint(EntryPointType::Application, &createApp);

    if(server.start()){
      int sig = WServer::waitForShutdown();
      server.stop();
      return sig;
    }
  } catch (std::exception& e){
    fprintf(stderr, "Error: %s\n", e.what());
  }
  return 1;
}
