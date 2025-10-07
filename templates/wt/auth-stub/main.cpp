// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: Simple username/password form, checks fixed credentials.


#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
using namespace Wt;

class AuthApp : public WApplication {
  WLineEdit *user_{nullptr};
  WLineEdit *pass_{nullptr};
  WText *status_{nullptr};
public:
  AuthApp(const WEnvironment& env) : WApplication(env) {
    setTitle("Umicom Wt Auth Stub");
    auto c = root();
    c->addNew<WText>("<h2>Sign in</h2>");
    user_ = c->addNew<WLineEdit>(); user_->setPlaceholderText("Username");
    pass_ = c->addNew<WLineEdit>(); pass_->setPlaceholderText("Password"); pass_->setEchoMode(WLineEdit::Password);
    auto btn = c->addNew<WPushButton>("Sign in");
    status_ = c->addNew<WText>();
    btn->clicked().connect([this]{
      auto u = user_->text().toUTF8();
      auto p = pass_->text().toUTF8();
      if(u=="admin" && p=="admin") status_->setText("<b>Welcome!</b>");
      else status_->setText("<span style='color:red'>Invalid credentials</span>");
    });
  }
};

WApplication* createApp(const WEnvironment& env){ return new AuthApp(env); }
int main(int argc,char**argv){ return WRun(argc,argv,&createApp); }
