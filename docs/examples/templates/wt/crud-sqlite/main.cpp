/* -----------------------------------------------------------------------------
 * Umicom Studio IDE
 * PURPOSE: Core sources for Umicom Studio IDE.
 * Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
 * Last updated: 2025-10-11
 * ---------------------------------------------------------------------------*/
// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: Use Wt::Dbo with SQLite to persist a simple Item table.


#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WTable.h>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Sqlite3.h>
#include <memory>
#include <string>
using namespace Wt;

namespace dbo = Wt::Dbo;

class Item {
public:
  std::string name;
  template<class Action> void persist(Action& a) { dbo::field(a, name, "name"); }
};

class App : public WApplication {
  dbo::Session session_;
  dbo::ptr<Item> tmp_;
  WTable *table_{nullptr};
  WLineEdit *input_{nullptr};
public:
  App(const WEnvironment& env) : WApplication(env) {
    setTitle("Umicom Wt CRUD + SQLite");
    // Connect to SQLite database (file will be created if missing).
    auto sqlite = std::make_unique<dbo::backend::Sqlite3>("crud.sqlite");
    sqlite->setProperty("show-queries","false");
    session_.setConnection(std::move(sqlite));
    session_.mapClass<Item>("item");
    try { session_.createTables(); } catch(...) { /* ignore if already exists */ }

    auto rootC = root();
    rootC->addNew<WText>("<h2>Persistent CRUD (SQLite)</h2>");
    input_ = rootC->addNew<WLineEdit>(); input_->setPlaceholderText("Enter item name");
    auto addBtn = rootC->addNew<WPushButton>("Add");
    addBtn->clicked().connect([this]{ onAdd(); });

    table_ = rootC->addNew<WTable>();
    refresh();
  }

  void onAdd(){
    auto v = input_->text().toUTF8();
    if(v.empty()) return;
    dbo::Transaction t(session_);
    dbo::ptr<Item> it = session_.add(std::make_unique<Item>());
    it.modify()->name = v;
    t.commit();
    input_->setText("");
    refresh();
  }

  void refresh(){
    table_->clear();
    table_->elementAt(0,0)->addNew<WText>("<b>#</b>");
    table_->elementAt(0,1)->addNew<WText>("<b>Name</b>");
    table_->elementAt(0,2)->addNew<WText>("<b>Actions</b>");
    dbo::Transaction t(session_);
    int row=1;
    for(auto it : session_.find<Item>()){
      table_->elementAt(row,0)->addNew<WText>(std::to_string(row));
      auto nameEdit = table_->elementAt(row,1)->addNew<WLineEdit>(it->name);
      auto saveBtn = table_->elementAt(row,2)->addNew<WPushButton>("Save");
      auto delBtn  = table_->elementAt(row,2)->addNew<WPushButton>("Delete");
      saveBtn->clicked().connect([this, it, nameEdit]{ dbo::Transaction tx(session_); it.modify()->name = nameEdit->text().toUTF8(); });
      delBtn->clicked().connect([this, it]{ dbo::Transaction tx(session_); it.remove(); refresh(); });
      ++row;
    }
  }
};

WApplication* createApp(const WEnvironment& env){ return new App(env); }
int main(int argc,char**argv){ return WRun(argc,argv,&createApp); }
