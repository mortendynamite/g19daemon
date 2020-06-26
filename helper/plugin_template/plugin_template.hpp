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

#ifndef plugin_template_H
#define plugin_template_H

#include <QtCore/QObject>

class plugin_template : public QObject {
  Q_OBJECT

public:
  plugin_template();
  virtual ~plugin_template();

  // Call this to quit application
  void quit();

signals:
  // Signal to finish, this is connected to Application Quit
  void finished();

public slots:
  // This is the slot that gets called from main to start everything
  // but, everthing is set up in the Constructor
  void run();

  // slot that get signal when that application is about to quit
  void aboutToQuitApp();
private slots:

private:
  QString saveHeader(const QString &name);
  QString saveCpp(const QString &name);
  QString saveQrc(const QString &name);
  QString saveCMakeLists(const QString &name);
};

#endif // plugin_template_H
