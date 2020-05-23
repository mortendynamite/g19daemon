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

#ifndef CLOCK_H
#define CLOCK_H

#include <QtCore>
#include <QtPlugin>
#include "../../plugininterface.hpp"
#include "../../gscreen.hpp"

class Clock: public QObject, public PluginInterface
{
	Q_OBJECT
    Q_INTERFACES(PluginInterface)

        Q_PLUGIN_METADATA(IID "clock")
	
	public:
                Clock();
                ~Clock();
		void LKeys(int keys);
		QString getName();
		QImage getIcon();
		void setActive(bool active);
		bool isPopup();
		QObject *getQObject();
	
	private:
		gScreen *screen;
		bool isActive;
		void paint();

	signals:
		void doAction(gAction action, void *data);			// Signal to draw img on screen
		
};

#endif // CLOCK_H
