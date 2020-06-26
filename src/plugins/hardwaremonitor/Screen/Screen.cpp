#include "Screen.h"

Screen::Screen(QString name)
    : name_(name), backgroundString_(""), firstStart_(true) {
  data_ = Data::Instance();
}

Screen::~Screen() {}

ScreenType Screen::getScreenType() { return ScreenType::No; }

void Screen::update() {
  // Nothing to do
}

void Screen::draw(Gscreen *screen) {
  // Nothing to do
}

void Screen::cleanData() {
  // Nothing to do
}

void Screen::openCustomScreen() {
  // Nothing to do
}

void Screen::okPressed() {}

QString Screen::getName() { return name_; }

void Screen::setBackground(QString background) {
  if (background.isEmpty()) {
    backgroundString_ = ":/hardwaremonitor/images/Default.png";
  } else {
    backgroundString_ =
        QDir::home().absolutePath() + "/.config/HWA//Background/" + background;
  }
}

QString Screen::getBackground() { return backgroundString_; }
