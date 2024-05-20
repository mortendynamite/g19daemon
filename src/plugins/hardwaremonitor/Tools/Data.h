#ifndef DATA_H
#define DATA_H

#include "../Defines.h"

#include "influxdb.h"
#include "commandtool.h"

#include "MonitorTool.h"


class Data {
public:
    static Data *Instance();

    static void removeInstance();

    virtual ~Data();

    QVector<Query> getAllData(MonitorSystem system);

    QStringList translateLines(QList<LineText>);

    QList<double> translateLines(QList<GraphLine>);

    double translateLine(Query);

private:

    Data();

    QList<MonitorTool *> tools_;
    GeneralSettings settings_;

    QMap<QString, QString> queryMapData(QMap<QString, Query>);

    MonitorTool *getMonitorTool(MonitorSystem);

    static Data *dataInstance;

};

#endif
