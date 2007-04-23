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

#include "krenametest.h"

#include "krenamefile.h"

#include <stdio.h>

#include <QTextEdit>
#include <QVBoxLayout>

#include <kurl.h>

// Macros to simplify the test code

#define PRINT_RESULT( name ) writeTestMessage(" %04i Running test: %20s = %s", \
                                              ++m_counter, name, m_result ? "[OK]" : "[FAILED]" ); \
                             if( !m_result ) { \
                                 ++m_failed; \
                                 writeTestMessage( "<b>ERROR: %04i Test %s failed!</b>\n", m_counter, name ); \
                             } else \
                                 ++m_success;

#define RUN_KRENAMEFILE_TEST( name, url, dir, file, ext, mode, dot ) \
                            m_result = testKRenameFileInternal( url, dir, file, ext, mode, dot ); \
                            PRINT_RESULT( name );


KRenameTest::KRenameTest()
    : QWidget( ),
      m_counter( 0 ), m_success( 0 ), m_failed( 0 )
{
    QVBoxLayout* layout = new QVBoxLayout( this );

    m_text = new QTextEdit( this );
    m_text->setReadOnly( true );

    layout->addWidget( m_text );

    this->resize( 640, 480 );
    this->show();
}

KRenameTest::~KRenameTest()
{
    
}

void KRenameTest::startTest()
{
    testKRenameFile();

    this->writeTestMessage( "<b>Results:</b>\n\nSuccessfull: %i Failed %i", m_success, m_failed );
}

void KRenameTest::writeTestHeader( const QString & text )
{
    QString str( "<b>Testing class: <i>%1</i></b>" );
    m_text->append( str.arg( text ) );
}

void KRenameTest::writeTestMessage( const char* format, ... )
{
    va_list  args;

    va_start( args, format );
    vsnprintf( m_buffer, KRenameTest::BUFFER_LENGTH, format, args );
    va_end( args );

    m_text->append( m_buffer );
}

