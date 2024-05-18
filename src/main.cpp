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

#include "config.h"
#include "g19daemon.hpp"
#include <QApplication>
#include <QtCore/QtCore>
#include <src/singleapplication/singleapplication.h>


int main(int argc, char **argv) {

    //Create a application with only one instance
    SingleApplication app(argc, argv);
    QCoreApplication::setApplicationName("G19");
    QCoreApplication::setApplicationVersion(VERSION);

    // create the main class
    G19daemon task;

    // connect up the signals
    QObject::connect(&task, SIGNAL(finished()), &app, SLOT(quit()));
    QObject::connect(&app, SIGNAL(aboutToQuit()), &task,
                     SLOT(aboutToQuitApp()));

    // This code will start the messaging engine in QT and in
    // 10ms it will start the execution in the MainClass.run routine;
    QTimer::singleShot(10, &task, SLOT(run()));

    return app.exec();
}
