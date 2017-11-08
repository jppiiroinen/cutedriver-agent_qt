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
#ifndef CUTELINUXHELPER_H
#define CUTELINUXHELPER_H

#include <QObject>

class CuteLinuxHelper : public QObject
{
    Q_OBJECT

public:
    explicit CuteLinuxHelper(QObject *parent = 0);

    static bool isMounted(QString mountPoint, QString fileSystem);
    static bool fs_mount(QString mountPoint, QString target, QString filesystem);
    static bool fs_umount(QString mountPoint);

    static QByteArray readFile(QString fileName);
    static QByteArray username();

};

#endif // CUTELINUXHELPER_H
