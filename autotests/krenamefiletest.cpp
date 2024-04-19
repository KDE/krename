// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#include "krenamefiletest.h"

#include "krenamefile.h"

#include <KLocalizedString>

#include <QtTest>

QTEST_GUILESS_MAIN(KRenameFileTest)
Q_DECLARE_METATYPE(ESplitMode)

void KRenameFileTest::initTestCase()
{
    KLocalizedString::setApplicationDomain("krename");
    qRegisterMetaType<ESplitMode>();
}

void KRenameFileTest::testKRenameFile_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QString>("directory");
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QString>("extension");
    QTest::addColumn<ESplitMode>("eSplitMode");
    QTest::addColumn<int>("dot");
    QTest::addColumn<bool>("isDir");

    // Test a simple filename
    QTest::newRow("simple/FirstDot")
      << QUrl("file:///home/test/simple.txt") << "/home/test"
      << "simple" << "txt" << eSplitMode_FirstDot << 0 << false;
    QTest::newRow("simple/LastDot")
      << QUrl("file:///home/test/simple.txt") << "/home/test"
      << "simple" << "txt" << eSplitMode_LastDot << 0 << false;
    QTest::newRow("simple/NoExtension")
      << QUrl("file:///home/test/simple.txt") << "/home/test"
      << "simple.txt" << "" << eSplitMode_NoExtension << 0 << false;
    QTest::newRow("simple/NoExtension/1")
      << QUrl("file:///home/test/simple.txt") << "/home/test"
      << "simple.txt" << "" << eSplitMode_NoExtension << 1 << false;
    QTest::newRow("simple/CustomDot/0")
      << QUrl("file:///home/test/simple.txt") << "/home/test"
      << "simple.txt" << "" << eSplitMode_CustomDot << 0 << false;
    QTest::newRow("simple/CustomDot/1")
      << QUrl("file:///home/test/simple.txt") << "/home/test"
      << "simple" << "txt" << eSplitMode_CustomDot << 1 << false;

    // Test a simple filename and a directory with spaces
    QTest::newRow("spaces/FirstDot")
      << QUrl("file:///home/dir with space/simple.txt")
      << "/home/dir with space" << "simple" << "txt"
      << eSplitMode_FirstDot << 0 << false;
    QTest::newRow("spaces/LastDot")
      << QUrl("file:///home/dir with space/simple.txt")
      << "/home/dir with space" << "simple" << "txt"
      << eSplitMode_LastDot << 0 << false;
    QTest::newRow("spaces/CustomDot/0")
      << QUrl("file:///home/dir with space/simple.txt")
      << "/home/dir with space" << "simple.txt" << ""
      << eSplitMode_CustomDot << 0 << false;
    QTest::newRow("spaces/CustomDot/1")
      << QUrl("file:///home/dir with space/simple.txt")
      << "/home/dir with space" << "simple" << "txt"
      << eSplitMode_CustomDot << 1 << false;

    // test a more complicated file extension
    QTest::newRow("complicated/FirstDot")
      << QUrl("file:///home/test/complicated.tar.gz")
      << "/home/test" << "complicated" << "tar.gz"
      << eSplitMode_FirstDot << 0 << false;
    QTest::newRow("complicated/LastDot")
      << QUrl("file:///home/test/complicated.tar.gz")
      << "/home/test" << "complicated.tar" << "gz"
      << eSplitMode_LastDot << 0 << false;
    QTest::newRow("complicated/CustomDot/0")
      << QUrl("file:///home/test/complicated.tar.gz")
      << "/home/test" << "complicated.tar.gz" << ""
      << eSplitMode_CustomDot << 0 << false;
    QTest::newRow("complicated/CustomDot/1")
      << QUrl("file:///home/test/complicated.tar.gz")
      << "/home/test" << "complicated" << "tar.gz"
      << eSplitMode_CustomDot << 1 << false;
    QTest::newRow("complicated/CustomDot/2")
      << QUrl("file:///home/test/complicated.tar.gz")
      << "/home/test" << "complicated.tar" << "gz"
      << eSplitMode_CustomDot << 2 << false;
    QTest::newRow("complicated/CustomDot/3")
      << QUrl("file:///home/test/complicated.tar.gz")
      << "/home/test" << "complicated.tar.gz" << ""
      << eSplitMode_CustomDot << 3 << false;

    // test a directory with dot
    QTest::newRow("dot-dir/FirstDot")
      << QUrl("file:///home/dir.with.dot/simple.txt")
      << "/home/dir.with.dot" << "simple" << "txt"
      << eSplitMode_FirstDot << 0 << false;
    QTest::newRow("dot-dir/LastDot")
      << QUrl("file:///home/dir.with.dot/simple.txt")
      << "/home/dir.with.dot" << "simple" << "txt"
      << eSplitMode_LastDot << 0 << false;
    QTest::newRow("dot-dir/CustomDot/0")
      << QUrl("file:///home/dir.with.dot/simple.txt")
      << "/home/dir.with.dot" << "simple.txt" << ""
      << eSplitMode_CustomDot << 0 << false;
    QTest::newRow("dot-dir/CustomDot/1")
      << QUrl("file:///home/dir.with.dot/simple.txt")
      << "/home/dir.with.dot" << "simple" << "txt"
      << eSplitMode_CustomDot << 1 << false;

    // test a directory which ends with a slash
    QTest::newRow("dir-ends-slash/FirstDot")
      << QUrl("file:///home/test/") << "/home"
      << "test" << "" << eSplitMode_FirstDot << 0 << true;
    QTest::newRow("dir-ends-slash/LastDot")
      << QUrl("file:///home/test/") << "/home"
      << "test" << "" << eSplitMode_LastDot << 0 << true;
    QTest::newRow("dir-ends-slash/CustomDot/0")
      << QUrl("file:///home/test/") << "/home"
      << "test" << "" << eSplitMode_CustomDot << 0 << true;
    QTest::newRow("dir-ends-slash/CustomDot/1")
      << QUrl("file:///home/test/") << "/home"
      << "test" << "" << eSplitMode_CustomDot << 1 << true;

    // test a directory which doesn't end with a slash
    QTest::newRow("dir-not-ends-slash/FirstDot")
      << QUrl("file:///home/test") << "/home"
      << "test" << "" << eSplitMode_FirstDot << 0 << true;
    QTest::newRow("dir-not-ends-slash/LastDot")
      << QUrl("file:///home/test") << "/home"
      << "test" << "" << eSplitMode_LastDot << 0 << true;
    QTest::newRow("dir-not-ends-slash/CustomDot/0")
      << QUrl("file:///home/test") << "/home"
      << "test" << "" << eSplitMode_CustomDot << 0 << true;
    QTest::newRow("dir-not-ends-slash/CustomDot/1")
      << QUrl("file:///home/test") << "/home"
      << "test" << "" << eSplitMode_CustomDot << 1 << true;

    // test no file extension
    QTest::newRow("no-extension/FirstDot")
      << QUrl("file:///home/test/simple") << "/home/test"
      << "simple" << "" << eSplitMode_FirstDot << 0 << false;
    QTest::newRow("no-extension/LastDot")
      << QUrl("file:///home/test/simple") << "/home/test"
      << "simple" << "" << eSplitMode_LastDot << 0 << false;
    QTest::newRow("no-extension/CustomDot/0")
      << QUrl("file:///home/test/simple") << "/home/test"
      << "simple" << "" << eSplitMode_CustomDot << 0 << false;
    QTest::newRow("no-extension/CustomDot/1")
      << QUrl("file:///home/test/simple") << "/home/test"
      << "simple" << "" << eSplitMode_CustomDot << 1 << false;

    // test very long complex file extension
    QTest::newRow("complex-extension/FirstDot")
      << QUrl("file:///home/test/simple.txt.long.ext.zip")
      << "/home/test" << "simple" << "txt.long.ext.zip"
      << eSplitMode_FirstDot << 0 << false;
    QTest::newRow("complex-extension/LastDot")
      << QUrl("file:///home/test/simple.txt.long.ext.zip")
      << "/home/test" << "simple.txt.long.ext" << "zip"
      << eSplitMode_LastDot << 0 << false;
    QTest::newRow("complex-extension/CustomDot/0")
      << QUrl("file:///home/test/simple.txt.long.ext.zip")
      << "/home/test" << "simple.txt.long.ext.zip" << ""
      << eSplitMode_CustomDot << 0 << false;
    QTest::newRow("complex-extension/CustomDot/1")
      << QUrl("file:///home/test/simple.txt.long.ext.zip")
      << "/home/test" << "simple" << "txt.long.ext.zip"
      << eSplitMode_CustomDot << 1 << false;
    QTest::newRow("complex-extension/CustomDot/2")
      << QUrl("file:///home/test/simple.txt.long.ext.zip")
      << "/home/test" << "simple.txt" << "long.ext.zip"
      << eSplitMode_CustomDot << 2 << false;
    QTest::newRow("complex-extension/CustomDot/3")
      << QUrl("file:///home/test/simple.txt.long.ext.zip")
      << "/home/test" << "simple.txt.long" << "ext.zip"
      << eSplitMode_CustomDot << 3 << false;
    QTest::newRow("complex-extension/CustomDot/4")
      << QUrl("file:///home/test/simple.txt.long.ext.zip")
      << "/home/test" << "simple.txt.long.ext" << "zip"
      << eSplitMode_CustomDot << 4 << false;
    QTest::newRow("complex-extension/CustomDot/5")
      << QUrl("file:///home/test/simple.txt.long.ext.zip")
      << "/home/test" << "simple.txt.long.ext.zip" << ""
      << eSplitMode_CustomDot << 5 << false;
}

void KRenameFileTest::testKRenameFile()
{
    QFETCH(QUrl, url);
    QFETCH(QString, directory);
    QFETCH(QString, filename);
    QFETCH(QString, extension);
    QFETCH(ESplitMode, eSplitMode);
    QFETCH(int, dot);
    QFETCH(bool, isDir);

    // if the expected filename and extension is empty expect a directory
    //bool        isDirectory = filename.isEmpty() && extension.isEmpty();
    KRenameFile file(url, isDir, eSplitMode, dot);

    QCOMPARE(file.srcDirectory(), directory);
    QCOMPARE(file.srcFilename(), filename);
    QCOMPARE(file.srcExtension(), extension);
}

#include "moc_krenamefiletest.cpp"
