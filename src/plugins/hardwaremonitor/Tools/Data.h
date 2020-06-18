#ifndef DATA_H
#define DATA_H

#include "../Defines.h"

#include "influxdb.h"

#include "MonitorTool.h"


class Data
{
public:
	static Data * Instance();
	static void removeInstance();

	virtual ~Data();

	QVector<HardwareSensor> getAllData(MonitorSystem system);
	QStringList translateLines(QList<LineText>);
	QList<double> translateLines(QList<GraphLine>);

	HardwareSensor translateLine(Query);

private:

	Data();

	QList<MonitorTool *> tools_;
	GeneralSettings settings_;

	QMap<QString, QString> queryMapData(QMap<QString, Query>);
	MonitorTool * getMonitorTool(MonitorSystem);

	static Data* dataInstance;
	
};
#endif
