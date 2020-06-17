//-----------------------------------------------------------------
// NormalScreen File
// C++ Header - NormalScreen.h - version 0.1 (2013/06/13)
//-----------------------------------------------------------------
#ifndef NORMALSCREEN_H
#define NORMALSCREEN_H

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Screen.h"

//-----------------------------------------------------------------
// NormalScreen Class
//-----------------------------------------------------------------
class NormalScreen : public Screen
{
public:
	//---------------------------
	// Constructor(s)
	//---------------------------
        NormalScreen(QString);

	//---------------------------
	// Destructor
	//---------------------------
	virtual ~NormalScreen();

	//---------------------------
	// General Methods
	//---------------------------
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
	// -------------------------
	// Datamembers
	// -------------------------
	QList<LineText> screenLines_;
	QList<CustomSettings> lineSettings_;
	
	// -------------------------
	// Disabling default copy constructor and default assignment operator.
	// If you get a linker error from one of these functions, your class is internally trying to use them. This is
	// an error in your class, these declarations are deliberately made without implementation because they should never be used.
	// -------------------------
	NormalScreen(const NormalScreen& t);
	NormalScreen& operator=(const NormalScreen& t);
};

#endif
