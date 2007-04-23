/***************************************************************************
                          krenamefile.cpp  -  description
                             -------------------
    begin                : Wed Apr 18 2007
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

#include "krenamefile.h"

#include <kfileitem.h>
#include <kio/netaccess.h>

KRenameFile::KRenameFile( KUrl src, ESplitMode eSplitMode, unsigned int dot )
    : m_bValid( false )
{
    KIO::UDSEntry entry;
    KIO::NetAccess::stat( src, entry, NULL );
    KFileItem file( entry, src );

    m_bValid     = file.isReadable();
    m_bDirectory = file.isDir();

    initFileDescription( m_src, src, eSplitMode, dot );
}

KRenameFile::KRenameFile( KUrl src, bool directory, ESplitMode eSplitMode, unsigned int dot )
    : m_bDirectory( directory ), m_bValid( true )
{
    initFileDescription( m_src, src, eSplitMode, dot );
}

KRenameFile::KRenameFile( const KRenameFile & rhs )
{
    this->operator=( rhs );
}

const KRenameFile & KRenameFile::operator=( const KRenameFile & rhs ) 
{
    m_bDirectory = rhs.m_bDirectory;
    m_src        = rhs.m_src;
    m_dst        = rhs.m_dst;

    return *this;
}

void KRenameFile::setCurrentSplitMode( ESplitMode eSplitMode, unsigned int dot )
{
    KUrl    url      = m_src.url;
    QString filename = m_src.filename + "." + m_src.extension;

    url.setDirectory( m_src.directory );
    url.addPath( filename );

    this->initFileDescription( m_src, url, eSplitMode, dot );
}

void KRenameFile::initFileDescription( TFileDescription & rDescription, const KUrl & url, 
                                       ESplitMode eSplitMode, unsigned int dot ) const
{
    int splitPos = -1;
    QString path = url.path();
    QString file;

    if( !m_bValid )
        return;

    rDescription.url       = url;
    rDescription.directory = path;

    if( !m_bDirectory ) 
    {
        // split directory/filename
        splitPos = path.lastIndexOf( '/' );
        if( splitPos == -1 )
        {
            // only a filename?? can this happen?
            file = path;
            path = QString::null;
        }
        else
        {
            file = path.right( path.length() - splitPos - 1 );
            path = path.left( splitPos );
        }

        // split filename.extension
        splitPos = -1;
        if( eSplitMode == eSplitMode_FirstDot ) 
            splitPos = file.indexOf( '.' );
        else if( eSplitMode == eSplitMode_LastDot )
            splitPos = file.lastIndexOf( '.' );
        else
        {
            if( dot ) 
            {
                int i = 0;
                splitPos = 0;
                do {
                    splitPos = file.indexOf( '.', splitPos + 1 );
                    ++i;
                } while( i < dot && splitPos != -1 );
            }
            else
                // if dot == 0, do not take an extension
                splitPos = file.length();
        }

        if( splitPos == -1 )
            splitPos = file.length();

        rDescription.filename  = file.left( splitPos );
        if( splitPos != file.length() )
            rDescription.extension = file.right( file.length() - splitPos - 1 );
        rDescription.directory = path;
    }
    else
    {
        if( rDescription.directory.endsWith( '/' ) )
            rDescription.directory = rDescription.directory.left( rDescription.directory.length() - 1 );
    }

    /*
      TODO: Write a real unit test for this class
    */
    /*
    qDebug("URL : %s", url.prettyUrl().toLatin1().data() );
    qDebug("Path: %s", rDescription.directory.toLatin1().data());
    qDebug("File: %s", rDescription.filename.toLatin1().data());
    qDebug("Ext : %s", rDescription.extension.toLatin1().data());
    qDebug("Split %i", splitPos );
    qDebug("Dot   %i", dot );
    qDebug("=====");
    */
}



