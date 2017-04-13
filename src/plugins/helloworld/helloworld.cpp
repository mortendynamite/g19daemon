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

#include "helloworld.hpp"
#include "../../g19daemon.hpp"
#include "../../gscreen.hpp"
#include <QtCore>
#include <QImage>
#include <QPainter>
#include <QString>

HelloWorld::HelloWorld()
{
	Q_INIT_RESOURCE(helloworld);

	isActive = false;
	screen = new gScreen(QImage(":/helloworld/icon.png"), tr("Hello World"));
}

HelloWorld::~HelloWorld()
{
	delete screen;
}

QString HelloWorld::getName()
{
    return tr("Hello World");
}

void HelloWorld::LKeys(int keys)
{
}

void HelloWorld::setActive(bool active)
{
	isActive = active;

	if (active)
	{
		paint();
	}
}

void HelloWorld::paint()
{
	QPainter *p;

	if (!isActive)
		return;

	p = screen->Begin();
	p->drawText(0, 0, 320, 206, Qt::AlignCenter, tr("Hello World!"));
	screen->End();
	emit doAction(displayScreen, screen);
}

bool HelloWorld::isPopup()
{
	return false;
}

QObject * HelloWorld::getQObject()
{
	return this;
}

QImage HelloWorld::getIcon()
{
	return QImage(":/helloworld/menu_icon.png");
}
