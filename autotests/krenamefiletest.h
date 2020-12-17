// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#ifndef KRENAMEFILE_TEST_H
#define KRENAMEFILE_TEST_H

#include <QObject>

/** This class contains KRename's selftest.
 *  The selftest consists of several unit tests for
 *  important classes.
 *
 *  Currently covered classes:
 *
 *  - KRenameFile
 */
class KRenameFileTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testKRenameFile_data();
    void testKRenameFile();
};

#endif // KRENAMEFILE_TEST_H
