// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: Create a bar chart from static data.


#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WStandardItemModel.h>
#include <Wt/Chart/WCartesianChart.h>
#include <Wt/Chart/WBarSeries.h>
using namespace Wt;

class ChartApp : public WApplication {
public:
  ChartApp(const WEnvironment& env): WApplication(env) {
    setTitle("Umicom Wt Charts");
    auto model = std::make_shared<WStandardItemModel>(3,2);
    model->setHeaderData(0, Orientation::Horizontal, "Label");
    model->setHeaderData(1, Orientation::Horizontal, "Value");
    model->setData(0,0,"A"); model->setData(0,1,10);
    model->setData(1,0,"B"); model->setData(1,1,25);
    model->setData(2,0,"C"); model->setData(2,1,15);

    auto chart = root()->addNew<Chart::WCartesianChart>();
    chart->setModel(model);
    chart->setXSeriesColumn(0);
    auto series = std::make_unique<Chart::WBarSeries>(1);
    chart->addSeries(std::move(series));
    chart->resize(600, 400);
  }
};

WApplication* createApp(const WEnvironment& env){ return new ChartApp(env); }
int main(int argc,char**argv){ return WRun(argc,argv,&createApp); }
