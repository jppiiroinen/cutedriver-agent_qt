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
#include <QtTest>
#include <QtXml>

#include "../src/imxgpumeasureinterface.h"
#include "../src/imxgpuapps.h"
#include "../src/imxgpuinfo.h"
#include "../src/imxgpumem.h"
#include "../src/imxgpuusage.h"
#include "../src/imxgpuversion.h"
#include "../src/cutelinuxhelper.h"

#include <taslogger.h>

#include "../../../tests/testhelper.h"

class TestImxFixture : public QObject
{
    Q_OBJECT

public:
    TestImxFixture();
    ~TestImxFixture();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_initialize_data();
    void test_initialize();
    void test_parse_info();
    void test_parse_apps();
    void test_parse_mem();
    void test_parse_usage();
    void test_parse_version();
};

TestImxFixture::TestImxFixture()
{
    TasLogger::logger()->setLevel(LOG_DEBUG);
    TasLogger::logger()->setLogDir(".");
    TasLogger::logger()->useQDebug(false);
    TasLogger::logger()->enableLogger();

}

TestImxFixture::~TestImxFixture()
{

}

void TestImxFixture::initTestCase()
{

}

void TestImxFixture::cleanupTestCase()
{

}


void TestImxFixture::test_initialize()
{
    QFETCH(ImxGpuMeasureInterface*, measurement);

    QVERIFY(measurement->checkValidity(measurement->lastResult()) == false);
    QVERIFY(measurement->lastResult().keys().count() == 0);
}

void TestImxFixture::test_initialize_data()
{
    QTest::addColumn<ImxGpuMeasureInterface*>("measurement");
    QTest::newRow("apps")       << (ImxGpuMeasureInterface*) new ImxGpuApps();
    QTest::newRow("mem")        << (ImxGpuMeasureInterface*) new ImxGpuMem();
    QTest::newRow("info")       << (ImxGpuMeasureInterface*) new ImxGpuInfo();
    QTest::newRow("usage")      << (ImxGpuMeasureInterface*) new ImxGpuUsage();
    QTest::newRow("version")    << (ImxGpuMeasureInterface*) new ImxGpuVersion();
}

