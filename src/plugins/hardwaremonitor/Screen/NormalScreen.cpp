//-----------------------------------------------------------------
// NormalScreen File
// C++ Source - NormalScreen.cpp - version 0.1 (2013/06/13)
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "NormalScreen.h"

//-----------------------------------------------------------------
// Defines
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// NormalScreen methods
//-----------------------------------------------------------------

NormalScreen::NormalScreen(QString name) : Screen(name) {
  // nothing to create
}

NormalScreen::~NormalScreen() { screenLines_.clear(); }

ScreenType NormalScreen::getScreenType() { return ScreenType::Normal; }
QList<LineText> NormalScreen::getLines() { return screenLines_; }

void NormalScreen::setData(QList<LineText> data) { screenLines_ = data; }

QList<CustomSettings> NormalScreen::getSettings() { return lineSettings_; }

void NormalScreen::setSettings(QList<CustomSettings> settings) {
  lineSettings_ = settings;
}

void NormalScreen::draw(Gscreen *screen) {
  QPainter *p = screen->beginFullScreen();

  QPixmap background(getBackground());
  p->drawPixmap(0, 0, 320, 240, background);

  int textPosition = 0;

  QStringList lines = data_->translateLines(screenLines_);

  for (int i = 0; i < lines.size(); i++) {
    CustomSettings custom = lineSettings_[i];

    Qt::AlignmentFlag aligment;

    /*
     *LGObjectType objectType;
     * if (custom.textScrolling)
    {
        objectType = LG_SCROLLING_TEXT;
    }
    else
    {
        objectType = LG_STATIC_TEXT;
    }*/

    switch (custom.aligment) {
    case Alignment::Center:
      aligment = Qt::AlignCenter;
      break;
    case Alignment::Left:
      aligment = Qt::AlignLeft;
      break;
    case Alignment::Right:
      aligment = Qt::AlignRight;
      break;
    default:
      aligment = Qt::AlignLeft;
      break;
    }
    p->setFont(custom.font);

    const QRect rectangle = QRect(0, textPosition, 320, 50);
    p->drawText(rectangle, aligment | Qt::AlignTop | Qt::TextSingleLine,
                lines[i]);

    QFontMetrics metric(custom.font);

    textPosition += metric.height() + custom.lineSpacing;
  }

  screen->end();
}

void NormalScreen::update() {}

void NormalScreen::cleanData() {}

void NormalScreen::okPressed() {}
