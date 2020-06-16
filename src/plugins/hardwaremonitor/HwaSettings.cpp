//-----------------------------------------------------------------
// Settings File
// C++ Source - Settings.cpp - version 0.1 (2013/06/13)
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "HwaSettings.h"

//-----------------------------------------------------------------
// Defines
//-----------------------------------------------------------------
HwaSettings* HwaSettings::singleton_ = nullptr;

//-----------------------------------------------------------------
// Settings methods
//-----------------------------------------------------------------
HwaSettings* HwaSettings::getInstance()
{
	if (singleton_ == nullptr)
        singleton_ = new HwaSettings();

	return singleton_;
}

void HwaSettings::releaseResources()
{
	if (singleton_ != nullptr)
	{
		delete singleton_;
		singleton_ = nullptr;
	}
}

HwaSettings::HwaSettings() : generalSettings_(GeneralSettings{ TemperatureType::Celsius })
{
        QString fileName = QDir::home().absolutePath() + "/.config/HWA/settings.ini";
	settings_ = new QSettings(fileName, QSettings::IniFormat);
}

HwaSettings::~HwaSettings()
{
    for(Screen* screen : screenList_)
    {
        delete screen;
    }


	if (settings_ != nullptr)
	{
		delete settings_;
		settings_ = nullptr;
	}
}

TemperatureType HwaSettings::getTemperature()
{
	loadGeneralSettings();

	return generalSettings_.temperature;
}

void HwaSettings::loadSettings()
{
    qDebug() << "Loading settings";
	loadGeneralSettings();

	int screenTotal = settings_->beginReadArray("pages");

	for (int i = 0; i < screenTotal; i++)
	{
		settings_->setArrayIndex(i);
		QString name = settings_->value("pageName").toString();
		QString background = settings_->value("background").toString();
		ScreenType type = Defines::translateScreenTypeEnum(settings_->value("type").toString());

		if (type == ScreenType::Normal)
		{
			loadNormalScreenSettings(name, background, type);

			QList<CustomSettings> list = loadCustomSettings();
                        NormalScreen* currentScreen = static_cast<NormalScreen*>(getScreenData(name));
			currentScreen->setSettings(list);
		}

		else if(type == ScreenType::Graph)
		{
			loadGraphScreenSettings(name, background, type);
		}
	}

	settings_->endArray();

	loadScreenOrder();
}


Screen * HwaSettings::getScreenData(QString name)
{
        for (Screen * screen : screenList_)
        {
                if (screen->getName() == name)
                {
                        return screen;
                }
        }

        return nullptr;
}

void HwaSettings::loadNormalScreenSettings(QString name, QString background, ScreenType type)
{
	QList<LineText> lines;
	LineText text;

	int totalLines = settings_->beginReadArray("lines");

	for (int j = 0; j < totalLines; j++)
	{
		settings_->setArrayIndex(j);

		QString linesText = settings_->value("text").toString();

		int totalData = settings_->beginReadArray("data");

		QMap<QString, Query> queryMap;

		for (int k = 0; k < totalData; k++)
		{
			settings_->setArrayIndex(k);

			QString key = settings_->value("key").toString();

			Query query;

			query.system = Defines::translateMonitorSystemEnum(settings_->value("system").toString());
			query.identifier = settings_->value("id").toString();
			query.name = settings_->value("name").toString();
			query.value = Defines::translateQueryValueEnum(settings_->value("value").toString());
			query.precision = settings_->value("precision").toInt();
            query.hardware = settings_->value("hardware").toString();
            query.unit = settings_->value("unit").toString();
            query.field = settings_->value("field").toString();

			queryMap.insert(key, query);
		}

		text.queryMap = queryMap;
		text.text = linesText;

		lines.append(text);

		settings_->endArray();
	}

        createNormalScreen(name, background, type, lines);

	settings_->endArray();
}

void HwaSettings::loadGraphScreenSettings(QString name, QString background, ScreenType type)
{
	QList<GraphLine> graphData;

	GraphSettings graphSetting;

	graphSetting.addTitle = settings_->value("AddTitle").toBool();
	graphSetting.range = settings_->value("Range").toInt();
	graphSetting.yMinRange = settings_->value("YMinRange").toInt();
	graphSetting.yMaxRange = settings_->value("YMaxRange").toInt();
	graphSetting.yAutoRange = settings_->value("YAutoRange").toBool();

	QColor fontcolor;
	fontcolor.setRed(settings_->value("TitleColorRed").toInt());
	fontcolor.setBlue(settings_->value("TitleColorBlue").toInt());
	fontcolor.setGreen(settings_->value("TitleColorGreen").toInt());

	QFont titleFont;
	titleFont.setPointSize(settings_->value("FontSize").toInt());
	titleFont.setFamily(settings_->value("FontFamily").toString());

	graphSetting.titleColor = fontcolor;
	graphSetting.titleFont = titleFont;

	int graphCount = settings_->beginReadArray("graphData");

	for (int i = 0; i < graphCount; i++)
	{
		settings_->setArrayIndex(i);

		GraphLine line;
		Query query;
		QColor color;

		line.text = settings_->value("Text").toString();

		query.system = Defines::translateMonitorSystemEnum(settings_->value("system").toString());
		query.identifier = settings_->value("id").toString();
		query.name = settings_->value("name").toString();
		query.value = Defines::translateQueryValueEnum(settings_->value("value").toString());
		query.precision = settings_->value("precision").toInt();

		line.query = query;

		color.setRed(settings_->value("ColorRed").toInt());
		color.setGreen(settings_->value("ColorGreen").toInt());
		color.setBlue(settings_->value("ColorBlue").toInt());

		line.color = color;

		graphData.append(line);
	}

	settings_->endArray();

        creategraphScreen(name, background, type, graphData, graphSetting);
}



