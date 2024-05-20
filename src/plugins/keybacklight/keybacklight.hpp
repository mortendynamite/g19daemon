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

#ifndef KEYBOARDLIGHT_H
#define KEYBOARDLIGHT_H

#include "../../plugininterface.hpp"
#include "../../gscreen.hpp"
#include <QObject>
#include <QSettings>
#include <QtCore>
#include <QtPlugin>

class KeyBacklight : public QObject, public PluginInterface {
Q_OBJECT

    Q_INTERFACES(PluginInterface)

    Q_PLUGIN_METADATA(IID "KeyBacklight")

public:
    KeyBacklight();

    ~KeyBacklight();

    void lKeys(int keys);

    QString getName();

    QImage getIcon();

    void setActive(bool active);

    bool isPopup();

    QObject *getQObject();

    void mKeys(int keys);

private:
    Gscreen *screen;
    bool isActive;

    QSettings *settings;
    int redValue;
    int greenValue;
    int blueValue;
    int step;
    QColor color;
    QString profile;

    void drawGuage(int x, int y, int w, int h, int pos, QColor color,
                   QPainter *p);

    int selected;

    void paint();

signals:

    void doAction(gAction action, void *data); // Signal to draw img on screen
};

#endif // KEYBOARDLIGHT_H
