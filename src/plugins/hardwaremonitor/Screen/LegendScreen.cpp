#include "LegendScreen.h"

LegendScreen::LegendScreen(QString name) : Screen(name), Xpos_(0), settings_({ 0 }), firstrun_(true)
{
    setBackground("");
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
    #ifdef _WIN32
	if (firstrun_)
	{
		lcd_->ModifyControlsOnPage(screenPage_);
		lcd_->ModifyDisplay(LG_COLOR);

		HANDLE lineHandle = lcd_->AddCustomText(LG_STATIC_TEXT, settings_.titleFont.pointSize(), DT_CENTER, 320, (LPCTSTR)settings_.titleFont.family().utf16(), false);
		lcd_->SetTextFontColor(lineHandle, RGB(settings_.titleColor.red(), settings_.titleColor.green(), settings_.titleColor.blue()));
		lcd_->SetOrigin(lineHandle, 0, 0);
		lcd_->SetText(lineHandle, (LPCTSTR)(name_).utf16());

		for (int i = 0; i < graphData_.size(); i++)
		{
			HardwareSensor sensor = data_->translateLine(graphData_[i].query);

			HANDLE lineHandle = lcd_->AddCustomText(LG_STATIC_TEXT, 14, DT_LEFT, 320, LG_FONT, false);

			lcd_->SetOrigin(lineHandle, 0, (18 * (i + 1)) + 10);
			lcd_->SetTextFontColor(lineHandle, RGB(graphData_[i].color.red(), graphData_[i].color.green(), graphData_[i].color.blue()));
			lcd_->SetText(lineHandle, (LPCTSTR)(graphData_[i].text + " (" + sensor.unit + ")").utf16());
		}

		firstrun_ = false;
	}
	lcd_->ShowPage(screenPage_);
#endif
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
