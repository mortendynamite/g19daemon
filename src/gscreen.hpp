/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2016  <copyright holder> <email>
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

#ifndef GSCREEN_H
#define GSCREEN_H

#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QImage>
#include <QString>
#include <QRgb>

class Gscreen : public QObject
{
	Q_OBJECT

	public:
        Gscreen(const QImage &Icon, const QString &Name);
        ~Gscreen();

		void setName(const QString &Name);
		void setIcon(const QImage &Icon);
		
		QFont getTextFont();
		QFontMetrics getTextFontMetrics();
		QRgb getTextRgb();
		QRgb getControlBackroundRgb();
		QRgb getControlRgb();
        
		QImage *getScreen();
		
		QImage *draw();
        QImage *drawFullScreen();
		QPainter *begin();
        QPainter *beginFullScreen();
		
		void drawHGuage(int x, int y, int w, int h, int pos);
		void drawHGuage(int x, int y, int w, int h, int pos, QColor color);
		void drawVGuage(int x, int y, int w, int h, int pos);
		void drawVGuage(int x, int y, int w, int h, int pos, QColor color);
		void end();
		
		void drawScrollbar (int pos, int max);
		
		
	private:
		QImage *screen;
		QImage *userscreen;
		QImage *icon;
		QFont fontHeader;
		QFont fontText;
		QPainter *userPainter;
		QRgb rgbText;
		QRgb rgbHeaderText;
		QRgb rgbControlBackground;
		QRgb rgbControl;
		QString fontName;
		QString name;
};

#endif // GSCREEN_H
