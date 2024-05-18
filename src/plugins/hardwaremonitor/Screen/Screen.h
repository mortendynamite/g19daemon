#ifndef SCREEN_H
#define SCREEN_H

#include "../Defines.h"
#include "../../../gscreen.hpp"
#include "../Tools/Data.h"
#include <QDebug>

class Screen {
public:
    Screen(QString name);

    virtual ~Screen();

    virtual ScreenType getScreenType();

    QString getName();

    virtual void update();

    virtual void draw(Gscreen *screen);

    virtual void cleanData();

    virtual void openCustomScreen();

    virtual void okPressed();

    virtual void setBackground(QString background);

    QString getBackground();

protected:
    QString name_;
    QString backgroundString_;
    bool firstStart_;

    Data *data_;

private:

};

#endif
