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

#include "gscreen.hpp"

#include <QFont>
#include <QFontMetrics>
#include <QImage>
#include <QPainter>
#include <QRgb>

Gscreen::Gscreen(const QImage &Icon, const QString &Name) {
    screen = new QImage(320, 240, QImage::Format_RGB32);
    userscreen = new QImage(320, 206, QImage::Format_ARGB32);
    userPainter = new QPainter();
    icon = new QImage(Icon);
    name = Name;

    fontName = "Noto Sans";
    rgbText = qRgb(255, 255, 255);
    rgbHeaderText = qRgb(255, 255, 255);
    rgbControlBackground = qRgb(183, 184, 187);
    rgbControl = qRgb(116, 119, 123);
    fontHeader = QFont(fontName, 16);
    fontText = QFont(fontName, 15);
}

Gscreen::~Gscreen() {
    delete screen;
    delete userscreen;
    delete icon;
    delete userPainter;
}

QImage *Gscreen::getScreen() { return screen; }

void Gscreen::setName(const QString &Name) { name = Name; }

void Gscreen::setIcon(const QImage &Icon) { icon = new QImage(Icon); }

QRgb Gscreen::getTextRgb() { return rgbText; }

QRgb Gscreen::getControlRgb() { return rgbControl; }

QRgb Gscreen::getControlBackroundRgb() { return rgbControlBackground; }

QFont Gscreen::getTextFont() { return fontText; }

QFontMetrics Gscreen::getTextFontMetrics() { return QFontMetrics(fontText); }

QImage *Gscreen::draw() {
    QPainter painter;
    QLinearGradient background(0, 0, 0, 240);
    QLinearGradient header(0, 0, 0, 30);

    background.setColorAt(0, qRgb(0, 0, 0));
    background.setColorAt(1, qRgb(0, 115, 153));
    header.setColorAt(0, qRgb(137, 182, 196));
    header.setColorAt(0.5, qRgb(0, 165, 219));
    header.setColorAt(1, qRgb(0, 105, 140));
    painter.begin(screen);
    painter.fillRect(0, 0, 320, 240, background);
    painter.fillRect(0, 0, 320, 30, header);
    painter.drawImage(3, 3, *icon, 0, 0, 24, 24);
    painter.setFont(fontHeader);
    painter.setPen(rgbHeaderText);
    painter.drawText(35, 0, 290, 30, Qt::AlignVCenter | Qt::AlignLeft, name);
    painter.drawImage(0, 32, *userscreen);
    painter.end();

    return screen;
}

QImage *Gscreen::drawFullScreen() { return screen; }

QPainter *Gscreen::begin() {
    userscreen->fill(qRgba(0, 0, 0, 0));
    userPainter->begin(userscreen);
    userPainter->setFont(fontText);
    userPainter->setPen(rgbText);
    return userPainter;
}

QPainter *Gscreen::beginFullScreen() {
    screen->fill(qRgba(0, 0, 0, 0));
    userPainter->begin(screen);
    userPainter->setFont(fontText);
    userPainter->setPen(rgbText);
    return userPainter;
}

void Gscreen::end() { userPainter->end(); }

void Gscreen::drawScrollbar(int pos, int max) {
    int y, h, step;

    step = 200 / max;
    h = step < 6 ? 6 : step;
    y = 2 + (pos * step);

    userPainter->setPen(rgbControlBackground);
    userPainter->setBrush(QBrush(rgbControlBackground));
    userPainter->drawRoundedRect(306, 2, 12, 202, 6, 6);
    userPainter->setPen(rgbControl);
    userPainter->setBrush(QBrush(rgbControl));
    userPainter->drawRoundedRect(306, y, 12, h, 6, 6);
}

void Gscreen::drawHGuage(int x, int y, int w, int h, int pos) {
    drawHGuage(x, y, w, h, pos, rgbControl);
}

void Gscreen::drawHGuage(int x, int y, int w, int h, int pos, QColor color) {
    int cy1, cy2, cheight;

    cheight = (h / 100.0) * pos;
    cy1 = y + h - cheight;
    cy2 = y + h - cy1;

    userPainter->setPen(rgbControlBackground);
    userPainter->setBrush(QBrush(rgbControlBackground));
    userPainter->drawRoundedRect(x, y, w, h, 8, 8);
    if (pos > 0) {
        userPainter->setPen(color);
        userPainter->setBrush(color);
        userPainter->drawRoundedRect(x, cy1, w, cy2, 8, 8);
    }
}

void Gscreen::drawVGuage(int x, int y, int w, int h, int pos) {
    drawVGuage(x, y, w, h, pos, rgbControl);
}

void Gscreen::drawVGuage(int x, int y, int w, int h, int pos, QColor color) {
    int cx1, cwidth;

    cwidth = (w / 100.0) * pos;
    cx1 = x + cwidth;

    userPainter->setPen(rgbControlBackground);
    userPainter->setBrush(QBrush(rgbControlBackground));
    userPainter->drawRoundedRect(x, y, w, h, 8, 8);
    if (pos > 0) {
        userPainter->setPen(color);
        userPainter->setBrush(color);
        userPainter->drawRoundedRect(x, y, cx1, h, 8, 8);
    }
}
