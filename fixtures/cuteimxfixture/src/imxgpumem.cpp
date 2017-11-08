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
#include "imxgpumem.h"
#include <taslogger.h>
#include <testabilityutils.h>
#include <tasqtdatamodel.h>

ImxGpuMem::ImxGpuMem(QObject *parent) : ImxGpuMeasureInterface(parent)
{
    TasLogger::logger()->debug(Q_FUNC_INFO);
    m_fileName = "/sys/kernel/debug/gc/meminfo";
    m_serviceName = "imxGpuMem";
    m_serviceType = "ImxGpuMem";
}

ImxGpuMem::~ImxGpuMem()
{
    TasLogger::logger()->debug(Q_FUNC_INFO);
}

QHash<QString, QVariant> ImxGpuMem::parseResult(QString data)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    /*
VIDEO MEMORY:
    gcvPOOL_SYSTEM:
        Free  :  109143604 B
        Used  :   25074124 B
        Total :  134217728 B
    gcvPOOL_CONTIGUOUS:
        Used  :          0 B
    gcvPOOL_VIRTUAL:
        Used  :          0 B

NON PAGED MEMORY:
    Used  :          0 B
     */
    QHash<QString, QVariant> retval;

    QString mainMemory;
    QString currentMemType;
    foreach (QString line, data.split("\n")) {
        if (line.trimmed().length() == 0) continue;

        // lets read values
        if (line.trimmed() == "VIDEO MEMORY:") {
            mainMemory = "VIDEOMEMORY";
            continue;
        } else if (line.trimmed() == "NON PAGED MEMORY:") {
            mainMemory = "NONPAGEDMEMORY";
            continue;
        }

        // lets parse video memory related values
        if (mainMemory == "VIDEOMEMORY") {
            if (line.trimmed() == "gcvPOOL_SYSTEM:") {
                currentMemType = "gcvPOOL_SYSTEM";
                continue;
            } else if (line.trimmed() == "gcvPOOL_CONTIGUOUS:") {
                currentMemType = "gcvPOOL_CONTIGUOUS";
                continue;
            } else if (line.trimmed() == "gcvPOOL_VIRTUAL:") {
                currentMemType = "gcvPOOL_VIRTUAL";
                continue;
            }
        }

        QList<QString> lineData = line.split(':');
        if (lineData.count() == 2) {
            QString key = lineData.at(0).trimmed();
            QString value = lineData.at(1).trimmed();
            value = value.replace(QRegularExpression("[A-Z]*"),"").trimmed();
            QString retval_key = QString("%0_%1_%2").arg(mainMemory.toLower()).arg(currentMemType.toLower()).arg(key.toLower());
            TasLogger::logger()->debug(QString("Adding '%0' = '%1'.").arg(retval_key).arg(value));
            bool ok = false;
            retval[retval_key] = value.toLongLong(&ok);
            if (!ok) {
                retval[retval_key] = value;
            }
        } else {
            currentMemType.clear();
        }
    }
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));

    return retval;
}

bool ImxGpuMem::checkValidity(QHash<QString, QVariant> result)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    bool retval = false;
    retval = result.count() > 0;
    if (!retval) {
        TasLogger::logger()->warning("no results");
    } else {
        QStringList fields;
        fields << "videomemory_gcvpool_system_used"
               << "nonpagedmemory_gcvpool_virtual_used"
               << "videomemory_gcvpool_contiguous_used"
               << "videomemory_gcvpool_system_free"
               << "videomemory_gcvpool_virtual_used"
               << "videomemory_gcvpool_system_total";
        foreach (QString field, fields) {
            retval = result.keys().indexOf(field) > -1;
            if (!retval) {
                TasLogger::logger()->warning(QString("Missing '%0' field.").arg(field));
                break;
            }
        }
    }

    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
    return retval;
}

void ImxGpuMem::reportData(TasObjectContainer& container)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    TasObject& parent = container.addNewObject("0", "GpuMem", "logData");

    parent.addAttribute("isValid", m_isValid);
    parent.addAttribute("entryCount", "1");

    TasObject& obj = parent.addNewObject(QString::number(0),"LogEntry", "logEntry");
    foreach(QString key, m_lastResult.keys()) {
        QString value = m_lastResult[key].toString();
        obj.addAttribute(key, value);
        TasLogger::logger()->debug(QString("Adding %0='%1'").arg(key).arg(value));
    }

    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}