void HwaSettings::loadScreenOrder()
{
        changeScreenOrder(loadMainScreenOrder(), loadSubScreenOrder());
}

void HwaSettings::changeScreenOrder(QList<QString> mainOrder, QMap<QString, QList<QString>> subOrder)
{
        mainOrder_.clear();
        subOrder_.clear();

        for (QString page : mainOrder)
        {
                mainOrder_.append(getScreenData(page));
        }

        QMap<QString, QList<Screen *>> subScreenOrder;

        QMap<QString, QList<QString>>::const_iterator i = subOrder.constBegin();
        while (i != subOrder.constEnd())
        {
                QList<QString> oldList = i.value();
                QList<Screen*> newList;

                for (QString page : oldList)
                {
                        newList.append(getScreenData(page));
                }

                subOrder_.insert(i.key(), newList);
                ++i;
        }
}

QList<QString> HwaSettings::loadMainScreenOrder()
{
	QList<QString> mainList;

	int size = settings_->beginReadArray("MainOrder");

	for (int i = 0; i < size; i++)
	{
		settings_->setArrayIndex(i);

		mainList.append(settings_->value("Page").toString());
	}

	settings_->endArray();

	return mainList;
}

QMap<QString, QList<QString>> HwaSettings::loadSubScreenOrder()
{
	QMap<QString, QList<QString>> subList;

	int size = settings_->beginReadArray("SubOrder");

	for (int i = 0; i < size; i++)
	{
		settings_->setArrayIndex(i);

		QString mainPage = settings_->value("MainPage").toString();

		QList<QString> subSubList;

		int subSize = settings_->beginReadArray("SubSubOrder");

		for (int j = 0; j < subSize; j++)
		{
			settings_->setArrayIndex(j);

			subSubList.append(settings_->value("Page").toString());
		}

		settings_->endArray();

		subList.insert(mainPage, subSubList);
	}
	settings_->endArray();

	return subList;
}


void HwaSettings::loadGeneralSettings()
{
	generalSettings_.temperature = Defines::translateTemperatureEnum(settings_->value("General/Temperature").toString());
	generalSettings_.autoStart = settings_->value("General/AutoStart").toBool();
	generalSettings_.language = settings_->value("General/Language").toString();

    InfluxDbSettings influxSettings;

    influxSettings.port =  settings_->value("Influx/influxPort").toInt();
    influxSettings.username = settings_->value("Influx/influxUsername").toString();
    influxSettings.password = settings_->value("Influx/influxPassword").toString();
    influxSettings.hostname = settings_->value("Influx/influxHostname").toString();
    influxSettings.database = settings_->value("Influx/influxDatabase").toString();

    generalSettings_.influxDbSettings = influxSettings;
}

void HwaSettings::creategraphScreen(QString name, QString background, ScreenType type, QList<GraphLine> graphData, GraphSettings settings)
{
        GraphScreen * screen = new GraphScreen(name);

        screen->setBackground(background);

        screen->setData(graphData);

        screen->setSettings(settings);

        screenList_.append(screen);
}

void HwaSettings::createNormalScreen(QString name, QString background, ScreenType type, QList<LineText> lines)
{
    NormalScreen * screen = new NormalScreen(name);
        screen->setBackground(background);
        screen->setData(lines);

        screenList_.append(screen);
}

QList<CustomSettings> HwaSettings::loadCustomSettings()
{
	int size = settings_->beginReadArray("CustomSettings");

	QList<CustomSettings> customList;

	for (int i = 0; i < size; i++)
	{
		settings_->setArrayIndex(i);

		CustomSettings custom;

		custom.aligment = Defines::translateAligmentEnum(settings_->value("Aligment").toString());
		
		custom.textScrolling = settings_->value("Scrolling").toBool();
		custom.lineSpacing = settings_->value("LineSpacing").toInt();

		QFont font;
		font.setFamily(settings_->value("FontFamily").toString());
		font.setPointSize(settings_->value("FontSize").toInt());
		font.setBold(settings_->value("FontBold").toBool());
		font.setItalic(settings_->value("FontItalic").toBool());

		QColor color;
		color.setRed(settings_->value("FontColorRed").toInt());
		color.setBlue(settings_->value("FontColorBlue").toInt());
		color.setGreen(settings_->value("FontColorGreen").toInt());
		
		custom.font = font;
		custom.fontColor = color;

		customList.append(custom);
	}

	settings_->endArray();

	return customList;
}

QString HwaSettings::getLanguage()
{
	return generalSettings_.language;
}


InfluxDbSettings HwaSettings::getInfluxSettings()
{
    return generalSettings_.influxDbSettings;
}

QVector<Screen*> HwaSettings::getScreenList()
{
    return screenList_;
}

QList<LineText> HwaSettings::optimizeData(QStringList lines, QMap<QString, Query> dataList)
{
        QList<LineText> data;

        for (QString line : lines)
        {
                QMap<QString, Query> optimizedData;

                QMap<QString, Query>::const_iterator i = dataList.constBegin();

                while (i != dataList.constEnd())
                {
                        if (line.contains(i.key()))
                        {
                                optimizedData.insert(i.key(), i.value());
                        }
                        ++i;
                }
                LineText text;
                text.text = line;
                text.queryMap = optimizedData;

                data.append(text);
        }

        return data;
}

QList<LineText> HwaSettings::optimizeLines(QList<LineText> lines)
{
        while (!lines.isEmpty() && lines.last().text.isEmpty())
        {
                lines.removeLast();
        }

        return lines;
}

QList<Screen *> HwaSettings::getMainOrder()
{
    return mainOrder_;
}

QMap<QString, QList<Screen *>> HwaSettings::getSubOrder()
{
    return subOrder_;
}
