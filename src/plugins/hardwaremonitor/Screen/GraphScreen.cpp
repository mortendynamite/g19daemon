#include "GraphScreen.h"

GraphScreen::GraphScreen(QString name)
    : Screen(name), Xpos_(0), legendScreen_(nullptr), legendOpen_(false),
      settings_({0}) {
  legendScreen_ = new LegendScreen("Legend " + name);

  plot_ = new QCustomPlot();
}

GraphScreen::~GraphScreen() {
  if (plot_ != nullptr) {
    delete plot_;
    plot_ = nullptr;
  }

  if (legendScreen_ != nullptr) {
    delete legendScreen_;
    legendScreen_ = nullptr;
  }
}

ScreenType GraphScreen::getScreenType() { return ScreenType::Graph; }

void GraphScreen::draw(Gscreen *screen) {
  if (legendOpen_) {
    legendScreen_->draw(screen);
  } else {
    QPainter *p = screen->beginFullScreen();

    if (Xpos_ >= settings_.range) {
      plot_->xAxis->setRange(Xpos_ - settings_.range, Xpos_);
    } else {
      plot_->xAxis->setRange(0, settings_.range);
    }

    if (settings_.yAutoRange) {
      plot_->yAxis->rescale();
    }

    plot_->replot();

    QPixmap pixmap = plot_->toPixmap(320, 240, 1);

    p->drawPixmap(0, 0, 320, 240, pixmap);

    p->end();
  }
}

void GraphScreen::update() {
  QList<double> listDate = data_->translateLines(graphData_);

  for (int i = 0; i < listDate.size(); i++) {
    plot_->graph(i)->addData(Xpos_, listDate[i]);
  }

  Xpos_++;
}

void GraphScreen::cleanData() {
  if (!legendOpen_) {
    Xpos_ = 0;

    for (int i = 0; i < graphData_.size(); i++) {
      plot_->graph(i)->clearData();
    }
  }
}

GraphSettings GraphScreen::getGraphSettings() { return settings_; }

void GraphScreen::setSettings(GraphSettings settings) {
  settings_ = settings;

  legendScreen_->setSettings(settings_);

  if (settings_.addTitle) {
    plot_->plotLayout()->insertRow(0);

    QCPItemText *plotTitle = new QCPItemText(plot_);
    plotTitle->setText(name_);
    plotTitle->setFont(settings.titleFont);
//    plotTitle->setTextColor(settings.titleColor);

//    plot_->plotLayout()->addElement(0, 0, plotTitle);
  }

  if (settings_.yMaxRange != -1 && settings_.yMinRange != -1) {
    plot_->yAxis->setRange(settings_.yMinRange, settings_.yMaxRange);
  }
}

void GraphScreen::setBackground(QString background) {
  Screen::setBackground(background);
  legendScreen_->setBackground(background);

  QPixmap backgroundPixMap(getBackground());

  if (!backgroundPixMap.isNull()) {
    plot_->setBackground(backgroundPixMap);
  }
}

void GraphScreen::setData(QList<GraphLine> data) {
  graphData_ = data;

  legendScreen_->setData(data);

  createPlot();
}

QList<GraphLine> GraphScreen::getData() { return graphData_; }

void GraphScreen::createPlot() {
  for (int i = 0; i < graphData_.size(); i++) {
    plot_->addGraph();
    plot_->graph(i)->setPen(QPen(graphData_[i].color, 2));
    plot_->graph(i)->setName(graphData_[i].text);
  }
}

void GraphScreen::openCustomScreen() { legendOpen_ = !legendOpen_; }

void GraphScreen::okPressed() { openCustomScreen(); }
