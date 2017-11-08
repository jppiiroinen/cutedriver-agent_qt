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
#ifndef IMXGPUMEASUREINTERFACE_H
#define IMXGPUMEASUREINTERFACE_H

#include <QObject>
#include <tasqtdatamodel.h>

class ImxGpuMeasureInterface : public QObject
{
    Q_OBJECT
public:
    explicit ImxGpuMeasureInterface(QObject *parent = 0);
    ~ImxGpuMeasureInterface();

    bool isValid() { return m_isValid; }
    QHash<QString, QVariant> lastResult() { return m_lastResult; }
    void sendReport(QString& stdOut);
    virtual void reportData(TasObjectContainer& container) = 0;
    virtual QHash<QString, QVariant> parseResult(QString data) = 0;
    virtual bool checkValidity(QHash<QString, QVariant> result) = 0;

public slots:
    void startMeasure();
    QHash<QString, QVariant> stopMeasure();

protected:
    QHash<QString, QVariant> processResult();

    QString m_fileName;
    QString m_serviceName;
    QString m_serviceType;
    bool m_isValid;
    QHash<QString, QVariant> m_lastResult;
    QHash<QString, QVariant> m_previousResult;

    friend class TestImxFixture;
};

#endif // IMXGPUMEASUREINTERFACE_H
