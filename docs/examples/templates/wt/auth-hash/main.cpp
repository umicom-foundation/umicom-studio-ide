/* -----------------------------------------------------------------------------
 * Umicom Studio IDE
 * PURPOSE: Core sources for Umicom Studio IDE.
 * Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
 * Last updated: 2025-10-11
 * ---------------------------------------------------------------------------*/
// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: Login form compares password against a naive hash (placeholder). For real apps use Wt::Auth or libsodium/argon2/bcrypt.


#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <string>
#include <functional>
using namespace Wt;

static std::string weak_hash(const std::string& s){
  // WARNING: This is a placeholder hash (std::hash). Do NOT use in production.
  return std::to_string(std::hash<std::string>{}(s));
}

class AuthApp : public WApplication {
  WLineEdit *user_{nullptr}, *pass_{nullptr}; WText *status_{nullptr};
  const std::string demo_user_ = "admin";
  const std::string demo_hash_ = weak_hash("admin"); // "admin" for demo
public:
  AuthApp(const WEnvironment& env): WApplication(env){
    setTitle("Umicom Wt Auth (Hash Stub)");
    auto c = root(); c->addNew<WText>("<h2>Sign in</h2>");
    user_ = c->addNew<WLineEdit>(); user_->setPlaceholderText("Username");
    pass_ = c->addNew<WLineEdit>(); pass_->setPlaceholderText("Password"); pass_->setEchoMode(WLineEdit::Password);
    auto btn = c->addNew<WPushButton>("Sign in");
    status_ = c->addNew<WText>("");
    btn->clicked().connect([this]{
      if(user_->text()=="admin" && weak_hash(pass_->text().toUTF8())==demo_hash_) status_->setText("<b>Welcome!</b>");
      else status_->setText("<span style='color:red'>Invalid</span>");
    });
  }
};
WApplication* createApp(const WEnvironment& env){ return new AuthApp(env); }
int main(int argc,char**argv){ return WRun(argc,argv,&createApp); }
