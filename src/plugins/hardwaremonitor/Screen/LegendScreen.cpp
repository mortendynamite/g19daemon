#include "LegendScreen.h"

LegendScreen::LegendScreen(QString name) : Screen(name), Xpos_(0), settings_({ 0 }), firstrun_(true)
{
   // setBackground("");
}


LegendScreen::~LegendScreen()
{
}

ScreenType LegendScreen::getScreenType()
{
	return ScreenType::Legend;
}

void LegendScreen::draw(Gscreen *screen)
{
    QPainter * p = screen->beginFullScreen();

    QPixmap background(getBackground());

    p->drawPixmap(0, 0, 320, 240, background);

    p->setFont(settings_.titleFont);
    p->setPen(settings_.titleColor);
    const QRect rectangle = QRect(0, 0, 320, 50 );
    QRect boundingRect;
    p->drawText(rectangle, Qt::AlignCenter | Qt::AlignTop | Qt::TextSingleLine	, name_, &boundingRect);

   int textPosition = boundingRect.y() +  settings_.titleFont.pointSize();

    for (int i = 0; i < graphData_.size(); i++)
    {
        HardwareSensor sensor = data_->translateLine(graphData_[i].query);

        const QRect rectangle = QRect(0, textPosition, 320, 50 );
        QRect boundingRect;

        p->setPen(graphData_[i].color);
        p->drawText(rectangle, Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, graphData_[i].text + " (" + sensor.unit + ")", &boundingRect);
        textPosition += boundingRect.y() +  settings_.titleFont.pointSize();
    }

    screen->end();
}

void LegendScreen::update()
{

}

void LegendScreen::setData(QList<GraphLine> data)
{
	graphData_ = data;

}

QList<GraphLine> LegendScreen::getData()
{
	return graphData_;
}

void LegendScreen::setSettings(GraphSettings settings)
{
	settings_ = settings;
}

void LegendScreen::okPressed()
{
}
