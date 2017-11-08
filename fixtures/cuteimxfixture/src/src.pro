############################################################################
##
## Author(s): Juhapekka Piiroinen <juhapekka.piiroinen@link-motion.com>
##
## Copyright (C) 2017 Link Motion Oy.
## All rights reserved.
## Contact: Link Motion Oy (info@link-motion.com)
##
## This file is part of cuTeDriver Qt Agent
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License version 2.1 as published by the Free Software Foundation
## and appearing in the file LICENSE.LGPL included in the packaging
## of this file.
##
############################################################################

TEMPLATE = lib
TARGET = cuteimxfixture
CONFIG += plugin
QT += xml widgets quick qml


include(../../../tasbase.pri)

target.path = $$TAS_TARGET_PLUGIN/tasfixtures

DEPENDPATH += .
INCLUDEPATH += . ../../../tascore/corelib

# Input
HEADERS += \
    imxgpumeasureinterface.h \
    imxgpuusage.h \
    cuteimxfixtureplugin.h \
    cutelinuxhelper.h \
    imxgpuapps.h \
    imxgpumem.h \
    imxgpuinfo.h \
    imxgpuversion.h
SOURCES += \
    imxgpumeasureinterface.cpp \
    imxgpuusage.cpp \
    cuteimxfixtureplugin.cpp \
    cutelinuxhelper.cpp \
    imxgpuapps.cpp \
    imxgpumem.cpp \
    imxgpuinfo.cpp \
    imxgpuversion.cpp
OTHER_FILES +=

DESTDIR = lib

INSTALLS += target

LIBS += -L../../../tascore/lib/ -lqttestability

DISTFILES += \
    TODO.txt \
    cuteimxfixtureplugin.json