void TestImxFixture::test_parse_info() {
    ImxGpuInfo* info = new ImxGpuInfo();
    QString testDataFile = "./data/info";

    QHash<QString, QVariant> result;
    QString testData = CuteLinuxHelper::readFile(testDataFile);
    QVERIFY(testData.count() > 0);
    QVERIFY(info->checkValidity(result) == false);

    result = info->parseResult(testData);
    QVERIFY(result.keys().count() > 0);
    QVERIFY(info->checkValidity(result));

    QStringList gpus;
    gpus << "0" << "1" << "2";
    QHash<QString, QString> fields;
    fields["0_model"] = "3000";
    fields["0_revision"] = "5450";

    fields["1_model"] = "320";
    fields["1_revision"] = "5303";

    fields["2_model"] = "355";
    fields["2_revision"] = "1216";

    foreach(QString gpu, gpus) {
        QStringList gpuFields;
        gpuFields << QString("%0_model").arg(gpu) << QString("%0_revision").arg(gpu);
        foreach(QString gpuField, gpuFields) {
            QVERIFY(result.contains(gpuField));
            QVERIFY2(result[gpuField] == fields[gpuField], QString("'%0' expected to be '%1' actual '%2'.").arg(gpuField).arg(fields[gpuField]).arg(result[gpuField].toString()).toLatin1().data());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    // check that the values above can be found from the generated xml file as well.
    QString xml;
    info->m_fileName = testDataFile;
    info->startMeasure();
    info->stopMeasure();
    info->sendReport(xml);

    // check that the xml document is valid.
    QDomNode rootItem;
    bool ok = false;
    TestHelper::get_tasmessage(xml, &rootItem, &ok);
    QVERIFY2(ok, "Unable to parse tasmessage from xml.");

    TEST_PROPERTY(rootItem, "env", "qt");
    TEST_PROPERTY(rootItem, "id", "0");
    TEST_PROPERTY(rootItem, "type", "logData");

    // lets verify that our data is correct.
    TEST_PROPERTY(rootItem, "name", "GpuInfo");
    TEST_ATTRIBUTE(rootItem, "entryCount", QString::number(gpus.count()));
    TEST_ATTRIBUTE(rootItem, "isValid", "1");

    int idx = 0;
    foreach(QString key, gpus) {
        TEST_ENTRY_PROPERTY(rootItem, idx, "name", "LogEntry");

        QString model = fields[QString("%0_model").arg(key)];
        QString revision = fields[QString("%0_revision").arg(key)];
        TEST_ENTRY_ATTRIBUTE(rootItem, idx, "model", model);
        TEST_ENTRY_ATTRIBUTE(rootItem, idx, "revision", revision);

        idx++;
    }
}

void TestImxFixture::test_parse_apps() {
    ImxGpuApps* apps = new ImxGpuApps();
    QString testDataFile = "./data/clients";

    QHash<QString, QVariant> result;
    QString testData = CuteLinuxHelper::readFile(testDataFile);
    QVERIFY(testData.count() > 0);
    QVERIFY(apps->checkValidity(result) == false);

    result = apps->parseResult(testData);
    QVERIFY(result.keys().count() > 0);
    QVERIFY(apps->checkValidity(result));

    QHash<QString, QString> processes;
    processes["389"] = "someApp-ui";
    processes["30"] = "otherAppd";

    foreach(QString key, processes.keys()) {
        QVERIFY(result.keys().contains(key));
        QVERIFY2(result[key] == processes[key], QString("'%0' expected to be '%1' actual '%2'.").arg(key).arg(processes[key]).arg(result[key].toString()).toLatin1().data());
    }

    /////////////////////////////////////////////////////////////////////////////////
    // check that the values above can be found from the generated xml file as well.
    QString xml;
    apps->m_fileName = testDataFile;
    apps->startMeasure();
    apps->stopMeasure();
    apps->sendReport(xml);

    // check that the xml document is valid.
    QDomNode rootItem;
    bool ok = false;
    TestHelper::get_tasmessage(xml, &rootItem, &ok);
    QVERIFY2(ok, "Unable to parse tasmessage from xml.");

    TEST_PROPERTY(rootItem, "env", "qt");
    TEST_PROPERTY(rootItem, "id", "0");
    TEST_PROPERTY(rootItem, "type", "logData");

    // lets verify that our data is correct.
    TEST_PROPERTY(rootItem, "name", "GpuApps");
    TEST_ATTRIBUTE(rootItem, "entryCount", QString::number(processes.keys().count()));
    TEST_ATTRIBUTE(rootItem, "isValid", "1");

    int idx = 0;
    foreach(QString key, processes.keys()) {
        TEST_ENTRY_PROPERTY(rootItem, idx, "name", "LogEntry");

        QString pid = key;
        QString processName = processes[key];
        TEST_ENTRY_ATTRIBUTE(rootItem, idx, "pid", pid);
        TEST_ENTRY_ATTRIBUTE(rootItem, idx, "processName", processName);

        idx++;
    }
}

void TestImxFixture::test_parse_mem() {
    ImxGpuMem* mem = new ImxGpuMem();
    QString testDataFile = "./data/meminfo";

    QHash<QString, QVariant> result;
    QString testData = CuteLinuxHelper::readFile(testDataFile);
    QVERIFY(testData.count() > 0);
    QVERIFY(mem->checkValidity(result) == false);

    result = mem->parseResult(testData);
    QVERIFY(result.keys().count() > 0);
    QVERIFY(mem->checkValidity(result));

    QHash<QString, qlonglong> memData;
    memData["nonpagedmemory_gcvpool_virtual_used"] = 3;
    memData["videomemory_gcvpool_virtual_used"] = 2;
    memData["videomemory_gcvpool_contiguous_used"] = 1;
    memData["videomemory_gcvpool_system_used"] = 25096088;
    memData["videomemory_gcvpool_system_free"] = 109121640;
    memData["videomemory_gcvpool_system_total"] = 134217728;

    foreach(QString key, memData.keys()) {
        QVERIFY2(result[key] == memData[key], QString("'%0' expected to be '%1' actual '%2'.").arg(key).arg(memData[key]).arg(result[key].toString()).toLatin1().data());
    }

    /////////////////////////////////////////////////////////////////////////////////
    // check that the values above can be found from the generated xml file as well.
    QString xml;
    mem->m_fileName = testDataFile;
    mem->startMeasure();
    mem->stopMeasure();
    mem->sendReport(xml);
    QVERIFY(xml.length() > 0);

    // check that the xml document is valid.
    QDomNode rootItem;
    bool ok = false;
    TestHelper::get_tasmessage(xml, &rootItem, &ok);
    QVERIFY2(ok, "Unable to parse tasmessage from xml.");

    TEST_PROPERTY(rootItem, "env", "qt");
    TEST_PROPERTY(rootItem, "id", "0");
    TEST_PROPERTY(rootItem, "type", "logData");

    // lets verify that our data is correct.
    TEST_PROPERTY(rootItem, "name", "GpuMem");
    TEST_ATTRIBUTE(rootItem, "entryCount", "1");
    TEST_ATTRIBUTE(rootItem, "isValid", "1");

    int idx = 0;

    TEST_ENTRY_PROPERTY(rootItem, idx, "name", "LogEntry");

    foreach(QString key, memData.keys()) {
        TEST_ENTRY_ATTRIBUTE(rootItem, idx, key, QString::number(memData[key]));
    }
}

void TestImxFixture::test_parse_usage() {
    ImxGpuUsage* usage = new ImxGpuUsage();
    QString testDataFile = "./data/idle";

    QHash<QString, QVariant> result;
    QString testData = CuteLinuxHelper::readFile(testDataFile);
    QVERIFY(testData.count() > 0);
    QVERIFY(usage->checkValidity(result) == false);

    result = usage->parseResult(testData);
    QVERIFY(result.keys().count() > 0);
    QVERIFY(usage->checkValidity(result));

    QHash<QString, QVariant> usageData;
    usageData["end"] = (qlonglong)13581369874241;
    usageData["start"] = (qlonglong)13569336592233;
    usageData["on"] = (qlonglong)4512480753;
    usageData["off"] = (qlonglong)2256240377;
    usageData["idle"] = (qlonglong)1504160251;
    usageData["suspend"] = (qlonglong)3760400627;

    foreach(QString key, usageData.keys()) {
        QVERIFY2(result[key].toString() == usageData[key].toString(), QString("'%0' expected to be '%1' actual '%2'.").arg(key).arg(usageData[key].toString()).arg(result[key].toString()).toLatin1().data());
    }

    /////////////////////////////////////////////////////////////////////////////////
    // check that the values above can be found from the generated xml file as well.
    QString xml;

    usage->m_fileName = testDataFile + ".previous";
    usage->startMeasure();
    usage->m_fileName = testDataFile;
    usage->stopMeasure();
    usage->sendReport(xml);

    // check that the xml document is valid.
    QDomNode rootItem;
    bool ok = false;
    TestHelper::get_tasmessage(xml, &rootItem, &ok);
    QVERIFY2(ok, "Unable to parse tasmessage from xml.");

    TEST_PROPERTY(rootItem, "env", "qt");
    TEST_PROPERTY(rootItem, "id", "0");
    TEST_PROPERTY(rootItem, "type", "logData");

    // lets verify that our data is correct.
    TEST_PROPERTY(rootItem, "name", "GpuUsage");
    TEST_ATTRIBUTE(rootItem, "entryCount", "1");
    TEST_ATTRIBUTE(rootItem, "isValid", "1");

    int idx = 0;

    TEST_ENTRY_PROPERTY(rootItem, idx, "name", "LogEntry");

    usageData["on_percentage"] = "37.5";
    usageData["off_percentage"] = "18.75";
    usageData["idle_percentage"] = "12.5";
    usageData["suspend_percentage"] = "31.25";

    foreach(QString key, usageData.keys()) {
        TEST_ENTRY_ATTRIBUTE(rootItem, idx, key, usageData[key].toString());
    }
}

void TestImxFixture::test_parse_version() {
    ImxGpuVersion* version = new ImxGpuVersion();
    QString testDataFile = "./data/version";

    QHash<QString, QVariant> result;
    QString testData = CuteLinuxHelper::readFile(testDataFile);
    QVERIFY(testData.count() > 0);
    QVERIFY(version->checkValidity(result) == false);

    result = version->parseResult(testData);
    QVERIFY(result.keys().count() > 0);
    QVERIFY(version->checkValidity(result));

    QHash<QString, QString> versionData;
    versionData["version"] = "5.0.11.p8.41671 built at ";

    foreach(QString key, versionData.keys()) {
        QVERIFY(result[key] == versionData[key]);
    }

    /////////////////////////////////////////////////////////////////////////////////
    // check that the values above can be found from the generated xml file as well.
    QString xml;
    version->m_fileName = testDataFile;
    version->startMeasure();
    version->stopMeasure();
    version->sendReport(xml);

    // check that the xml document is valid.
    QDomNode rootItem;
    bool ok = false;
    TestHelper::get_tasmessage(xml, &rootItem, &ok);
    QVERIFY2(ok, "Unable to parse tasmessage from xml.");

    TEST_PROPERTY(rootItem, "env", "qt");
    TEST_PROPERTY(rootItem, "id", "0");
    TEST_PROPERTY(rootItem, "type", "logData");

    // lets verify that our data is correct.
    TEST_PROPERTY(rootItem, "name", "GpuVersion");
    TEST_ATTRIBUTE(rootItem, "entryCount", "1");
    TEST_ATTRIBUTE(rootItem, "isValid", "1");

    int idx = 0;

    TEST_ENTRY_PROPERTY(rootItem, idx, "name", "LogEntry");

    foreach(QString key, versionData.keys()) {
        TEST_ENTRY_ATTRIBUTE(rootItem, idx, key, versionData[key]);
    }

}


QTEST_APPLESS_MAIN(TestImxFixture)

#include "tst_testimxfixture.moc"
