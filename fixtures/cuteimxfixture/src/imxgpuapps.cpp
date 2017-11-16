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
#include "imxgpuapps.h"
#include <taslogger.h>
#include <testabilityutils.h>
#include <tasqtdatamodel.h>

ImxGpuApps::ImxGpuApps(QObject *parent) : ImxGpuMeasureInterface(parent)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    m_fileName = "/sys/kernel/debug/gc/clients";
    m_serviceName = "imxGpuApps";
    m_serviceType = "ImxGpuApps";
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}

ImxGpuApps::~ImxGpuApps()
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}

ResultHash ImxGpuApps::parseResult(QString data)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    /*
PID     NAME
------------------------
24      someOtherApp
330     someApp
     */
    ResultHash appData;

    int lineCounter = 0;
    foreach (QString line, data.split("\n")) {
        lineCounter++;
        if (line.trimmed().length() == 0) continue;

        // lets skip header
        if (lineCounter < 3) continue;

        // lets read values
        QList<QString> lineData = line.split(QRegExp("(\\s+|/)"));

        if (lineData.count() == 2) {
            QString key = lineData.at(0).trimmed();
            QString value = lineData.at(1).trimmed();
            appData[key] = value;
        }
    }
    ResultHash retval;
    retval["apps"] = appData;
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
    return retval;
}

bool ImxGpuApps::checkValidity(ResultHash result)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    bool retval = false;
    retval = result.keys().count() > 0;
    if (retval) {
        foreach(QString pid, result.keys()) {
            int value = pid.toInt(&retval);
            Q_UNUSED(value);
            if (!retval) {
                TasLogger::logger()->warning(QString("Invalid pid detected '%0'").arg(pid));
                break;
            }
            QString processName = result[pid].toString();
            retval = (processName.length() != 0);
            if (!retval) {
                TasLogger::logger()->warning(QString("Invalid processName detected '%0' for pid '%1'").arg(processName).arg(pid));
                break;
            }
            TasLogger::logger()->debug(QString("Found: pid='%0' processName='%1'").arg(pid).arg(processName));
        }
    }
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
    return retval;
}

void ImxGpuApps::reportData(TasObjectContainer& container)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));

    TasObject& parent = container.addNewObject("0", "GpuApps", "logData");

    parent.addAttribute("entryCount", QString::number(m_results.count()));
    int idx = 0;
    foreach(ResultHash result, m_results) {
        TasObject& resultObj = parent.addNewObject(QString::number(idx),"LogEntry", "logEntry");
        resultObj.addAttribute("count", QString::number(result["apps"].toHash().keys().count()));
        resultObj.addAttribute("timestamp_start", result["timestamp_start"].toString());
        resultObj.addAttribute("timestamp_end", result["timestamp_end"].toString());
        resultObj.addAttribute("is_valid", result["is_valid"].toString());
        int keyIdx = 0;
        foreach(QString pid, result["apps"].toHash().keys()) {
            TasObject& appObj = resultObj.addNewObject(QString::number(keyIdx++),"DataRow", "dataRow");
            QString processName = result[pid].toString();
            appObj.addAttribute("pid", pid);
            appObj.addAttribute("processName", processName);
            TasLogger::logger()->debug(QString("Adding pid='%0' processName='%1'").arg(pid).arg(processName));
        }
        idx++;
    }
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}
