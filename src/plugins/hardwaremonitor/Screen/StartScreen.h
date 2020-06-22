//-----------------------------------------------------------------
// StartScreen File
// C++ Header - StartScreen.h - version 0.1 (2013/06/13)
//-----------------------------------------------------------------
#ifndef STARTSCREEN_H
#define STARTSCREEN_H

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Screen.h"
#include <QObject>
#include <QDesktopServices>
#include <QUrl>


//-----------------------------------------------------------------
// StartScreen Class
//-----------------------------------------------------------------
class StartScreen: public Screen
{
public:
	//---------------------------
	// Constructor(s)
	//---------------------------
        StartScreen(QString);

	//---------------------------
	// Destructor
	//---------------------------
	virtual ~StartScreen();

	//---------------------------
	// General Methods
	//---------------------------
	ScreenType getScreenType();
	void update();
        void draw(Gscreen *screen);
        void okPressed();
	

private:
	// -------------------------
	// Datamembers
	// -------------------------
	
	// -------------------------
	// Disabling default copy constructor and default assignment operator.
	// If you get a linker error from one of these functions, your class is internally trying to use them. This is
	// an error in your class, these declarations are deliberately made without implementation because they should never be used.
	// -------------------------
	StartScreen(const StartScreen& t);
	StartScreen& operator=(const StartScreen& t);
};
#endif
