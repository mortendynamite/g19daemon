#include "MonitorTool.h"


MonitorTool::MonitorTool()
{
	//Nothing to be created
}

MonitorTool::~MonitorTool()
{
	// nothing to destroy
}

QVector<Query> MonitorTool::getAllSensors()
{
    QVector<Query> sensors;

	return sensors;
}

/// <summary>
/// Gets the monitor system.
/// </summary>
/// <returns>MonitorSystem::NONE</returns>
MonitorSystem MonitorTool::getMonitorSystem()
{
	return MonitorSystem::NONE;
}


double MonitorTool::getData(Query query)
{
    return 0;
}
