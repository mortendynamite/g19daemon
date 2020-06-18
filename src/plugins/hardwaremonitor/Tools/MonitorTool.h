#ifndef MONITORTOOL_H
#define MONITORTOOL_H

#include "../Defines.h"


class MonitorTool
{
public:

	MonitorTool();

	virtual ~MonitorTool();

	virtual QVector<HardwareSensor> getAllSensors();
	virtual MonitorSystem getMonitorSystem();
	virtual HardwareSensor getData(Query);

private:

};
#endif
