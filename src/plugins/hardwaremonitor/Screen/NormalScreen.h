#ifndef NORMALSCREEN_H
#define NORMALSCREEN_H

#include "Screen.h"
#include <QFontMetrics>

class NormalScreen : public Screen {
public:

    NormalScreen(QString);

    virtual ~NormalScreen();

    ScreenType getScreenType();

    void update();

    void draw(Gscreen *screen);

    void cleanData();

    void okPressed();

    void setData(QList<LineText>);

    void setSettings(QList<CustomSettings> settings);

    QList<LineText> getLines();

    QList<CustomSettings> getSettings();

private:
    QList<LineText> screenLines_;
    QList<CustomSettings> lineSettings_;

};

#endif
