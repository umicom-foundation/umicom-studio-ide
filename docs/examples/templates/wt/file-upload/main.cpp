/* -----------------------------------------------------------------------------
 * Umicom Studio IDE
 * PURPOSE: Core sources for Umicom Studio IDE.
 * Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
 * Last updated: 2025-10-11
 * ---------------------------------------------------------------------------*/
// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: Use WFileUpload to receive a single file; shows result on page.


#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WFileUpload.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <filesystem>
#include <fstream>
using namespace Wt;

class UploadApp : public WApplication {
  WFileUpload *up_{nullptr};
  WText *status_{nullptr};
public:
  UploadApp(const WEnvironment& env) : WApplication(env) {
    setTitle("Umicom Wt Upload");
    auto c = root();
    up_ = c->addNew<WFileUpload>();
    up_->setFileTextSize(50);
    auto btn = c->addNew<WPushButton>("Upload");
    status_ = c->addNew<WText>("");

    btn->clicked().connect([this]{
      up_->upload();
    });
    up_->uploaded().connect([this]{
      std::filesystem::create_directories("uploads");
      auto src = up_->spoolFileName();
      auto dst = std::filesystem::path("uploads") / up_->clientFileName().toUTF8();
      std::ifstream in(src, std::ios::binary);
      std::ofstream out(dst, std::ios::binary);
      out << in.rdbuf();
      status_->setText("<b>Uploaded:</b> " + up_->clientFileName().toUTF8());
    });
    up_->fileTooLarge().connect([this]{ status_->setText("<span style='color:red'>File too large</span>"); });
  }
};

WApplication* createApp(const WEnvironment& env){ return new UploadApp(env); }
int main(int argc,char**argv){ return WRun(argc,argv,&createApp); }
