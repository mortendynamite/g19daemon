#ifndef MONITORTOOL_H
#define MONITORTOOL_H

#include "../Defines.h"


class MonitorTool
{
public:

	MonitorTool();

	virtual ~MonitorTool();

    virtual QVector<Query> getAllSensors();
	virtual MonitorSystem getMonitorSystem();
    virtual double getData(Query);

private:

};
#endif
