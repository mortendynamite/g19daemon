#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QVector>
#include <QMap>
#include <QList>
#include <QDebug>
#include "Defines.h"

#include "Screen/Screen.h"
#include "Screen/NormalScreen.h"
#include "Screen/GraphScreen.h"

class HwaSettings
{
public:

    static HwaSettings* getInstance();
	static void releaseResources();


    virtual ~HwaSettings();


	void loadSettings();

	TemperatureType getTemperature();
	QString getLanguage();
    InfluxDbSettings getInfluxSettings();

    QVector<Screen*> getScreenList();
      QList<Screen *> getMainOrder();
      QMap<QString, QList<Screen *>> getSubOrder();
      void removeSettings();

protected:

    HwaSettings();

private:

	void loadGeneralSettings();
	QList<CustomSettings> loadCustomSettings();
	void loadNormalScreenSettings(QString, QString, ScreenType);
	void loadGraphScreenSettings(QString, QString, ScreenType);
	void loadScreenOrder();
	QList<QString> loadMainScreenOrder();
	QMap<QString, QList<QString>> loadSubScreenOrder();

    void createNormalScreen(QString name, QString background, ScreenType type, QList<LineText> lines);
    void creategraphScreen(QString name, QString background, ScreenType type, QList<GraphLine> graphData, GraphSettings settings);
    Screen * getScreenData(QString name);
    void changeScreenOrder(QList<QString> mainOrder, QMap<QString, QList<QString>> subOrder);
    QList<LineText> optimizeData(QStringList lines, QMap<QString, Query> dataList);
    QList<LineText> optimizeLines(QList<LineText>);
	QSettings * settings_;

    static HwaSettings* singleton_;

	GeneralSettings generalSettings_;
    QVector<Screen*> screenList_;
    QList<Screen *> mainOrder_;
    QMap<QString, QList<Screen *>> subOrder_;

};

#endif
