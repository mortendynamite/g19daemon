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

#include "clock.hpp"
#include "../../g19daemon.hpp"
#include "../../gscreen.hpp"
#include <QtCore>
#include <QImage>
#include <QPainter>
#include <QString>
#include <QTime>
#include <QTimer>

Clock::Clock()
{
        Q_INIT_RESOURCE(clock);

	isActive = false;
        screen = new gScreen(QImage(":/clock/icon.png"), tr("Clock"));

        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, QOverload<>::of(&Clock::paint));
        timer->start(1000);
}

Clock::~Clock()
{
	delete screen;
}

QString Clock::getName()
{
    return tr("Clock");
}

void Clock::LKeys(int keys)
{
}

void Clock::setActive(bool active)
{
	isActive = active;

	if (active)
	{
		paint();
	}
}

void Clock::paint()
{
	QPainter *p;

	if (!isActive)
		return;

        static const QPoint hourHand[3] = {
            QPoint(7, 8),
            QPoint(-7, 8),
            QPoint(0, -40)
        };
        static const QPoint minuteHand[3] = {
            QPoint(7, 8),
            QPoint(-7, 8),
            QPoint(0, -70)
        };

        QColor hourColor(255, 25, 0);
        QColor minuteColor(0, 169, 181, 168);

        int side = qMin(320, 206);
        QTime time = QTime::currentTime();
	p = screen->Begin();

        p->setRenderHint(QPainter::Antialiasing);
        p->translate(320 / 2, 206 / 2);
        p->scale(side / 200.0, side / 200.0);
        p->setPen(Qt::NoPen);
        p->setBrush(hourColor);
        p->save();
        p->rotate(30.0 * ((time.hour() + time.minute() / 60.0)));
        p->drawConvexPolygon(hourHand, 3);
        p->restore();
        p->setPen(hourColor);
        for (int i = 0; i < 12; ++i) {
            p->drawLine(88, 0, 96, 0);
            p->rotate(30.0);
        }
        p->setPen(Qt::NoPen);
        p->setBrush(minuteColor);
        p->save();
        p->rotate(6.0 * (time.minute() + time.second() / 60.0));
        p->drawConvexPolygon(minuteHand, 3);
        p->restore();
        p->setPen(minuteColor);

        for (int j = 0; j < 60; ++j) {
            if ((j % 5) != 0)
                p->drawLine(92, 0, 96, 0);
            p->rotate(6.0);
        }

	screen->End();
	emit doAction(displayScreen, screen);
}


bool Clock::isPopup()
{
	return false;
}

QObject * Clock::getQObject()
{
	return this;
}

QImage Clock::getIcon()
{
        return QImage(":/clock/menu_icon.png");
}
