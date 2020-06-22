#include "Data.h"

Data * Data::dataInstance = nullptr;

/// <summary>
/// Create only one instance of this class.
/// Will create the object if no exist.
/// </summary>
/// <returns>Data Object</returns>
Data * Data::Instance()
{
	if (dataInstance == nullptr)   // Only allow one instance of class to be generated.
	{
		dataInstance = new Data();
	}
	return dataInstance;
}

/// <summary>
/// Removes the instance.
/// </summary>
void Data::removeInstance()
{
	if (dataInstance != nullptr)
	{
		delete dataInstance;
		dataInstance = nullptr;
	}
}

/// <summary>
/// Initialize some required requirements.
/// </summary>
Data::Data()
{
    MonitorTool * influx = new InfluxDb(nullptr);

    tools_.append(influx);

}

/// <summary>
/// Finalizes an instance of the <see cref="Data"/> class.
/// </summary>
Data::~Data()
{
	for (MonitorTool * monitorSystem : tools_)
	{
		if (monitorSystem != nullptr)
		{
			delete monitorSystem;
			monitorSystem = nullptr;
		}
	}

	tools_.clear();
}

/// <summary>
/// Gets all the sensor data from one specific Monitor system.
/// </summary>
/// <param name="system">The monitor system</param>
/// <returns>A list of all the sensor data from that Monitor system</returns>
QVector<Query> Data::getAllData(MonitorSystem system)
{
    QVector<Query> emptyVector;

	for (MonitorTool * monitorSystem : tools_)
	{
		if (monitorSystem->getMonitorSystem() == system)
		{
			return monitorSystem->getAllSensors();
		}
	}

	return emptyVector;
}

/// <summary>
/// Translates the line text to a queried value.
/// </summary>
/// <param name="lines">The lines</param>
/// <returns>QStringList with all the translated values in it</returns>
QStringList Data::translateLines(QList<LineText> lines)
{
	QStringList returnValue;

	for (LineText text : lines)
	{
		QString temp = text.text;

		QMap<QString, QString> values = queryMapData(text.queryMap);

		QMap<QString, QString>::const_iterator i = values.constBegin();

		while (i != values.constEnd())
		{
			temp = temp.replace(i.key(), i.value());
			++i;
		}

		returnValue.append(temp);
	}

	return returnValue;
}

/// <summary>
/// Translates the graph text to a queried value.
/// </summary>
/// <param name="lines">The lines.</param>
/// <returns>List of the translated values</returns>
QList<double> Data::translateLines(QList<GraphLine> lines)
{
	QList<double> returnValue;

	for (GraphLine line : lines)
	{
        returnValue.append(getMonitorTool(line.query.system)->getData(line.query));
	}

    return returnValue;
}

/// <summary>
/// Queries the sensor data from the map.
/// </summary>
/// <param name="map">The map.</param>
/// <returns>The result of the query of the full map</returns>
QMap<QString, QString> Data::queryMapData(QMap<QString, Query> map)
{
	QMap<QString, QString> returnmap;

	QMap<QString, Query>::const_iterator i = map.constBegin();

	while (i != map.constEnd())
	{
		Query query = i.value();

        returnmap.insert(i.key(), QString::number(getMonitorTool(query.system)->getData(query), 'f', query.precision));

		++i;
	}

	return returnmap;
}

/// <summary>
/// Gets the monitor tool object by the MonitorSystem enum.
/// </summary>
/// <param name="system">The monitorSystem name</param>
/// <returns>The MonitorTool object</returns>
MonitorTool * Data::getMonitorTool(MonitorSystem system)
{
	for (MonitorTool * tool : tools_)
	{
		if (tool->getMonitorSystem() == system)
		{
			return tool;
		}
	}

	return nullptr;
}


/// <summary>
/// Translates the line.
/// </summary>
/// <param name="query">The query</param>
/// <returns>Hardware sensor data</returns>
double Data::translateLine(Query query)
{
	return getMonitorTool(query.system)->getData(query);;
}
