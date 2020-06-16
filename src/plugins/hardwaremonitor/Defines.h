#ifndef DEFINES_H
#define DEFINES_H


#include <qdir.h>
#include <qpixmap.h>
#include <QVector>
#include <qactiongroup.h>
#include <qobject.h>
#include <qtimer.h>
#include <qfont.h>
#include <qmap.h>
#include <QDir>

enum KeyboardTypes {Color, Monochrome, None};
enum ScreenType {Normal, Graph, Start, Legend, No};
enum MonitorSystem { INFLUXDB, NONE };
enum Page{ Page_Intro, Page_Background, Page_Type, Page_Data, Page_LineEdit, Page_GraphEdit, Page_Customize };
enum TemperatureType { Celsius, Fahrenheit };
enum Alignment { Left, Center, Right };
enum PageDirection {Next, Previous, Up, Down};

//TODO remove Name & hardware -> users  can manually type this in the lineEditor
enum QueryValue {Name, Current, Max, Min, Hardware};

struct Query{
	MonitorSystem system;
	QString identifier;
	QString name;
	QueryValue value;
	int precision;
        QString hardware;
        QString field;
        QString unit;

	bool operator == (const Query& rhs)
	{
            return system == rhs.system &&
                    identifier == rhs.identifier &&
                    name == rhs.name &&
                    value == rhs.value &&
                    precision == rhs.precision &&
                    hardware == rhs.hardware &&
                    field == rhs.hardware;
    }
};

//TODO remove HardwareSensor OR Query

struct HardwareSensor{
        QString id;
        QString name;
        double value;
        double max;
        double min;
        QString unit;
        QString hardware;
        QString field;
};

struct LineText{
	QString text;
	QMap<QString, Query> queryMap;
    #ifdef _WIN32
    HANDLE textHandle;
    #endif
};

struct GraphLine{
	QString text;
	Query query;
	QColor color;
};

struct GraphSettings{
	int range;
	int yMinRange;
	int yMaxRange;
	bool yAutoRange;
	bool addTitle;
	QFont titleFont;
	QColor titleColor;
};

struct AppletFont{
	QFont name;
	int height;
	double lineSpace;
	QColor color;
};

struct CustomSettings
{
	Alignment aligment;
	bool textScrolling;
	QFont font;
	QColor fontColor;
	int lineSpacing;
};


struct InfluxDbSettings
{
    QString hostname;
    int port;
    QString username;
    QString password;
    QString database;
};

struct GeneralSettings
{
	TemperatureType temperature;
	bool autoStart;
	QString language;
    InfluxDbSettings influxDbSettings;
};

const QChar degreeChar(0260);

//-----------------------------------------------------------------
// Defines Class
//-----------------------------------------------------------------
class Defines
{
	public:
		static QString translateMonitorSystemEnum(MonitorSystem);
		static QString translateQueryValueEnum(QueryValue);
		static QString translateScreenTypeEnum(ScreenType);

		static MonitorSystem translateMonitorSystemEnum(QString);
		static QueryValue translateQueryValueEnum(QString);
		static ScreenType translateScreenTypeEnum(QString);

		static QString translateAligmentEnum(Alignment);
		static Alignment translateAligmentEnum(QString);

		static QString translateTemperatureEnum(TemperatureType);
		static TemperatureType translateTemperatureEnum(QString);

	private:
		//---------------------------
		// Constructor(s)
		//---------------------------
		Defines();
		//---------------------------
		// Destructor
		//---------------------------
		~Defines();
};
#endif
