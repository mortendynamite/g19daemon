#ifndef GRAPHSCREEN_H
#define GRAPHSCREEN_H

#include "Screen.h"
#include "../external/QCustomPlot/qcustomplot.h"
#include "LegendScreen.h"

class GraphScreen : public Screen
{
public:
    GraphScreen(QString);
	virtual ~GraphScreen();

    void draw(Gscreen *screen);
	void update();

	ScreenType getScreenType();

	void setData(QList<GraphLine>);
    void okPressed();
	QList<GraphLine> getData();

	void setBackground(QString background);

	void cleanData();
	void openCustomScreen();
	void setSettings(GraphSettings settings);

	GraphSettings getGraphSettings();

private:
	void createPlot();

	QList<GraphLine> graphData_;
	QCustomPlot * plot_;
	int Xpos_;

	LegendScreen * legendScreen_;
	bool legendOpen_;
	GraphSettings settings_;
	
};

#endif
