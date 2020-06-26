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

#include "plugin_template.hpp"
#include <QCoreApplication>
#include <QTimer>

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  QCoreApplication::setApplicationName("plugin-template");
  QCoreApplication::setApplicationVersion("0.1");

  // create the main class
  plugin_template plugin_template;

  // connect up the signals
  QObject::connect(&plugin_template, SIGNAL(finished()), &app, SLOT(quit()));
  QObject::connect(&app, SIGNAL(aboutToQuit()), &plugin_template,
                   SLOT(aboutToQuitApp()));

  // This code will start the messaging engine in QT and in
  // 10ms it will start the execution in the MainClass.run routine;
  QTimer::singleShot(10, &plugin_template, SLOT(run()));

  return app.exec();
}
