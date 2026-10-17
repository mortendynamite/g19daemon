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
#include <QtCore/QtCore>
#include <QApplication>
#include <QSocketNotifier>
#include <csignal>
#include <sys/socket.h>
#include <unistd.h>

namespace {
int signalSockets[2] = {-1, -1};

void handleSignal(int) {
    const char byte = 1;
    ::write(signalSockets[0], &byte, sizeof(byte));
}
}


int main(int argc, char **argv) {

    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("G19");
    QCoreApplication::setApplicationVersion(VERSION);

    // create the main class
    G19daemon task;

    // Forward POSIX signals into the Qt event loop through a socket pair.
    // write() is async-signal-safe, unlike calling Qt directly from a signal handler.
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, signalSockets) != 0)
        return 1;

    QSocketNotifier signalNotifier(signalSockets[1], QSocketNotifier::Read);
    QObject::connect(&signalNotifier, &QSocketNotifier::activated, &task,
                     [&task, &signalNotifier]() {
                         signalNotifier.setEnabled(false);
                         char byte;
                         ::read(signalSockets[1], &byte, sizeof(byte));
                         task.quit();
                     });

    std::signal(SIGTERM, handleSignal);
    std::signal(SIGINT, handleSignal);

    // connect up the signals
    QObject::connect(&task, SIGNAL(finished()), &app, SLOT(quit()));
    QObject::connect(&app, SIGNAL(aboutToQuit()), &task,
                     SLOT(aboutToQuitApp()));

    // This code will start the messaging engine in QT and in
    // 10ms it will start the execution in the MainClass.run routine;
    QTimer::singleShot(10, &task, SLOT(run()));

    const int result = app.exec();

    ::close(signalSockets[0]);
    ::close(signalSockets[1]);

    return result;
}
