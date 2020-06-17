//-----------------------------------------------------------------
// NormalScreen File
// C++ Source - NormalScreen.cpp - version 0.1 (2013/06/13)
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "NormalScreen.h"

//-----------------------------------------------------------------
// Defines
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// NormalScreen methods
//-----------------------------------------------------------------

NormalScreen::NormalScreen(QString name) : Screen(name)
{
    // nothing to create
}


NormalScreen::~NormalScreen()
{
	screenLines_.clear();
}

ScreenType NormalScreen::getScreenType()
{
	return ScreenType::Normal;
}
QList<LineText> NormalScreen::getLines()
{
	return screenLines_;
}

void NormalScreen::setData(QList<LineText> data)
{
	screenLines_ = data;
}

QList<CustomSettings> NormalScreen::getSettings()
{
	return lineSettings_;
}

void NormalScreen::setSettings(QList<CustomSettings> settings)
{
	lineSettings_ = settings;
}

void NormalScreen::draw(Gscreen *screen)
{

}

void NormalScreen::update()
{
	
}

void NormalScreen::cleanData()
{

}

void NormalScreen::okPressed()
{
}
