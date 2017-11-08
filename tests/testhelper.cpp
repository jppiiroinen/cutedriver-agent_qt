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
#include "testhelper.h"

void TestHelper::get_tasmessage(QString xml, QDomNode* node, bool* ok) {
    if (ok) *ok = false;
    QDomDocument doc = TestHelper::get_xml_document("testxmldoc", xml, ok);
    QVERIFY2(ok, "Invalid xml file");

    if (ok) *ok = false;
    TestHelper::validate_tasmessage_xml_header(doc, ok);
    QVERIFY2(ok,"XML header does not have proper format.");

    // lets make sure that we keep the basic structure the same
    *node = doc.firstChild().firstChild().firstChild();
    QVERIFY(node->nodeName() == "obj");
    if (ok) *ok = true;
}

void TestHelper::check_property_startsWith(QDomNode rootItem, QString propertyName, QString propertyValue, bool* ok, QString* retval) {
    if (ok) *ok = false;
    QString value = TestHelper::get_property(rootItem, propertyName);
    if (retval) *retval = value;
    QVERIFY(value.startsWith(propertyValue));
    if (ok) *ok = true;
}

QString TestHelper::get_property(QDomNode rootItem, QString propertyName) {
    QString retval;
    if (rootItem.attributes().contains(propertyName) == false) return retval;
    return rootItem.attributes().namedItem(propertyName).nodeValue();
}

void TestHelper::check_property(QDomNode rootItem, QString propertyName, QString propertyValue, bool* ok, QString* retval) {
    if (ok) *ok = false;
    QString value = TestHelper::get_property(rootItem, propertyName);
    if (retval) *retval = value;
    QVERIFY2(value == propertyValue, QString("Property '%0' value. Expected '%1', actual '%2'").arg(propertyName).arg(propertyValue).arg(value).toLatin1().data());
    if (ok) *ok = true;
}

void TestHelper::check_attribute(QDomNode rootItem, QString attribute, QString attributeValue, bool* ok, QString* retval) {
    if (ok) *ok = false;
    QDomNode node = TestHelper::get_node_by_name(rootItem, attribute);
    QVERIFY(node.nodeName() == "attr");
    QString value = node.firstChild().nodeValue();
    QVERIFY2(value == attributeValue, QString("Attribute '%0' value. Expected '%1', actual '%2'").arg(attribute).arg(attributeValue).arg(value).toLatin1().data());
    if (retval) *retval = value;
    if (ok) *ok = true;
}

void TestHelper::check_entry_property(QDomNode rootItem, int entryIdx, QString propertyName, QString propertyValue, bool* ok, QString* retval) {
    if (ok) *ok = false;
    QList<QDomNode> nodes = TestHelper::get_nodes_by_type(rootItem, "logEntry");
    QVERIFY(nodes.count() > 0);
    foreach(QDomNode node, nodes) {
        QString idx = TestHelper::get_property(node, "id");
        if (idx.toInt() == entryIdx) {
            QString property = TestHelper::get_property(node, propertyName);
            if (retval) *retval = property;
            if (property == propertyValue) {
                if (ok) *ok = true;
            }
            break;
        }
    }
}

void TestHelper::check_entry_attribute(QDomNode rootItem, int entryIdx, QString attributeName, QString attributeValue, bool* ok, QString* retval) {
    if (ok) *ok = false;
    QList<QDomNode> nodes = TestHelper::get_nodes_by_type(rootItem, "logEntry");
    QVERIFY(nodes.count() > 0);
    foreach(QDomNode node, nodes) {
        QString idx = TestHelper::get_property(node, "id");
        if (idx.toInt() == entryIdx) {
            QString value;
            TestHelper::check_attribute(node, attributeName, attributeValue, ok, &value);
            if (retval) *retval = value;
            break;
        }
    }
}

QList<QDomNode> TestHelper::get_nodes_by_type(QDomNode rootItem, QString typeName) {
    QList<QDomNode> retval;
    for(int idx=0; idx < rootItem.childNodes().count(); idx++) {
        QDomNode node = rootItem.childNodes().at(idx);
        QString nodeType = node.attributes().namedItem("type").nodeValue();
        if (nodeType == typeName) {
            retval.append(node);
        }
    }
    return retval;
}


QDomNode TestHelper::get_node_by_name(QDomNode rootItem, QString name) {
    QDomNode retval;
    for(int idx=0; idx < rootItem.childNodes().count(); idx++) {
        QDomNode node = rootItem.childNodes().at(idx);
        QString nodeName = node.attributes().namedItem("name").nodeValue();
        if (nodeName == name) {
            retval = node;
            break;
        }
    }
    return retval;
}

void TestHelper::validate_tasmessage_xml_header(QDomDocument doc, bool *ok) {
    if (ok) *ok = false;

    QVERIFY(doc.hasChildNodes());
    QDomNode tasMessage = doc.firstChild();
    QVERIFY(tasMessage.nodeName() == "tasMessage");
    QVERIFY(tasMessage.attributes().contains("version"));

    QDomNode tasInfo = tasMessage.firstChild();
    QVERIFY(tasInfo.nodeName() == "tasInfo");

    bool check_ok = false;
    check_property(tasInfo, "id", "1", &check_ok);
    QVERIFY(check_ok);

    check_property_startsWith(tasInfo, "name", "Qt", &check_ok);
    QVERIFY(check_ok);

    check_property(tasInfo, "type", "qt", &check_ok);
    QVERIFY(check_ok);

    if (ok) *ok = true;
}

QDomDocument TestHelper::get_xml_document(QString name, QString xml, bool* ok) {
    QDomDocument xmlReport(name);

    if (ok) *ok = true;
    if (xml.length() <= 0) {
        if (ok) *ok = false;
    }

    if (*ok) {
        // check that the xml document is valid.
        QString errorMessage = "";
        int errorLine = -1;
        int errorColumn = -1;
        xmlReport.setContent(xml, &errorMessage, &errorLine, &errorColumn);
        if (!errorMessage.isEmpty()) {
            qWarning() << "Error: " << errorMessage;
            if (ok) *ok = false;
        }

        if (errorLine != -1) {
            qWarning() << "Error in line" << errorLine;
            if (ok) *ok = false;
        }
        if (errorColumn != -1) {
            qWarning() << "Error in column " << errorColumn;
            if (ok) *ok = false;
        }
    }

    return xmlReport;
}
