#ifndef COMMANDTOOL_H
#define COMMANDTOOL_H

#include "MonitorTool.h"
#include <QDebug>
#include <QProcess>
#include <QStringList>
#include <QVector>
#include <QLocale>


class CommandTool : public MonitorTool
{
public:
    CommandTool();

    //---------------------------
    // Destructor
    //---------------------------
    ~CommandTool();

    //---------------------------
    // General Methods
    //---------------------------
    QVector<Query> getAllSensors();
    MonitorSystem getMonitorSystem();
    double getData(Query);
};

#endif // COMMANDTOOL_H
