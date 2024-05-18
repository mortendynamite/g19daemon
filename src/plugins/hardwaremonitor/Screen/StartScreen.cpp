//-----------------------------------------------------------------
// StartScreen File
// C++ Source - StartScreen.cpp - version 0.1 (2013/06/13)
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "StartScreen.h"

//-----------------------------------------------------------------
// Defines
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// StartScreen methods
//-----------------------------------------------------------------
StartScreen::StartScreen(QString name) : Screen(name) {}

StartScreen::~StartScreen() {}

ScreenType StartScreen::getScreenType() { return ScreenType::Start; }

void StartScreen::update() {}

void StartScreen::draw(Gscreen *screen) {
    QPainter *p;
    p = screen->beginFullScreen();
    p->drawText(0, 0, 320, 206, Qt::AlignCenter,
                "No settings are found! \n "
                "Please download configurator\n"
                "from: \n "
                "https://cutt.ly/Kumdskl \n"
                "or press OK to open url");
    screen->end();
}

void StartScreen::okPressed() {
    QDesktopServices::openUrl(
            QUrl("https://github.com/lonelobo0070/Hardware-Monitor-Applet/releases",
                 QUrl::TolerantMode));
}
