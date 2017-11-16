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
#ifndef IMXGPUINFO_H
#define IMXGPUINFO_H

#include <QObject>
#include <tasqtdatamodel.h>
#include "imxgpumeasureinterface.h"

class ImxGpuInfo : public ImxGpuMeasureInterface
{
public:
    explicit ImxGpuInfo(QObject *parent = 0);
    ~ImxGpuInfo();

    void reportData(TasObjectContainer& container);
    ResultHash parseResult(QString data);
    bool checkValidity(ResultHash result);
};

#endif // IMXGPUINFO_H
