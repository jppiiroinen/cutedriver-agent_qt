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
#include "imxgpumeasureinterface.h"
#include "cutelinuxhelper.h"
#include <taslogger.h>
#include <testabilityutils.h>
#include <tasqtdatamodel.h>

ImxGpuMeasureInterface::ImxGpuMeasureInterface(QObject *parent) : QObject(parent)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    cancelled = false;
    m_interval = -1;
    m_serviceName = "notSet";
    m_serviceType = "NotSet";
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}

ImxGpuMeasureInterface::~ImxGpuMeasureInterface() {
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}

void ImxGpuMeasureInterface::startMeasure(int interval)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    m_interval = interval;
    m_lastResult = processResult();
    m_lastResult["is_valid"] = false;
    m_lastResult["timestamp_end"] = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if (m_interval > 0) {
        TasLogger::logger()->debug(QString("Interval measuring activated with interval '%0'").arg(m_interval));
        QTimer::singleShot(m_interval, this, SLOT(stopMeasure()));
    }
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}

ResultHash ImxGpuMeasureInterface::stopMeasure()
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));

    if (cancelled) { return ResultHash(); }

    // store previous result for validity check
    m_previousResult = m_lastResult;

    // make the result available for later use.
    m_lastResult = processResult();
    m_lastResult["timestamp_start"] = m_previousResult["timestamp_end"];
    m_lastResult["timestamp_end"] = QDateTime::currentDateTime().toMSecsSinceEpoch();
    m_lastResult["is_valid"] = checkValidity(m_lastResult);

    // store the measurement result for reporting in interval measures
    m_results.append(m_lastResult);

    if (m_interval > 0) {
        TasLogger::logger()->debug(QString("Triggering next interval in '%0'").arg(m_interval));
        startMeasure(m_interval);
        QTimer::singleShot(m_interval, this, SLOT(stopMeasure()));
    }

    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
    return m_lastResult;
}


ResultHash ImxGpuMeasureInterface::processResult()
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));

    if (QProcess::systemEnvironment().contains("CUTEDRIVER_DEBUG")) {
        m_fileName.replace("/sys/kernel/debug/gc/","/logs/testability/debug/imxfixture/");
    }

    QString data = CuteLinuxHelper::readFile(m_fileName);

    TasLogger::logger()->debug(QString("Reading file %0: %1").arg(m_fileName).arg(data));

    ResultHash retval = parseResult(data);
    TasLogger::logger()->debug(retval.keys().join(", "));
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
    return retval;
}

void ImxGpuMeasureInterface::sendReport(QString& stdOut)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));

    TasDataModel* model = new TasDataModel();
    QString qtVersion = "Qt" + QString(qVersion());
    TasObjectContainer& container = model->addNewObjectContainer(1, qtVersion, "qt");

    reportData(container);

    // create xml and write result
    QByteArray xml;
    model->serializeModel(xml);
    stdOut.append(QString::fromUtf8(xml.data()));
    delete model;
    model = Q_NULLPTR;

    // print the xml data to logs for easier debugging
    TasLogger::logger()->debug(stdOut);

    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}
