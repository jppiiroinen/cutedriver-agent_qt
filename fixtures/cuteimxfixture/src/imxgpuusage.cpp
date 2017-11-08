/***************************************************************************
**
** Author(s): Juhapekka Piiroinen <juhapekka.piiroinen@link-motion.com>
**
** Copyright (C) 2017 Link Motion Oy.
** All rights reserved.
** Contact: Link Motion Oy (info@link-motion.com)
**
** This file is part of cuTeDriver Qt Agent
**
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/
#include "imxgpuusage.h"
#include "cutelinuxhelper.h"
#include <taslogger.h>
#include <testabilityutils.h>
#include <tasqtdatamodel.h>

ImxGpuUsage::ImxGpuUsage(QObject *parent) : ImxGpuMeasureInterface(parent)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    m_fileName = "/sys/kernel/debug/gc/idle";
    m_serviceName = "imxGpuUsage";
    m_serviceType = "ImxGpuUsage";
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}

ImxGpuUsage::~ImxGpuUsage()
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}

QHash<QString, QVariant> ImxGpuUsage::parseResult(QString data)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    /*
    Start:   2860731999 ns
    End:     66276078984233 ns
    On:      44108974309 ns
    Off:     65719908311167 ns
    Idle:    0 ns
    Suspend: 509200966758 ns
    */
    QHash<QString, QVariant> retval;

    foreach (QString line, data.split("\n")) {
        if (line.trimmed().count() == 0) continue;
        TasLogger::logger()->debug(line);
        QList<QString> lineData = line.split(':');
        if (lineData.count() == 2) {
            QString key = lineData.at(0).trimmed().toLower();
            QString value = lineData.at(1).trimmed();
            value = value.replace(QRegularExpression("[a-z]*"),"").trimmed();
            TasLogger::logger()->debug(QString("Adding '%0' = '%1'.").arg(key).arg(value));
            bool ok = false;
            retval[key] = value.toLongLong(&ok);
            if (!ok) {
                retval[key] = value;
            }
        }
    }
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));

    return retval;
}

bool ImxGpuUsage::checkValidity(QHash<QString, QVariant> result)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    bool retval = true;

    retval = result.keys().count() > 0;

    if (!retval) {
        TasLogger::logger()->warning("No results");
    } else {

        // mark that data as the valid measurement data
        QStringList fields;
        fields << "start" << "end" << "on" << "off" << "idle" << "suspend";
        TasLogger::logger()->debug(result.keys().join(", "));
        foreach (QString field, fields) {
            retval = result.keys().indexOf(field) > -1;
            if (retval == false) {
                TasLogger::logger()->warning(QString("Missing field '%0'").arg(field));
                break;
            }
        }

        if (retval && !m_previousResult.isEmpty()) {
            retval = result["start"] == m_previousResult["end"];
            if (retval == false) {
                TasLogger::logger()->warning(QString("Measurement data is compromised. 'start' field (%0) is not equal to 'end' field (%1) in previous measurement.").arg(result["start"].toString()).arg(m_previousResult["end"].toString()));
            }
        }
    }
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
    return retval;
}

void ImxGpuUsage::reportData(TasObjectContainer& container)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    TasObject& parent = container.addNewObject("0", "GpuUsage", "logData");

    parent.addAttribute("isValid", m_isValid);
    parent.addAttribute("entryCount", "1");

    TasObject& obj = parent.addNewObject(QString::number(0),"LogEntry", "logEntry");

    qlonglong start = m_lastResult["start"].toLongLong();
    qlonglong end = m_lastResult["end"].toLongLong();
    qlonglong on = m_lastResult["on"].toLongLong();
    qlonglong off = m_lastResult["off"].toLongLong();
    qlonglong idle = m_lastResult["idle"].toLongLong();
    qlonglong suspend = m_lastResult["suspend"].toLongLong();

    // a quick validity check
    qlonglong total_time = end - start;
    qlonglong check = on + off + idle + suspend;
    if (total_time != check) {
        TasLogger::logger()->warning("data does not seem legit.");
        m_isValid = false;
    }
    obj.addAttribute("isValid", m_isValid);

    // add fields and calculate percentages when applicable
    QStringList fields;
    fields << "start" << "end" << "on" << "off" << "idle" << "suspend";
    foreach(QString field, fields) {
        QString value = m_lastResult[field].toString();
        obj.addAttribute(field, value);
        TasLogger::logger()->debug(QString("Adding %0='%1'").arg(field).arg(value));

        if (field != "start" && field != "end") {
            // calculate percentages
            qlonglong value = m_lastResult[field].toLongLong();
            double percentage = 0;
            if (value > 0) {
                percentage = double(value) / total_time * 100;
            }
            obj.addAttribute(QString("%0_percentage").arg(field), percentage);
            TasLogger::logger()->debug(QString("Adding %0_percentage='%1'").arg(field).arg(percentage));
        }
    }
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}
