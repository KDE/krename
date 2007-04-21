/***************************************************************************
                          krenametest.h  -  description
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

#ifndef _KRENAME_TEST_H
#define _KRENAME_TEST_H

#include <QWidget>

class QTextEdit;

/** This class contains KRename's selftest.
 *  The selftest consists of several unit tests for
 *  important classes.
 *
 *  Currently covered classes:
 *
 *  - KRenameFile
 */
class KRenameTest : public QWidget {
 public:
    /** Construct a new KRenameTest dialog
     */
    KRenameTest();
    ~KRenameTest();

    /** Starts the selftest
     */
    void startTest();

 private:
    /** Test the KRenameFile class
     */
    void testKRenameFile();

    /** Write a test caption to the output window
     *  \param text the text which will be output
     */
    void writeTestHeader( const QString & text );

 private:
    QTextEdit* m_text;
};

#endif // _KRENAME_TEST_H
