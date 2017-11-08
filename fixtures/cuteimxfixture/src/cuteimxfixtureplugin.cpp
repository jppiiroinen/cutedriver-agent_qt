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

#include <QApplication>
#include <QtPlugin>
#include <QDebug>
#include <QHash>

#include <taslogger.h>
#include <testabilityutils.h>
#include <tasqtdatamodel.h>

#include "cutelinuxhelper.h"
#include "cuteimxfixtureplugin.h"


CuteImxFixturePlugin::CuteImxFixturePlugin(QObject* parent)
    : QObject(parent), m_gpuUsage(new ImxGpuUsage()), m_gpuApps(new ImxGpuApps()), m_gpuInfo(new ImxGpuInfo()), m_gpuMem(new ImxGpuMem()), m_gpuVersion(new ImxGpuVersion()), m_wasAutomounted(false)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));

    // if the debugfs does not exist try to mount it
    TasLogger::logger()->debug(QString("%0 automatic mounting is enabled.").arg(Q_FUNC_INFO));

    if (CuteLinuxHelper::isMounted("/sys/kernel/debug", "debugfs") == false) {
        TasLogger::logger()->debug(QString("%0 debugfs was not mounted.").arg(Q_FUNC_INFO));
        if (CuteLinuxHelper::fs_mount("/sys/kernel/debug","none","debugfs") == false) {
            TasLogger::logger()->debug(QString("%0 debugfs couldnt been mounted.").arg(Q_FUNC_INFO));
        } else {
            m_wasAutomounted = true;
            TasLogger::logger()->debug(QString("%0 debugfs has been mounted.").arg(Q_FUNC_INFO));
        }
    } else {
        TasLogger::logger()->debug(QString("%0 debugfs was already mounted.").arg(Q_FUNC_INFO));
    }
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}

CuteImxFixturePlugin::~CuteImxFixturePlugin()
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    if (m_wasAutomounted) {
        CuteLinuxHelper::fs_umount("/sys/kernel/debug");
    }
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}

bool CuteImxFixturePlugin::execute(void * objectInstance, QString actionName, QHash<QString, QString> parameters, QString & stdOut)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    TasLogger::logger()->debug(QString("ActionName: %0").arg(actionName));
    Q_UNUSED(objectInstance)
    Q_UNUSED(parameters)

    bool result = true;

    QStringList actionPath = actionName.split('.');

    if (actionPath.count() != 3) {
        stdOut = "Unknown command '" + actionName + "' for ImxFixture! Should have three parts: [feature].[method].[action]";
        TasLogger::logger()->debug(stdOut);
        TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
        return false;
    }

    QString feature = actionPath.at(0);
    QString service = actionPath.at(1);
    QString cmd = actionPath.at(2);

    if (feature != "gpu") {
        stdOut = "Unknown command '" + cmd + "' for ImxFixture! Should start with 'gpu'.";
        TasLogger::logger()->debug(stdOut);
        TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
        return false;
    }


    // USAGE
    if (service == "usage") {
        if (cmd == "start") {
            m_gpuUsage->startMeasure();
        } else if ( cmd == "collect" || cmd == "stop") {
            m_gpuUsage->stopMeasure();
            m_gpuUsage->sendReport(stdOut);
        } else {
            result = false;
            stdOut = "Unknown command '" + cmd + "' for gpu.usage!";
        }

    // APPS
    } else if (service == "apps") {
        if (cmd == "start") {
            m_gpuApps->startMeasure();
        } else if ( cmd == "collect" || cmd == "stop") {
            m_gpuApps->stopMeasure();
            m_gpuApps->sendReport(stdOut);
        } else {
            result = false;
            stdOut = "Unknown command '" + cmd + "' for gpu.apps!";
        }

    // INFO
    } else if (service == "info") {
        if (cmd == "start") {
            m_gpuInfo->startMeasure();
        } else if ( cmd == "collect" || cmd == "stop") {
            m_gpuInfo->stopMeasure();
            m_gpuInfo->sendReport(stdOut);
        } else {
            result = false;
            stdOut = "Unknown command '" + cmd + "' for gpu.info!";
        }

    // MEM
    } else if (service == "mem") {
        if (cmd == "start") {
            m_gpuMem->startMeasure();
        } else if ( cmd == "collect" || cmd == "stop") {
            m_gpuMem->stopMeasure();
            m_gpuMem->sendReport(stdOut);
        } else {
            result = false;
            stdOut = "Unknown command '" + cmd + "' for gpu.mem!";
        }

    // VERSION
    } else if (service == "version") {
        if (cmd == "start") {
            m_gpuVersion->startMeasure();
        } else if ( cmd == "collect" || cmd == "stop") {
            m_gpuVersion->stopMeasure();
            m_gpuVersion->sendReport(stdOut);
        } else {
            result = false;
            stdOut = "Unknown command '" + cmd + "' for gpu.version!";
        }

    // OTHER
    } else {
        result = false;
        stdOut = "Unknown command '" + cmd + "' for service '" + actionName + "' for ImxFixture!";
    }

    TasLogger::logger()->debug(stdOut);
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
    return result;
}

