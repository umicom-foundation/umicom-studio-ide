/* -----------------------------------------------------------------------------
 * Umicom Studio IDE
 * PURPOSE: Core sources for Umicom Studio IDE.
 * Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
 * Last updated: 2025-10-11
 * ---------------------------------------------------------------------------*/
// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: In-memory CRUD demo. Not persistent; for teaching purposes.


#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WTable.h>
#include <vector>
#include <string>
#include <memory>
using namespace Wt;

struct Item {
  std::string name;
};

class CrudApp : public WApplication {
  std::vector<Item> items_;
  WTable *table_{nullptr};
  WLineEdit *input_{nullptr};
public:
  CrudApp(const WEnvironment& env) : WApplication(env) {
    setTitle("Umicom Wt CRUD");
    auto rootC = root();

    auto title = rootC->addNew<WText>("<h2>In-memory CRUD</h2>");
    title->setInline(false);

    input_ = rootC->addNew<WLineEdit>();
    input_->setPlaceholderText("Enter item name");

    auto addBtn = rootC->addNew<WPushButton>("Add");
    addBtn->clicked().connect([this]{
      auto v = input_->text().toUTF8();
      if(!v.empty()) { items_.push_back({v}); input_->setText(""); refresh(); }
    });

    table_ = rootC->addNew<WTable>();
    refresh();
  }

  void refresh(){
    table_->clear();
    table_->elementAt(0,0)->addNew<WText>("<b>#</b>");
    table_->elementAt(0,1)->addNew<WText>("<b>Name</b>");
    table_->elementAt(0,2)->addNew<WText>("<b>Actions</b>");
    for(size_t i=0;i<items_.size();++i){
      table_->elementAt(i+1,0)->addNew<WText>(std::to_string(i+1));
      auto nameEdit = table_->elementAt(i+1,1)->addNew<WLineEdit>(items_[i].name);
      auto delBtn = table_->elementAt(i+1,2)->addNew<WPushButton>("Delete");
      auto saveBtn = table_->elementAt(i+1,2)->addNew<WPushButton>("Save");
      delBtn->clicked().connect([this,i]{ items_.erase(items_.begin()+i); refresh(); });
      saveBtn->clicked().connect([this,i,nameEdit]{ items_[i].name = nameEdit->text().toUTF8(); refresh(); });
    }
  }
};

WApplication* createApp(const WEnvironment& env){ return new CrudApp(env); }
int main(int argc,char**argv){ return WRun(argc,argv,&createApp); }
