//-----------------------------------------------------------------
// Screen File
// C++ Header - Screen.h - version 0.1 (2013/06/13)
//-----------------------------------------------------------------
#ifndef SCREEN_H
#define SCREEN_H

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "../Defines.h"
#include "../../../gscreen.hpp"

//-----------------------------------------------------------------
// Screen Class
//-----------------------------------------------------------------
class Screen
{
public:
	//---------------------------
	// Constructor(s)
	//---------------------------
       Screen(QString name);


	//---------------------------
	// Destructor
	//---------------------------
	virtual ~Screen();

	//---------------------------
	// General Methods
	//---------------------------
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
	// -------------------------
	// Datamembers
    // -------------------------

	QString name_;
	QString backgroundString_;
	bool firstStart_;

private:
	
	// -------------------------
	// Disabling default copy constructor and default assignment operator.
	// If you get a linker error from one of these functions, your class is internally trying to use them. This is
	// an error in your class, these declarations are deliberately made without implementation because they should never be used.
	// -------------------------
	Screen(const Screen& t);
	Screen& operator=(const Screen& t);
};
#endif
