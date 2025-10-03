// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: Minimal Wt application that shows a single text widget.


#include <Wt/WApplication.h>
#include <Wt/WText.h>
using namespace Wt;

// Define an application class that manages widgets and state for each user session.
class HelloApp : public WApplication {
public:
  HelloApp(const WEnvironment& env) : WApplication(env) {
    setTitle("Umicom Wt Hello");          // Set the browser window title
    root()->addNew<WText>("Hello from Wt!"); // Add a text widget to the root container
  }
};

// Factory function used by WRun to spawn the app for each incoming connection.
WApplication* createApp(const WEnvironment& env) {
  return new HelloApp(env);
}

// The Wt-provided WRun function starts an embedded HTTP(S) server and runs the app factory.
int main(int argc, char **argv) { return WRun(argc, argv, &createApp); }
