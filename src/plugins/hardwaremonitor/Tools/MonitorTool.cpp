#include "MonitorTool.h"


MonitorTool::MonitorTool()
{
	//Nothing to be created
}

MonitorTool::~MonitorTool()
{
	// nothing to destroy
}

QVector<HardwareSensor> MonitorTool::getAllSensors()
{
	QVector<HardwareSensor> sensors;

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

/// <summary>
/// Gets the data from the query
/// </summary>
/// <param name="query">The query</param>
/// <returns>Empty HardwareSensor</returns>
HardwareSensor MonitorTool::getData(Query query)
{
	HardwareSensor emptySensor = {0};
	return emptySensor;
}
