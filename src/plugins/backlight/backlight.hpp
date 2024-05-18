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

#ifndef BACKLIGHT_H
#define BACKLIGHT_H

#include "../../gscreen.hpp"
#include "../../plugininterface.hpp"
#include <QSettings>
#include <QtCore>
#include <QtPlugin>

class Backlight : public QObject, public PluginInterface {
    Q_OBJECT
  Q_INTERFACES(PluginInterface)

  Q_PLUGIN_METADATA(IID "Backlight")

public:
  Backlight();
  ~Backlight() override;
  void lKeys(int keys) override;
  QString getName() override;
  QImage getIcon() override;
  void setActive(bool active) override;
  bool isPopup() override;
  QObject *getQObject() override;
  void mKeys(int keys) override;

private:
  Gscreen *screen;
  bool isActive;

  QString profile;
  QSettings *settings;
  int Value;
  int step;

  void drawGuage(int x, int y, int w, int h, int pos, QColor color,
                 QPainter *p);

  int selected;

  void paint();

signals:
  void doAction(gAction action, void *data); // Signal to draw img on screen
};

#endif // BACKLIGHT_H
