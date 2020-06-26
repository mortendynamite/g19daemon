/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "keybacklight.hpp"
#include "../../g19daemon.hpp"
#include "../../gscreen.hpp"
#include <QImage>
#include <QPainter>
#include <QSettings>
#include <QString>
#include <QtCore>

KeyBacklight::KeyBacklight() {
  Q_INIT_RESOURCE(keybacklight);

  isActive = false;
  QImage icon(":/keybacklight/icon.png");
  screen = new Gscreen(icon, tr("Hello World"));

  settings = new QSettings("Dynamite", "G19Daemon");

  color = new QColor();

  redValue = settings->value("KeyBacklight_Red").toInt();
  greenValue = settings->value("KeyBacklight_Green").toInt();
  blueValue = settings->value("KeyBacklight_Blue").toInt();
  step = 10;
  selected = 1;
}

KeyBacklight::~KeyBacklight() {
  settings->sync();

  delete settings;
  delete color;
  delete screen;
}

void KeyBacklight::lKeys(int keys) {
  if (keys & G19_KEY_LLEFT) {
    selected--;
    if (selected < 1)
      selected = 3;
    paint();
  }

  if (keys & G19_KEY_LRIGHT) {
    selected++;
    if (selected > 3)
      selected = 1;
    paint();
  }

  if (keys & G19_KEY_LUP) {
    switch (selected) {
    case 1:
      redValue += step;
      if (redValue > 255)
        redValue = 255;
      break;
    case 2:
      greenValue += step;
      if (greenValue > 255)
        greenValue = 255;
      break;
    case 3:
      blueValue += step;
      if (blueValue > 255)
        blueValue = 255;
    }
    paint();
  }

  if (keys & G19_KEY_LDOWN) {
    switch (selected) {
    case 1:
      redValue -= step;
      if (redValue < 0)
        redValue = 0;
      break;
    case 2:
      greenValue -= step;
      if (greenValue < 0)
        greenValue = 0;
      break;
    case 3:
      blueValue -= step;
      if (blueValue < 0)
        blueValue = 0;
    }
    paint();
  }

  if (keys & G19_KEY_LCANCEL) {
    switch (step) {
    case 1:
      step = 10;
      break;
    case 10:
      step = 20;
      break;
    case 20:
      step = 1;
      break;
    }
    paint();
  }

  if (keys & G19_KEY_LOK) {
    settings->setValue("KeyBacklight_Red", redValue);
    settings->setValue("KeyBacklight_Green", greenValue);
    settings->setValue("KeyBacklight_Blue", blueValue);
    color->setRed(redValue);
    color->setGreen(greenValue);
    color->setBlue(blueValue);
    emit doAction(setKeyBackground, color);
  }
}

void KeyBacklight::setActive(bool active) {
  if (active) {
    isActive = true;
    paint();
  } else {
    isActive = false;
  }
}

void KeyBacklight::paint() {
  QPainter *p;
  QColor color;
  QString text;
  int value;

  p = screen->begin();

  // Red

  if (selected == 1) {
    color = Qt::red;
    value = redValue;
  } else
    color = Qt::darkRed;

  drawGuage(40, 30, 40, 170, (redValue * 100) / 255, color, p);

  // Green
  if (selected == 2) {
    color = Qt::green;
    value = greenValue;
  } else
    color = Qt::darkGreen;

  drawGuage(132, 30, 40, 170, (greenValue * 100) / 255, color, p);

  // Blue
  if (selected == 3) {
    color = Qt::blue;
    value = blueValue;
  } else
    color = Qt::darkBlue;

  drawGuage(224, 30, 40, 170, (blueValue * 100) / 255, color, p);

  text = "Current step: " + QString::number(step) +
         ", Value: " + QString::number(value);

  p->setPen(Qt::white);
  p->drawText(0, 0, 320, 30, Qt::AlignCenter, text);

  p->end();
  emit doAction(displayScreen, screen);
}

void KeyBacklight::drawGuage(int x, int y, int w, int h, int pos, QColor color,
                             QPainter *p) {
  int cy1, cy2, cheight;

  cheight = (h / 100.0) * pos;
  cy1 = y + h - cheight;
  cy2 = y + h - cy1;

  p->setPen(qRgb(183, 184, 187));
  p->setBrush(QBrush(qRgb(183, 184, 187)));
  p->drawRoundedRect(x, y, w, h, 8, 8);
  p->setPen(color);
  p->setBrush(color);
  p->drawRoundedRect(x, cy1, w, cy2, 8, 8);
}

bool KeyBacklight::isPopup() { return false; }

QImage KeyBacklight::getIcon() {
  return QImage(":/keybacklight/menu_icon.png");
}

QObject *KeyBacklight::getQObject() { return this; }

QString KeyBacklight::getName() { return tr("Key Backlight"); }
