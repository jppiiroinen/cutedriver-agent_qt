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
#include "cutelinuxhelper.h"
#include <errno.h>
#include <mntent.h>
#include <stdio.h>
#include <sys/mount.h>
#include <taslogger.h>
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>

CuteLinuxHelper::CuteLinuxHelper(QObject *parent) : QObject(parent)
{
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
}

bool CuteLinuxHelper::isMounted(QString mountPoint, QString fileSystem) {
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    bool retval = false;
    FILE* fp = setmntent("/etc/mtab", "r");
    struct mntent entry;
    char buf[BUFSIZ];
    while (getmntent_r(fp, &entry, buf, sizeof(buf)) != NULL) {
        if (strcmp(entry.mnt_fsname, fileSystem.toLatin1().data()) == 0 && strcmp(entry.mnt_dir, mountPoint.toLatin1().data()) == 0) {
          retval = true;
          break;
        }
    }
    endmntent(fp);
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
    return retval;
}

bool CuteLinuxHelper::fs_umount(QString mountPoint) {
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    bool retval = umount(mountPoint.toLatin1().data()) != 0;
    TasLogger::logger()->debug(QString(retval));
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
    return retval;
}


bool CuteLinuxHelper::fs_mount(QString mountPoint, QString target, QString filesystem) {
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    int retval = mount(target.toLatin1().data(), mountPoint.toLatin1().data(), filesystem.toLatin1().data(), 0, "");
    if (retval != 0) {
        TasLogger::logger()->debug(QString("%0 %1").arg(Q_FUNC_INFO).arg(strerror(retval)));
    }
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
    return retval == 0;
}

QByteArray CuteLinuxHelper::readFile(QString fileName) {
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    TasLogger::logger()->debug(QString("username: %0").arg(QString::fromLatin1(username())));
    QByteArray retval;
    FILE* fp;
    fp = fopen(fileName.toLatin1().data(), "r");
    if (fp != NULL) {
        char data[100];
        while (fgets(data, 100, fp) != NULL) {
            retval.append(data);
            retval.append("\n");
        }
        fclose(fp);
    } else {
        TasLogger::logger()->warning(QString("Unable to open file: %0").arg(fileName.toLatin1().data()));
    }

    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
    return retval;
}

QByteArray CuteLinuxHelper::username() {
    TasLogger::logger()->debug(QString(">> %0").arg(Q_FUNC_INFO));
    QByteArray retval;
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw)
    {
        retval = pw->pw_name;
    }
    TasLogger::logger()->debug(QString("<< %0").arg(Q_FUNC_INFO));
    return retval;
}
