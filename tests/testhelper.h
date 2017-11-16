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
#ifndef TESTHELPER_H
#define TESTHELPER_H

#include <QtTest>
#include <QtXml>

#define TEST_ENTRY_ATTRIBUTE(item, idx, fieldName, expectedValue) { \
        bool ok = false; \
        QString actualValue; \
        TestHelper::check_entry_attribute(item, idx, fieldName, expectedValue, &ok, &actualValue); \
        QVERIFY2(ok, QString("Invalid attribute '%0' for idx %1 in results, expected '%2' actual was '%3'.").arg(fieldName).arg(idx).arg(expectedValue).arg(actualValue).toLatin1().data()); \
    } \

#define TEST_ENTRY_PROPERTY(item, idx, fieldName, expectedValue) { \
        bool ok = false; \
        QString actualValue; \
        TestHelper::check_entry_property(item, idx, fieldName, expectedValue, &ok, &actualValue); \
        QVERIFY2(ok, QString("Invalid property '%0' for idx %1 in results, expected '%2' actual was '%3'.").arg(fieldName).arg(idx).arg(expectedValue).arg(actualValue).toLatin1().data()); \
    } \


#define TEST_PROPERTY(item, fieldName, expectedValue) { \
        bool ok = false; \
        QString actualValue; \
        TestHelper::check_property(rootItem, fieldName, expectedValue, &ok, &actualValue); \
        QVERIFY2(ok, QString("Invalid property '%0', expected '%1' actual was '%2'.").arg(fieldName).arg(expectedValue).arg(actualValue).toLatin1().data()); \
    } \

#define TEST_ATTRIBUTE(item, fieldName, expectedValue) { \
        bool ok = false; \
        QString actualValue; \
        TestHelper::check_attribute(rootItem, fieldName, expectedValue, &ok, &actualValue); \
        QVERIFY2(ok, QString("Invalid attribute '%0', expected '%1' actual was '%2'.").arg(fieldName).arg(expectedValue).arg(actualValue).toLatin1().data()); \
    } \

class TestHelper : public QObject
{
    Q_OBJECT

public:
    static QString readFile(QString fileName);
    static bool validate_tasmessage(QString xml, QString xsdFile);
    static void get_tasmessage(QString xml, QDomNode* node, bool* ok = NULL);
    static QDomDocument get_xml_document(QString name, QString xml, bool* ok = NULL);
    static void validate_tasmessage_xml_header(QDomDocument doc, bool* ok = NULL);
    static QDomNode get_node_by_name(QDomNode rootItem, QString name);
    static void check_attribute(QDomNode rootItem, QString attribute, QString attributeValue, bool* ok = NULL, QString* retval = NULL);
    static void check_property(QDomNode rootItem, QString propertyname, QString propertyValue, bool* ok = NULL, QString* retval = NULL);
    static void check_property_startsWith(QDomNode rootItem, QString propertyname, QString propertyValue, bool* ok = NULL, QString* retval = NULL);
    static void check_entry_attribute(QDomNode rootItem, int entryIdx, QString attributeName, QString attributeValue, bool* ok = NULL, QString* retval = NULL);
    static QList<QDomNode> get_nodes_by_type(QDomNode rootItem, QString typeName);
    static QString get_property(QDomNode rootItem, QString propertyName);
    static QDomNode get_node_by_id(QDomNode rootItem, int entryId);
    static QDomNode get_datarow_by_id(QDomNode rootItem, int dataRowId);
    static void get_attribute(QDomNode rootItem, QString attribute, QString* value);
    static void check_entry_property(QDomNode rootItem, int entryIdx, QString propertyName, QString propertyValue, bool* ok = NULL, QString* retval = NULL);

    static QString prettyPrintXml(QString xml);
};


#endif
