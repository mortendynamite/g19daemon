#ifndef LEDGENDSCREEN_H
#define LEDGENDSCREEN_H

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Screen.h"
#include "../external/QCustomPlot/qcustomplot.h"

//-----------------------------------------------------------------
// LegendScreen Class
//-----------------------------------------------------------------
class LegendScreen : public Screen
{
public:
	//---------------------------
	// Constructor(s)
	//---------------------------


    LegendScreen(QString);


	//---------------------------
	// Destructor
	//---------------------------
	virtual ~LegendScreen();

	//---------------------------
	// General Methods
	//---------------------------
    void draw(Gscreen *screen);
	void update();

	ScreenType getScreenType();

	void setData(QList<GraphLine>);
	void setSettings(GraphSettings);
    void okPressed();
	QList<GraphLine> getData();

private:

	// -------------------------
	// Datamembers
	// -------------------------
	QList<GraphLine> graphData_;
	//QCustomPlot * plot_;
	int Xpos_;
	//HANDLE bitmapHandle_;
	//HBITMAP bitmap_;
	GraphSettings settings_;
	bool firstrun_;

	// -------------------------
	// Disabling default copy constructor and default assignment operator.
	// If you get a linker error from one of these functions, your class is internally trying to use them. This is
	// an error in your class, these declarations are deliberately made without implementation because they should never be used.
	// -------------------------
	LegendScreen(const LegendScreen& t);
	LegendScreen& operator=(const LegendScreen& t);
};
#endif
