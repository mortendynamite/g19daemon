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

#ifndef HARDWAREMONITOR_H
#define HARDWAREMONITOR_H

#include "../../g19daemon.hpp"
#include "../../gscreen.hpp"
#include "../../plugininterface.hpp"
#include "Defines.h"
#include "HwaSettings.h"
#include "Screen/GraphScreen.h"
#include "Screen/NormalScreen.h"
#include "Screen/Screen.h"
#include "Screen/StartScreen.h"
#include <QDebug>
#include <QFileSystemWatcher>
#include <QImage>
#include <QPainter>
#include <QString>
#include <QTime>
#include <QTimer>
#include <QtCore>
#include <QtPlugin>

class HardwareMonitor : public QObject, public PluginInterface {
  Q_OBJECT
  Q_INTERFACES(PluginInterface)

  Q_PLUGIN_METADATA(IID "hardwaremonitor")

public:
  HardwareMonitor();
  ~HardwareMonitor() override;
  void lKeys(int keys) override;
  QString getName() override;
  QImage getIcon() override;
  void setActive(bool active) override;
  bool isPopup() override;
  QObject *getQObject() override;
  void mKeys(int keys) override;
private:
  void loadsettings();
  Gscreen *screen;
  Screen *currentScreen_;
  Screen *currentMainScreen_;
  bool isActive;
  void paint();

  QVector<Screen *> screens;
  StartScreen *startScreen;
  QFileSystemWatcher *watcher;

private slots:
  void reloadSettings();

signals:
  void doAction(gAction action, void *data); // Signal to draw img on screen
};

#endif // HARDWAREMONITOR_H
