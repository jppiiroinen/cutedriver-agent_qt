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
#include "imxgpuinfo.h"
#include <taslogger.h>
#include <testabilityutils.h>
#include <tasqtdatamodel.h>
#include <QHash>

ImxGpuInfo::ImxGpuInfo(QObject *parent) : ImxGpuMeasureInterface(parent)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    m_fileName = "/sys/kernel/debug/gc/info";
    m_serviceName = "imxGpuInfo";
    m_serviceType = "ImxGpuInfo";
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}

ImxGpuInfo::~ImxGpuInfo()
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}

ResultHash ImxGpuInfo::parseResult(QString data)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    /*
    bash-4.3# cat info
    gpu      : 0
    model    : 3000
    revision : 5450

    gpu      : 1
    model    :  320
    revision : 5303

    gpu      : 2
    model    :  355
    revision : 1216
    */
    ResultHash retval;
    QString currentGpu;
    QHash<QString, QVariant> currentGpuInfo;
    int gpuCounter = 0;
    foreach (QString line, data.split("\n")) {
        // lets read values
        QList<QString> lineData = line.split(':');

        if (lineData.count() == 2) {
            QString key = lineData.at(0).trimmed();
            QString value = lineData.at(1).trimmed();

            if (key == "gpu") {
                gpuCounter++;
                currentGpu = value;
                TasLogger::logger()->debug(QString("Found gpu '%0'").arg(currentGpu));
            } else {
                ResultHash data = retval[currentGpu].toHash();
                data[key] = value;
                retval[currentGpu] = data;
                TasLogger::logger()->debug(QString("Found '%0'"
                                           " and '%1' for '%2'").arg(key).arg(value).arg(currentGpu));
            }
        }
    }

    retval["count"] = gpuCounter;
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
    return retval;
}

bool ImxGpuInfo::checkValidity(ResultHash result)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    bool retval = false;
    retval = result.keys().count() > 0 && result.keys().indexOf("count") > -1;
    if (retval) {
        int count = result["count"].toInt();

        QStringList fields;
        fields << "model" << "revision";
        for(int gpu=0; gpu < count; gpu++) {
            QString key = QString::number(gpu);
            ResultHash gpuData = result[key].toHash();
            foreach(QString field, fields) {
                retval = gpuData.keys().indexOf(field) > -1;
                if (!retval) {
                    TasLogger::logger()->warning(QString("Missing '%0' field for gpu '%1'").arg(key).arg(gpu));
                    break;
                }
                retval = gpuData[field].toString().length() > 0;
                if (!retval) {
                    TasLogger::logger()->warning(QString("Invalid '%0' field for gpu '%1'").arg(key).arg(gpu));
                    break;
                }
            }
        }

    } else {
        TasLogger::logger()->warning(QString("No gpu info."));
    }
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
    return retval;
}

void ImxGpuInfo::reportData(TasObjectContainer& container)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));

    TasObject& parent = container.addNewObject("0", "GpuInfo", "logData");

    parent.addAttribute("entryCount", QString::number(m_results.count()));
    int idx = 0;
    foreach(ResultHash result, m_results) {
        TasObject& resultObj = parent.addNewObject(QString::number(idx),"LogEntry", "logEntry");
        resultObj.addAttribute("count", result["count"].toInt());
        resultObj.addAttribute("timestamp_start", result["timestamp_start"].toString());
        resultObj.addAttribute("timestamp_end", result["timestamp_end"].toString());
        resultObj.addAttribute("is_valid", result["is_valid"].toString());
        TasLogger::logger()->debug(QString("Found '%0' results.").arg(result.keys().count()));
        for(int gpuIdx=0; gpuIdx < result["count"].toInt(); gpuIdx++) {
            QString gpu = QString::number(gpuIdx);
            TasObject& appObj = resultObj.addNewObject(gpu,"DataRow", "dataRow");
            ResultHash gpuResult = result[gpu].toHash();
            QString model = gpuResult["model"].toString();
            QString revision = gpuResult["revision"].toString();
            appObj.addAttribute("gpu", gpu);
            appObj.addAttribute("model", model);
            appObj.addAttribute("revision", revision);
            TasLogger::logger()->debug(QString("Adding gpu='%0' model='%1' revision='%2'").arg(gpu).arg(model).arg(revision));

        }
        idx++;
    }
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}
