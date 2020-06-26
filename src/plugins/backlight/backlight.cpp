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

#include "backlight.hpp"
#include "../../g19daemon.hpp"
#include "../../gscreen.hpp"
#include <QImage>
#include <QPainter>
#include <QSettings>
#include <QString>
#include <QtCore>

Backlight::Backlight() {
  Q_INIT_RESOURCE(backlight);

  isActive = false;
  QImage icon(":/backlight/icon.png");
  screen = new Gscreen(icon, tr("Hello World"));

  settings = new QSettings("Dynamite", "G19Daemon");

  Value = settings->value("Backlight").toInt();
  step = 10;

  selected = 1;
}

Backlight::~Backlight() {
  settings->sync();

  delete settings;
  delete screen;
}

void Backlight::lKeys(int keys) {
  if (keys & G19_KEY_LUP) {
    Value += step;
    if (Value > 127)
      Value = 127;
    paint();
  }

  if (keys & G19_KEY_LDOWN) {
    Value -= step;
    if (Value < 0)
      Value = 0;
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
    settings->setValue("Backlight", Value);
    emit doAction(setLcdBrightness, &Value);
  }
}

void Backlight::setActive(bool active) {
  if (active) {
    isActive = true;
    paint();
  } else {
    isActive = false;
  }
}

void Backlight::paint() {
  QPainter *p;
  QColor color;
  QString text;

  p = screen->begin();

  drawGuage(132, 30, 40, 170, (Value * 100) / 127, qRgb(116, 119, 123), p);

  text = "Current step: " + QString::number(step) +
         ", Value: " + QString::number(Value);

  p->setPen(Qt::white);
  p->drawText(0, 0, 320, 30, Qt::AlignCenter, text);

  p->end();
  emit doAction(displayScreen, screen);
}

void Backlight::drawGuage(int x, int y, int w, int h, int pos, QColor color,
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

bool Backlight::isPopup() { return false; }

QImage Backlight::getIcon() { return QImage(":/backlight/menu_icon.png"); }

QObject *Backlight::getQObject() { return this; }

QString Backlight::getName() { return tr("LCD Brightness"); }
