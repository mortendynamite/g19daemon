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

#include <QApplication>
#include <QtCore/QtCore>
#include <QFile>
#include "g19daemon.hpp"
#include "config.h"


int main(int argc, char** argv)
{
	int ret;
	QString file;
	
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("G19");
	QCoreApplication::setApplicationVersion(VERSION);

	file = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation) + "/g19daemon.pid";
    QFile isRunning_file(file);
    
	qDebug() << isRunning_file.fileName();
	
    if (!isRunning_file.exists())
    {
        if (!isRunning_file.open(QIODevice::WriteOnly))
		{
			qDebug() << "Error: " << isRunning_file.errorString();
		}
		else
		{
			QString pid;
			pid.setNum(app.applicationPid());
			isRunning_file.write(pid.toLatin1(), pid.length());
			isRunning_file.close();
		}

		// create the main class
		g19daemon task;
		
		// connect up the signals
		QObject::connect(&task, SIGNAL(finished()), &app, SLOT(quit()));
		QObject::connect(&app, SIGNAL(aboutToQuit()), &task, SLOT(aboutToQuitApp()));
		
		// This code will start the messaging engine in QT and in
		// 10ms it will start the execution in the MainClass.run routine;
		QTimer::singleShot(10, &task, SLOT(run())); 
		ret = app.exec();
		
		if (!isRunning_file.remove())
		{
			qDebug() << "Error: " << isRunning_file.errorString();
		}
		return ret;
    }
    else
	{
		app.exit;
		return 0;
	}	
}
