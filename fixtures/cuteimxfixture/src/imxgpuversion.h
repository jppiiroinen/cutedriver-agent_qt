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
#ifndef IMXGPUVERSION_H
#define IMXGPUVERSION_H

#include <QObject>
#include <QHash>
#include <tasqtdatamodel.h>
#include "imxgpumeasureinterface.h"

class ImxGpuVersion : public ImxGpuMeasureInterface
{
public:
    explicit ImxGpuVersion(QObject *parent = 0);
    ~ImxGpuVersion();

    void reportData(TasObjectContainer& container);
    QHash<QString, QVariant> parseResult(QString data);
    bool checkValidity(QHash<QString, QVariant> result);
};

#endif // IMXGPUVERSION_H
