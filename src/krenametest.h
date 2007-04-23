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

#include <stdarg.h>

#include <QWidget>

#include "krenamefile.h"

class KUrl;
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

    /** Test a KRenameFile class object for
     *  an url if the pass is splitted correctly.
     *
     *  \param url the url to split
     *  \param directory the expected directory
     *  \param filename the expected filename
     *  \param extension the expected extension
     *  \param eSplitMode the splitmode to use
     *  \param dot the dot to use as splitter
     */
    bool testKRenameFileInternal( const KUrl & url, const QString & directory, 
                                  const QString & filename, const QString & extension,
                                  ESplitMode eSplitMode, int dot );

    /** Write a test caption to the output window
     *  \param text the text which will be output
     */
    void writeTestHeader( const QString & text );

    /** Write a message about to the output window
     *  \param text the text which will be output
     */
    void writeTestMessage( const char* format, ... );

 private:
    QTextEdit* m_text;       ///< output window

    int        m_counter;    ///< test counter (increased for each individual test)
    bool       m_result;     ///< return value of tests is stored here
    int        m_success;    ///< counter of successfull tests
    int        m_failed;     ///< counter of failed tests

    static const int BUFFER_LENGTH = 512; 
    char       m_buffer[BUFFER_LENGTH]; ///< buffer in which messages are constructed
};

#endif // _KRENAME_TEST_H
