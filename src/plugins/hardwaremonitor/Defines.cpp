#include "Defines.h"

Defines::Defines() {}

Defines::~Defines() {}

QString Defines::translateMonitorSystemEnum(MonitorSystem system) {
    switch (system) {
        case MonitorSystem::INFLUXDB:
            return "InfluxDb";
            break;
        case MonitorSystem::COMMAND:
            return "COMMAND";
            break;
        default:
            return "NONE";
    };
}

QString Defines::translateQueryValueEnum(QueryValue value) {
    switch (value) {
        case QueryValue::Current:
            return "Current";
            break;

        case QueryValue::Max:
            return "Max";
            break;
        case QueryValue::Min:
            return "Min";
            break;
    };

    return "";
}

MonitorSystem Defines::translateMonitorSystemEnum(QString string) {
    if (string == "InfluxDb") {
        return MonitorSystem::INFLUXDB;
    } else if (string == "COMMAND") {
        return MonitorSystem::COMMAND;
    }

    return MonitorSystem::NONE;
}

QueryValue Defines::translateQueryValueEnum(QString string) {
    QueryValue value = QueryValue::Current;

    if (string == "Current") {
        value = QueryValue::Current;
    } else if (string == "Max") {
        value = QueryValue::Max;
    } else if (string == "Min") {
        value = QueryValue::Min;
    }

    return value;
}

QString Defines::translateScreenTypeEnum(ScreenType type) {
    if (type == ScreenType::Graph) {
        return "Graph";
    } else if (type == ScreenType::Normal) {
        return "Normal";
    } else if (type == ScreenType::Start) {
        return "Start";
    }

    return "No";
}

ScreenType Defines::translateScreenTypeEnum(QString type) {
    if (type == "Graph") {
        return ScreenType::Graph;
    } else if (type == "Normal") {
        return ScreenType::Normal;
    } else if (type == "Start") {
        return ScreenType::Start;
    }

    return ScreenType::No;
}

QString Defines::translateAligmentEnum(Alignment aligment) {
    if (aligment == Alignment::Center) {
        return "Center";
    } else if (aligment == Alignment::Left) {
        return "Left";
    } else if (aligment == Alignment::Right) {
        return "Right";
    }

    return "Left";
}

Alignment Defines::translateAligmentEnum(QString aligment) {
    if (aligment == "Center") {
        return Alignment::Center;
    } else if (aligment == "Left") {
        return Alignment::Left;
    } else if (aligment == "Right") {
        return Alignment::Right;
    }

    return Alignment::Center;
}

QString Defines::translateTemperatureEnum(TemperatureType temp) {
    switch (temp) {
        case Celsius:
            return "Celsius";
            break;
        case Fahrenheit:
            return "Fahrenheit";
            break;
        default:
            return "Celsius";
            break;
    }
}

TemperatureType Defines::translateTemperatureEnum(QString temp) {
    if (temp == "Celsius") {
        return TemperatureType::Celsius;
    } else if (temp == "Fahrenheit") {
        return TemperatureType::Fahrenheit;
    }

    return TemperatureType::Celsius;
}
