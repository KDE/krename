/***************************************************************************
                          krenametest.cpp  -  description
                             -------------------
    begin                : Sat Apr 20 2007
    copyright            : (C) 2007 by Dominik Seichter
    email                : domseichter@web.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "krenamefiletest.h"

#include <QtTest>

QTEST_GUILESS_MAIN(KRenameFileTest)

void KRenameFileTest::testKRenameFile()
{
    // Test a simple filename
    testKRenameFileInternal(QUrl("file:///home/test/simple.txt"), "/home/test",
                            "simple", "txt", eSplitMode_FirstDot, 0, false);
    testKRenameFileInternal(QUrl("file:///home/test/simple.txt"), "/home/test",
                            "simple", "txt", eSplitMode_LastDot, 0, false );
    testKRenameFileInternal(QUrl("file:///home/test/simple.txt"), "/home/test",
                            "simple.txt", "", eSplitMode_NoExtension, 0, false );
    testKRenameFileInternal(QUrl("file:///home/test/simple.txt"), "/home/test",
                            "simple.txt", "", eSplitMode_NoExtension, 1, false );
    testKRenameFileInternal(QUrl("file:///home/test/simple.txt"), "/home/test",
                            "simple.txt", "", eSplitMode_CustomDot, 0, false );
    testKRenameFileInternal(QUrl("file:///home/test/simple.txt"), "/home/test",
                            "simple", "txt", eSplitMode_CustomDot, 1, false );

    // Test a simple filename and a directory with spaces
    testKRenameFileInternal(QUrl("file:///home/dir with space/simple.txt"),
                            "/home/dir with space", "simple", "txt",
                            eSplitMode_FirstDot, 0, false );
    testKRenameFileInternal(QUrl("file:///home/dir with space/simple.txt"),
                            "/home/dir with space", "simple", "txt",
                            eSplitMode_LastDot, 0, false );
    testKRenameFileInternal(QUrl("file:///home/dir with space/simple.txt"),
                            "/home/dir with space", "simple.txt", "",
                            eSplitMode_CustomDot, 0, false );
    testKRenameFileInternal(QUrl("file:///home/dir with space/simple.txt"),
                            "/home/dir with space", "simple", "txt",
                            eSplitMode_CustomDot, 1, false );

    // test a more complicated file extension
    testKRenameFileInternal(QUrl("file:///home/test/complicated.tar.gz"),
                            "/home/test", "complicated", "tar.gz",
                            eSplitMode_FirstDot, 0, false );
    testKRenameFileInternal(QUrl("file:///home/test/complicated.tar.gz"),
                            "/home/test", "complicated.tar", "gz",
                            eSplitMode_LastDot, 0, false );
    testKRenameFileInternal(QUrl("file:///home/test/complicated.tar.gz"),
                            "/home/test", "complicated.tar.gz", "",
                            eSplitMode_CustomDot, 0, false );
    testKRenameFileInternal(QUrl("file:///home/test/complicated.tar.gz"),
                            "/home/test", "complicated", "tar.gz",
                            eSplitMode_CustomDot, 1, false );
    testKRenameFileInternal(QUrl("file:///home/test/complicated.tar.gz"),
                            "/home/test", "complicated.tar", "gz",
                            eSplitMode_CustomDot, 2, false );
    testKRenameFileInternal(QUrl("file:///home/test/complicated.tar.gz"),
                            "/home/test", "complicated.tar.gz", "",
                            eSplitMode_CustomDot, 3, false );

    // test a directory with dot
    testKRenameFileInternal(QUrl("file:///home/dir.with.dot/simple.txt"),
                            "/home/dir.with.dot", "simple", "txt",
                            eSplitMode_FirstDot, 0, false );
    testKRenameFileInternal(QUrl("file:///home/dir.with.dot/simple.txt"),
                            "/home/dir.with.dot", "simple", "txt",
                            eSplitMode_LastDot, 0, false );
    testKRenameFileInternal(QUrl("file:///home/dir.with.dot/simple.txt"),
                            "/home/dir.with.dot", "simple.txt", "",
                            eSplitMode_CustomDot, 0, false );
    testKRenameFileInternal(QUrl("file:///home/dir.with.dot/simple.txt"),
                            "/home/dir.with.dot", "simple", "txt",
                            eSplitMode_CustomDot, 1, false );

    // test a directory which ends with a slash
    testKRenameFileInternal(QUrl("file:///home/test/"), "/home",
                            "test", "", eSplitMode_FirstDot, 0, true );
    testKRenameFileInternal(QUrl("file:///home/test/"), "/home",
                            "test", "", eSplitMode_LastDot, 0, true );
    testKRenameFileInternal(QUrl("file:///home/test/"), "/home",
                            "test", "", eSplitMode_CustomDot, 0, true );
    testKRenameFileInternal(QUrl("file:///home/test/"), "/home",
                            "test", "", eSplitMode_CustomDot, 1, true );

    // test a directory which doesn't end with a slash
    testKRenameFileInternal(QUrl("file:///home/test"), "/home",
                            "test", "", eSplitMode_FirstDot, 0, true );
    testKRenameFileInternal(QUrl("file:///home/test"), "/home",
                            "test", "", eSplitMode_LastDot, 0, true );
    testKRenameFileInternal(QUrl("file:///home/test"), "/home",
                            "test", "", eSplitMode_CustomDot, 0, true );
    testKRenameFileInternal(QUrl("file:///home/test"), "/home",
                            "test", "", eSplitMode_CustomDot, 1, true );

    // test no file extension
    testKRenameFileInternal(QUrl("file:///home/test/simple"), "/home/test",
                            "simple", "", eSplitMode_FirstDot, 0, false );
    testKRenameFileInternal(QUrl("file:///home/test/simple"), "/home/test",
                            "simple", "", eSplitMode_LastDot, 0, false );
    testKRenameFileInternal(QUrl("file:///home/test/simple"), "/home/test",
                            "simple", "", eSplitMode_CustomDot, 0, false );
    testKRenameFileInternal(QUrl("file:///home/test/simple"), "/home/test",
                            "simple", "", eSplitMode_CustomDot, 1, false );

    // test very long complex file extension
    testKRenameFileInternal(QUrl("file:///home/test/simple.txt.long.ext.zip"),
                            "/home/test", "simple", "txt.long.ext.zip",
                            eSplitMode_FirstDot, 0, false );
    testKRenameFileInternal(QUrl("file:///home/test/simple.txt.long.ext.zip"),
                            "/home/test", "simple.txt.long.ext", "zip",
                            eSplitMode_LastDot, 0, false );
    testKRenameFileInternal(QUrl("file:///home/test/simple.txt.long.ext.zip"),
                            "/home/test", "simple.txt.long.ext.zip", "",
                            eSplitMode_CustomDot, 0, false );
    testKRenameFileInternal(QUrl("file:///home/test/simple.txt.long.ext.zip"),
                            "/home/test", "simple", "txt.long.ext.zip",
                            eSplitMode_CustomDot, 1, false );
    testKRenameFileInternal(QUrl("file:///home/test/simple.txt.long.ext.zip"),
                            "/home/test", "simple.txt", "long.ext.zip",
                            eSplitMode_CustomDot, 2, false );
    testKRenameFileInternal(QUrl("file:///home/test/simple.txt.long.ext.zip"),
                            "/home/test", "simple.txt.long", "ext.zip",
                            eSplitMode_CustomDot, 3, false );
    testKRenameFileInternal(QUrl("file:///home/test/simple.txt.long.ext.zip"),
                            "/home/test", "simple.txt.long.ext", "zip",
                            eSplitMode_CustomDot, 4, false );
    testKRenameFileInternal(QUrl("file:///home/test/simple.txt.long.ext.zip"),
                            "/home/test", "simple.txt.long.ext.zip", "",
                            eSplitMode_CustomDot, 5, false );
}

void KRenameFileTest::testKRenameFileInternal(const QUrl &url, const QString & directory,
                                          const QString & filename, const QString & extension,
                                          ESplitMode eSplitMode, int dot, bool isDir)
{
    // if the expected filename and extension is empty expect a directory
    //bool        isDirectory = filename.isEmpty() && extension.isEmpty();
    KRenameFile file(url, isDir, eSplitMode, dot);

    QCOMPARE(file.srcDirectory(), directory);
    QCOMPARE(file.srcFilename(), filename);
    QCOMPARE(file.srcExtension(), extension);
}
