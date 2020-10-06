/***************************************************************************
                          krenamefiletest.h  -  description
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
