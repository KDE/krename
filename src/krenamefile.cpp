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
#include <kio/previewjob.h>

/** A singleton class that loads icons for urls in a synchronous way
 */
class KRenamePreviewProvider {

public:
    /** Get the KRenamePreviewProvider instance. This is a singleton
     *  as only one KRenamePreviewProvider may exist for one application.
     */
    static KRenamePreviewProvider* Instance() 
    {
        if( !s_instance ) 
            s_instance = new KRenamePreviewProvider();
            
        return s_instance;
    }

    QPixmap loadIcon( const KUrl & url ) 
    {
        return KIO::pixmapForUrl( url );

	/*
        KIO::UDSEntry entry;
        KIO::NetAccess::stat( url, entry, NULL );
        KFileItem item( entry, url );
        QList<KFileItem> list;
        list.append( item );

        KIO::PreviewJob* job = new KIO::PreviewJob( list, 64, 64, 0, 0, true, false, NULL ); 
        if( !job->exec() ) 
        {
            return item.pixmap( 64 );
        }
        else
        {
	    return KIO::pixmapForUrl( url );
        }
	*/
    }

private:
    /** Create a KRenamePreviewProvider 
     */
    KRenamePreviewProvider() 
    {
    }

    ~KRenamePreviewProvider() 
    {
    }

private:
    static KRenamePreviewProvider* s_instance;
}; 

KRenamePreviewProvider* KRenamePreviewProvider::s_instance = NULL;


KRenameFile::KRenameFile( const KUrl & src, ESplitMode eSplitMode, unsigned int dot )
    : m_bValid( false ), m_error( 0 )
{
    KIO::UDSEntry entry;
    KIO::NetAccess::stat( src, entry, NULL );
    KFileItem file( entry, src );

    m_bValid     = file.isReadable();
    m_bDirectory = file.isDir();

    initFileDescription( m_src, src, eSplitMode, dot );
}

KRenameFile::KRenameFile( const KUrl & src, bool directory, ESplitMode eSplitMode, unsigned int dot )
    : m_bDirectory( directory ), m_bValid( true ), m_error( 0 )
{
    initFileDescription( m_src, src, eSplitMode, dot );
}

KRenameFile::KRenameFile( const KFileItem& item, ESplitMode eSplitMode, unsigned int dot )
    : m_bDirectory( item.isDir() ), m_bValid( item.isReadable() ), m_error( 0 )
{
    initFileDescription( m_src, item.url(), eSplitMode, dot );
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

    m_bValid     = rhs.m_bValid;
    m_icon       = rhs.m_icon;
    m_error      = rhs.m_error;
    m_manual     = rhs.m_manual;

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
                } while( i < static_cast<int>(dot) && splitPos != -1 );
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

	int lastSlash = rDescription.directory.lastIndexOf('/');
	rDescription.filename  = rDescription.directory.right( rDescription.directory.length() - lastSlash - 1 );
	rDescription.directory = rDescription.directory.left( lastSlash );
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

void KRenameFile::loadPreviewIcon()
{
    m_icon = KRenamePreviewProvider::Instance()->loadIcon( m_src.url );
}

int KRenameFile::dots() const
{
    int dots = 0;

    dots += m_src.filename.count( '.' );
    dots += m_src.extension.count( '.' );
    
    if( !m_src.extension.isEmpty() )
        ++dots;

    return dots;
}

const KUrl KRenameFile::srcUrl() const 
{
    if( m_overrideDir.isNull() )
	return m_src.url;
    else
    {
	KUrl changed = m_src.url;
	changed.setDirectory( m_overrideDir );
	QString filename = m_src.filename;
	if( !m_src.extension.isEmpty() )
	{
	    filename += '.';
	    filename += m_src.extension;
	}

	changed.setFileName( filename );
	return changed;
    }
}


