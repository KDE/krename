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

#include "krenamefile.h"

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
    void testKRenameFile();

private:
    /** Test a KRenameFile class object for
     *  an url if the pass is split correctly.
     *
     *  \param url the url to split
     *  \param directory the expected directory
     *  \param filename the expected filename
     *  \param extension the expected extension
     *  \param eSplitMode the splitmode to use
     *  \param dot the dot to use as splitter
     *  \param isDir test a file or a directory
     */
    void testKRenameFileInternal(const QUrl &url, const QString &directory,
                                 const QString &filename, const QString &extension,
                                 ESplitMode eSplitMode, int dot, bool isDir);

};

#endif // KRENAMEFILE_TEST_H
