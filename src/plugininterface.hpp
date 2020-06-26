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

#ifndef plugininterface_H
#define plugininterface_H

#include <QImage>
#include <QObject>
#include <QString>
#include <QtPlugin>

enum gAction {
  displayFullScreen,
  displayScreen,
  setKeyBackground,
  setLcdBrightness,
  grabFocus,
  releaseFocus,
  restoreKeyBackground
};

class PluginInterface {
public:
  virtual ~PluginInterface() {}
  virtual void lKeys(int keys) = 0;
  virtual QString getName() = 0;
  virtual QImage getIcon() = 0;
  virtual void setActive(bool active) = 0;
  virtual bool isPopup() = 0;
  virtual QObject *getQObject() = 0;

private:
};

Q_DECLARE_INTERFACE(PluginInterface, "g19daemon.plugin/0.3");

#endif
