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

#include <QtCore>
#include <QtPlugin>
#include "../../plugininterface.hpp"
#include "../../gscreen.hpp"
#include "HwaSettings.h"
#include "Defines.h"
#include "Screen/Screen.h"
#include "Screen/GraphScreen.h"
#include "Screen/NormalScreen.h"
#include "Screen/StartScreen.h"
#include "../../g19daemon.hpp"
#include <QImage>
#include <QPainter>
#include <QString>
#include <QTime>
#include <QTimer>
#include <QFileSystemWatcher>
#include <QDebug>

class HardwareMonitor: public QObject, public PluginInterface
{
	Q_OBJECT
    Q_INTERFACES(PluginInterface)

        Q_PLUGIN_METADATA(IID "hardwaremonitor")
	
	public:
                HardwareMonitor();
                ~HardwareMonitor();
		void lKeys(int keys);
		QString getName();
		QImage getIcon();
		void setActive(bool active);
		bool isPopup();
		QObject *getQObject();
	
	private:
        void loadsettings();
        Gscreen *screen;
        Screen * currentScreen_;
        Screen * currentMainScreen_;
		bool isActive;
		void paint();

        QVector<Screen*> screens;
        StartScreen* startScreen;
        QFileSystemWatcher * watcher;


    private slots:
        void reloadSettings();

    signals:
		void doAction(gAction action, void *data);			// Signal to draw img on screen
		
};

#endif // HARDWAREMONITOR_H
