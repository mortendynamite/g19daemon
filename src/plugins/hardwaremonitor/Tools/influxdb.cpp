#include "influxdb.h"
#include "../HwaSettings.h"

InfluxDb::InfluxDb(QObject * parent): QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}


InfluxDb::~InfluxDb()
{
  delete manager;
}


QNetworkReply * InfluxDb::sendQuery(QString query)
{
    QNetworkRequest request(getUrl(query));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply * reply = manager->get(request);

    QTimer timer;
    QEventLoop loop;
    connect( manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));;
    timer.start(60000);
    loop.exec();

    return reply;
}

QVector<HardwareSensor> InfluxDb::getAllSensors()
{
    QVector<HardwareSensor> sensors;

    QNetworkReply * reply = sendQuery("show series");

    QVector<QString> measurements = readValues(reply);

    QString sensorName;

    for(int i = 0; i < measurements.size(); i++)
    {
        QString value = measurements[i];

        QString hardware = value.mid(0, value.indexOf(','));
        QString name = value.mid(value.indexOf(',')+1);

        if(hardware != sensorName)
        {
            sensorName = hardware;

            QNetworkReply * fieldReply = sendQuery("show field keys from " + hardware);

            QVector<QString> fields = readValues(fieldReply);

            for(int j=0; j < fields.size(); j++)
            {
                HardwareSensor sensor;
                sensor.hardware = hardware;
                sensor.name = name;
                sensor.field = fields[j];

                sensors.append(sensor);
            }
        }
    }

    delete reply;
    return sensors;
}

QVector<QString> InfluxDb::readValues(QNetworkReply* reply)
{
    QVector<QString> values;

    if(reply->error() == QNetworkReply::NoError)
    {
        QString response = reply->readAll();

        QJsonDocument jsonResponse = QJsonDocument::fromJson(response.toUtf8());
        QJsonObject root = jsonResponse.object();

        QJsonObject results = root.value(QString("results")).toArray()[0].toObject();

        QJsonArray result = results.value(QString("series")).toArray()[0].toObject().value("values").toArray();

        for(int i = 0; i < result.size(); i++)
        {
            values.append(result[i].toArray()[0].toString());
        }
    }
    else // handle error
    {
      qDebug() << reply->errorString();
    }

  return values;
}



MonitorSystem InfluxDb::getMonitorSystem()
{
    return MonitorSystem::INFLUXDB;
}

HardwareSensor InfluxDb::getData(Query query)
{
    //TODO query to InfluxDB system to get results

    HardwareSensor sensor;

    sensor.hardware =query.hardware;
    sensor.unit = query.unit;
    sensor.field= query.field;
    sensor.name = query.name;
    sensor.value = query.value;
    sensor.field = query.field;

    return sensor;
}

QUrl InfluxDb::getUrl(QString query)
{
    InfluxDbSettings influxSettings = HwaSettings::getInstance()->getInfluxSettings();

    QString url = "http://" + influxSettings.hostname + ":" + QString::number(influxSettings.port) + "/query?db=" + influxSettings.database + "&q=" + QUrl::toPercentEncoding(query);

    if(!influxSettings.username.isEmpty() && !influxSettings.password.isEmpty())
    {
        url += "&u=" + influxSettings.username + "&p=" + influxSettings.password;
    }

    return QUrl(url);
}
