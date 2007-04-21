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

#include <QTextEdit>
#include <QVBoxLayout>

KRenameTest::KRenameTest()
    : QWidget( )
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
}

void KRenameTest::writeTestHeader( const QString & text )
{
    m_text->append( text );
    m_text->append( "\n=====================\n\n" );
}

void KRenameTest::testKRenameFile()
{
    writeTestHeader( "KRenameFile" );
}
