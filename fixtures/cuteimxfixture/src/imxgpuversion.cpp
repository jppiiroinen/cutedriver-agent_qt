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
#include "imxgpuversion.h"
#include <taslogger.h>
#include <testabilityutils.h>
#include <tasqtdatamodel.h>

ImxGpuVersion::ImxGpuVersion(QObject *parent) : ImxGpuMeasureInterface(parent)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    m_fileName = "/sys/kernel/debug/gc/version";
    m_serviceName = "imxGpuVersion";
    m_serviceType = "ImxGpuVersion";
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}

ImxGpuVersion::~ImxGpuVersion()
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}

ResultHash ImxGpuVersion::parseResult(QString data)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    /*
5.0.11.p8.41671 built at
Platform path: ../drivers/mxc/gpu-viv/hal/os/linux/kernel/platform/freescale/gc_hal_kernel_platform_imx6q14.c

     */
    ResultHash retval;
    int lineCounter = 0;
    foreach (QString line, data.split("\n")) {
        TasLogger::logger()->debug(line);
        if (line.trimmed().length() == 0) continue;

        lineCounter++;
        switch(lineCounter) {
        case 1:
            retval["version"] = line;
            break;
        default:
            break;
        }
    }
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));

    return retval;
}

bool ImxGpuVersion::checkValidity(ResultHash result)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    bool retval = false;
    retval = result.keys().contains("version");
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
    return retval;
}

void ImxGpuVersion::reportData(TasObjectContainer& container)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    TasObject& parent = container.addNewObject("0", "GpuVersion", "logData");

    parent.addAttribute("entryCount", QString::number(m_results.count()));
    int idx = 0;
    foreach(ResultHash result, m_results) {
        TasObject& obj = parent.addNewObject(QString::number(idx),"LogEntry", "logEntry");
        obj.addAttribute("timestamp_start", result["timestamp_start"].toString());
        obj.addAttribute("timestamp_end", result["timestamp_end"].toString());
        obj.addAttribute("is_valid", result["is_valid"].toString());

        QString field = "version";
        if (result.keys().indexOf(field) > -1) {
            QString value = result[field].toString();
            obj.addAttribute(field,value);
            TasLogger::logger()->debug(QString("Adding %0='%1'").arg(field).arg(value));
        }
        idx++;
    }
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}
