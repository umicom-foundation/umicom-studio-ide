// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
#include <Wt/WApplication.h>
#include <Wt/WText.h>
using namespace Wt;
class HelloApp: public WApplication {
public:
  HelloApp(const WEnvironment& env): WApplication(env) {
    setTitle("Umicom Wt Hello");
    root()->addNew<WText>("Hello from Wt C++ in Umicom Studio IDE!");
  }
};
WApplication* createApp(const WEnvironment& env) { return new HelloApp(env); }
int main(int argc, char **argv) { return WRun(argc, argv, &createApp); }
