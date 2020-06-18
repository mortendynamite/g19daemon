#ifndef LEDGENDSCREEN_H
#define LEDGENDSCREEN_H

#include "Screen.h"
#include "../external/QCustomPlot/qcustomplot.h"

class LegendScreen : public Screen
{
public:
    LegendScreen(QString);
	virtual ~LegendScreen();

    void draw(Gscreen *screen);
	void update();

	ScreenType getScreenType();

	void setData(QList<GraphLine>);
	void setSettings(GraphSettings);
    void okPressed();
	QList<GraphLine> getData();

private:
	QList<GraphLine> graphData_;
	//QCustomPlot * plot_;
	int Xpos_;
	//HANDLE bitmapHandle_;
	//HBITMAP bitmap_;
	GraphSettings settings_;
	bool firstrun_;

};
#endif
