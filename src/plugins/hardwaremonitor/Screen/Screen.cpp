//-----------------------------------------------------------------
// Screen File
// C++ Source - Screen.cpp - version 0.1 (2013/06/13)
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Screen.h"

//-----------------------------------------------------------------
// Defines
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Screen methods
//-----------------------------------------------------------------
#ifdef __linux__
Screen::Screen( QString name) : name_(name), backgroundString_(""), firstStart_(true)
{

}

#endif

Screen::~Screen()
{

}


ScreenType Screen::getScreenType()
{
	return ScreenType::No;
}

void Screen::update()
{
	//Nothing to do
}

void Screen::draw(Gscreen *screen)
{
	//Nothing to do
}

void Screen::cleanData()
{
	//Nothing to do
}

void Screen::openCustomScreen()
{
	//Nothing to do
}

void Screen::okPressed()
{

}

QString Screen::getName()
{
	return name_;
}

void Screen::setBackground(QString background)
{

}

QString Screen::getBackground()
{
	return backgroundString_;
}

