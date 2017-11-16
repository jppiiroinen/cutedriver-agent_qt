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

QT += testlib
QT += xml
QT += xmlpatterns

include(../../../tasbase.pri)
QT += gui
QT += core

DEPENDPATH += ../src
INCLUDEPATH += . ../../../tascore/corelib
LIBS += -L../../../tascore/lib/ -lqttestability -L../src/lib/ -lcuteimxfixture

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle
TEMPLATE = app

# enable 'make check'
check.commands = LD_LIBRARY_PATH=../src/lib:$$LD_LIBRARY_PATH

OTHER_FILES += $$files(data/*) $$files(xml/*)

SOURCES += \
    tst_imxfixture.cpp \
    ../../../tests/testhelper.cpp

HEADERS += \
    ../../../tests/testhelper.h
