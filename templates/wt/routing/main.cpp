// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: Switch views based on internal path, e.g. /about.


#include <Wt/WApplication.h>
#include <Wt/WAnchor.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
using namespace Wt;

class RoutingApp : public WApplication {
  WContainerWidget *content_{nullptr};
public:
  RoutingApp(const WEnvironment& env) : WApplication(env) {
    setTitle("Umicom Wt Routing");
    auto nav = root()->addNew<WContainerWidget>();
    nav->addNew<WAnchor>(WLink(InternalPath, "/"), "Home");
    nav->addNew<WText>(" | ");
    nav->addNew<WAnchor>(WLink(InternalPath, "/about"), "About");
    nav->addNew<WText>(" | ");
    nav->addNew<WAnchor>(WLink(InternalPath, "/help"), "Help");

    content_ = root()->addNew<WContainerWidget>();
    internalPathChanged().connect(this, &RoutingApp::updateView);
    updateView(); // render initial
  }

  void updateView(){
    content_->clear();
    if(internalPath() == "/about") {
      content_->addNew<WText>("<h2>About</h2><p>About this demo...</p>");
    } else if(internalPath() == "/help") {
      content_->addNew<WText>("<h2>Help</h2><p>Help content goes here.</p>");
    } else {
      content_->addNew<WText>("<h2>Home</h2><p>Welcome to the routing demo.</p>");
    }
  }
};

WApplication* createApp(const WEnvironment& env){ return new RoutingApp(env); }
int main(int argc,char**argv){ return WRun(argc,argv,&createApp); }
