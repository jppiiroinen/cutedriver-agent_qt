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

QHash<QString, QVariant> ImxGpuApps::parseResult(QString data)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    /*
PID     NAME
------------------------
24      someOtherApp
330     someApp
     */
    QHash<QString, QVariant> retval;

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
            retval[key] = value;
        }
    }
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
    return retval;
}

bool ImxGpuApps::checkValidity(QHash<QString, QVariant> result)
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

    parent.addAttribute("isValid", m_isValid);
    parent.addAttribute("entryCount", QString::number(m_lastResult.keys().size()));
    for(int i = 0 ; i < m_lastResult.keys().size(); i++ ){
        TasObject& appObj = parent.addNewObject(QString::number(i),"LogEntry", "logEntry");
        QString pid = m_lastResult.keys().at(i);
        QString processName = m_lastResult[pid].toString();
        appObj.addAttribute("pid", pid);
        appObj.addAttribute("processName", processName);
        TasLogger::logger()->debug(QString("Adding pid='%0' processName='%1'").arg(pid).arg(processName));
    }
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}
