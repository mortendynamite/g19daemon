#ifndef INFLUXDB_H
#define INFLUXDB_H

#include "MonitorTool.h"
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>
#include <QMap>

class InfluxDb : private QObject, public MonitorTool {
Q_OBJECT

public:
    InfluxDb(QObject *parent = nullptr);

    ~InfluxDb();

    QVector<Query> getAllSensors();

    MonitorSystem getMonitorSystem();

    double getData(Query query);

private:
    QNetworkAccessManager *manager;

    QUrl getUrl(QString query);

    QVector<QString> readValues(QNetworkReply *);

    double readQueryValue(QNetworkReply *);

    QNetworkReply *sendQuery(QString);

    QMap<QString, QString> parseQueryArguments(Query query);
};

#endif // INFLUXDB_H