void KRenameTest::testKRenameFile()
{
    writeTestHeader( "KRenameFile" );

    // Test a simple filename
    RUN_KRENAMEFILE_TEST( "file:///home/test/simple.txt (first dot)", KUrl("file:///home/test/simple.txt"),
                          "/home/test", "simple", "txt", eSplitMode_FirstDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/simple.txt (last dot)", KUrl("file:///home/test/simple.txt"),
                          "/home/test", "simple", "txt", eSplitMode_LastDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/simple.txt (custom dot 0)", KUrl("file:///home/test/simple.txt"),
                          "/home/test", "simple.txt", "", eSplitMode_CustomDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/simple.txt (custom dot 1)", KUrl("file:///home/test/simple.txt"),
                          "/home/test", "simple", "txt", eSplitMode_CustomDot, 1 );

    // Test a simple filename and a directory with spaces
    RUN_KRENAMEFILE_TEST( "file:///home/dir with space/simple.txt (first dot)", KUrl("file:///home/dir with space/simple.txt"),
                          "/home/dir with space", "simple", "txt", eSplitMode_FirstDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/dir with space/simple.txt (last dot)", KUrl("file:///home/dir with space/simple.txt"),
                          "/home/dir with space", "simple", "txt", eSplitMode_LastDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/dir with space/simple.txt (custom dot 0)", KUrl("file:///home/dir with space/simple.txt"),
                          "/home/dir with space", "simple.txt", "", eSplitMode_CustomDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/dir with space/simple.txt (custom dot 1)", KUrl("file:///home/dir with space/simple.txt"),
                          "/home/dir with space", "simple", "txt", eSplitMode_CustomDot, 1 );

    // test a more complicated file extension
    RUN_KRENAMEFILE_TEST( "file:///home/test/complicated.tar.gz (first dot)", KUrl("file:///home/test/complicated.tar.gz"),
                          "/home/test", "complicated", "tar.gz", eSplitMode_FirstDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/complicated.tar.gz (last dot)", KUrl("file:///home/test/complicated.tar.gz"),
                          "/home/test", "complicated.tar", "gz", eSplitMode_LastDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/complicated.tar.gz (custom dot 0)", KUrl("file:///home/test/complicated.tar.gz"),
                          "/home/test", "complicated.tar.gz", "", eSplitMode_CustomDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/complicated.tar.gz (custom dot 1)", KUrl("file:///home/test/complicated.tar.gz"),
                          "/home/test", "complicated", "tar.gz", eSplitMode_CustomDot, 1 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/complicated.tar.gz (custom dot 2)", KUrl("file:///home/test/complicated.tar.gz"),
                          "/home/test", "complicated.tar", "gz", eSplitMode_CustomDot, 2 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/complicated.tar.gz (custom dot 3)", KUrl("file:///home/test/complicated.tar.gz"),
                          "/home/test", "complicated.tar.gz", "", eSplitMode_CustomDot, 3 );


    // test a directory with dot
    RUN_KRENAMEFILE_TEST( "file:///home/dir.with.dot/simple.txt (first dot)", KUrl("file:///home/dir.with.dot/simple.txt"),
                          "/home/dir.with.dot", "simple", "txt", eSplitMode_FirstDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/dir.with.dot/simple.txt (last dot)", KUrl("file:///home/dir.with.dot/simple.txt"),
                          "/home/dir.with.dot", "simple", "txt", eSplitMode_LastDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/dir.with.dot/simple.txt (custom dot 0)", KUrl("file:///home/dir.with.dot/simple.txt"),
                          "/home/dir.with.dot", "simple.txt", "", eSplitMode_CustomDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/dir.with.dot/simple.txt (custom dot 1)", KUrl("file:///home/dir.with.dot/simple.txt"),
                          "/home/dir.with.dot", "simple", "txt", eSplitMode_CustomDot, 1 );

    // test a directory which ends with a slash
    RUN_KRENAMEFILE_TEST( "file:///home/test/ (first dot)", KUrl("file:///home/test/"),
                          "/home/test", "", "", eSplitMode_FirstDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/ (last dot)", KUrl("file:///home/test/"),
                          "/home/test", "", "", eSplitMode_LastDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/ (custom dot 0)", KUrl("file:///home/test/"),
                          "/home/test", "", "", eSplitMode_CustomDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/ (custom dot 1)", KUrl("file:///home/test/"),
                          "/home/test", "", "", eSplitMode_CustomDot, 1 );

    // test a directory which doesn't end with a slash
    RUN_KRENAMEFILE_TEST( "file:///home/test (first dot)", KUrl("file:///home/test"),
                          "/home/test", "", "", eSplitMode_FirstDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/test (last dot)", KUrl("file:///home/test"),
                          "/home/test", "", "", eSplitMode_LastDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/test (custom dot 0)", KUrl("file:///home/test"),
                          "/home/test", "", "", eSplitMode_CustomDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/test (custom dot 1)", KUrl("file:///home/test"),
                          "/home/test", "", "", eSplitMode_CustomDot, 1 );

    // test no file extension
    RUN_KRENAMEFILE_TEST( "file:///home/test/simple (first dot)", KUrl("file:///home/test/simple"),
                          "/home/test", "simple", "", eSplitMode_FirstDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/simple (last dot)", KUrl("file:///home/test/simple"),
                          "/home/test", "simple", "", eSplitMode_LastDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/simple (custom dot 0)", KUrl("file:///home/test/simple"),
                          "/home/test", "simple", "", eSplitMode_CustomDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/simple (custom dot 1)", KUrl("file:///home/test/simple"),
                          "/home/test", "simple", "", eSplitMode_CustomDot, 1 );

    // test very long complex file extension
    RUN_KRENAMEFILE_TEST( "file:///home/test/simple.txt.long.ext.zip (first dot)", KUrl("file:///home/test/simple.txt.long.ext.zip"),
                          "/home/test", "simple", "txt.long.ext.zip", eSplitMode_FirstDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/simple.txt.long.ext.zip (last dot)", KUrl("file:///home/test/simple.txt.long.ext.zip"),
                          "/home/test", "simple.txt.long.ext", "zip", eSplitMode_LastDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/simple.txt.long.ext.zip (custom dot 0)", KUrl("file:///home/test/simple.txt.long.ext.zip"),
                          "/home/test", "simple.txt.long.ext.zip", "", eSplitMode_CustomDot, 0 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/simple.txt.long.ext.zip (custom dot 1)", KUrl("file:///home/test/simple.txt.long.ext.zip"),
                          "/home/test", "simple", "txt.long.ext.zip", eSplitMode_CustomDot, 1 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/simple.txt.long.ext.zip (custom dot 2)", KUrl("file:///home/test/simple.txt.long.ext.zip"),
                          "/home/test", "simple.txt", "long.ext.zip", eSplitMode_CustomDot, 2 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/simple.txt.long.ext.zip (custom dot 3)", KUrl("file:///home/test/simple.txt.long.ext.zip"),
                          "/home/test", "simple.txt.long", "ext.zip", eSplitMode_CustomDot, 3 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/simple.txt.long.ext.zip (custom dot 4)", KUrl("file:///home/test/simple.txt.long.ext.zip"),
                          "/home/test", "simple.txt.long.ext", "zip", eSplitMode_CustomDot, 4 );
    RUN_KRENAMEFILE_TEST( "file:///home/test/simple.txt.long.ext.zip (custom dot 5)", KUrl("file:///home/test/simple.txt.long.ext.zip"),
                          "/home/test", "simple.txt.long.ext.zip", "", eSplitMode_CustomDot, 5 );
}

bool KRenameTest::testKRenameFileInternal( const KUrl & url, const QString & directory, 
                                           const QString & filename, const QString & extension, 
                                           ESplitMode eSplitMode, int dot )
{
    // if the expected filename and extension is empty expect a directoy
    bool        isDirectory = filename.isEmpty() && extension.isEmpty();
    KRenameFile file( url, isDirectory, eSplitMode, dot );

    if( file.srcDirectory() != directory )
    {
        writeTestMessage( "Directory=(%s) expected (%s).\n", 
                          file.srcDirectory().toLatin1().data(), 
                          directory.toLatin1().data() );
        return false;
    }

    if( file.srcFilename() != filename )
    {
        writeTestMessage( "Filename=(%s) expected (%s).\n", 
                          file.srcFilename().toLatin1().data(), 
                          filename.toLatin1().data() );
        return false;
    }

    if( file.srcExtension() != extension )
    {
        writeTestMessage( "Extension=(%s) expected (%s).\n", 
                          file.srcExtension().toLatin1().data(), 
                          extension.toLatin1().data() );
        return false;
    }

    return true;
}
