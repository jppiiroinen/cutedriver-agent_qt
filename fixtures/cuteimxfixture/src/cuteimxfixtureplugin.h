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
#ifndef IMXFIXTUREPLUGIN_H
#define IMXFIXTUREPLUGIN_H

#include <QObject>
#include <QHash>
#include <QTime>
#include <QHash>
#include <QPair>
#include <tasqtfixtureplugininterface.h>

#include "imxgpuusage.h"
#include "imxgpuinfo.h"
#include "imxgpuapps.h"
#include "imxgpumem.h"
#include "imxgpuversion.h"

class CuteImxFixturePlugin : public QObject, public TasFixturePluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.github.cutedriver.ImxFixture" FILE "cuteimxfixtureplugin.json")
    Q_INTERFACES(TasFixturePluginInterface)

public:
    CuteImxFixturePlugin(QObject* parent=0);
    ~CuteImxFixturePlugin();
    bool execute(void* objectInstance, QString actionName, QHash<QString, QString> parameters, QString & stdOut);

private:
    ImxGpuUsage*    m_gpuUsage;
    ImxGpuApps*     m_gpuApps;
    ImxGpuInfo*     m_gpuInfo;
    ImxGpuMem*      m_gpuMem;
    ImxGpuVersion*  m_gpuVersion;
    bool m_wasAutomounted;
};

#endif

