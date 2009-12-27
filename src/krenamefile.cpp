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


const int KRenameFile::DEFAULT_ICON_SIZE = 64;
const char* KRenameFile::EXTRA_DATA_KEY = "KRenameFile::EXTRA_DATA_KEY";

KRenameFile::KRenameFile( const KUrl & src, ESplitMode eSplitMode, unsigned int dot )
    : m_bValid( false ), m_error( 0 ), m_manualMode( eManualChangeMode_None )
{
    KIO::UDSEntry entry;
    KIO::NetAccess::stat( src, entry, NULL );
    KFileItem file( entry, src );

    m_bValid     = file.isReadable();
    m_bDirectory = file.isDir();

    m_fileItem = file;
    initFileDescription( m_src, src, eSplitMode, dot );
}

KRenameFile::KRenameFile( const KUrl & src, bool directory, ESplitMode eSplitMode, unsigned int dot )
    : m_bDirectory( directory ), m_bValid( true ), m_error( 0 ), m_manualMode( eManualChangeMode_None )
{
    initFileDescription( m_src, src, eSplitMode, dot );
}

KRenameFile::KRenameFile( const KFileItem& item, ESplitMode eSplitMode, unsigned int dot )
    : m_bDirectory( item.isDir() ), m_bValid( item.isReadable() ), m_error( 0 ), m_manualMode( eManualChangeMode_None )
{
    m_fileItem = item;

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
    m_manualMode = rhs.m_manualMode;

    return *this;
}

bool KRenameFile::operator==( const KFileItem & item ) const
{
    return this->fileItem() == item;
}

void KRenameFile::setCurrentSplitMode( ESplitMode eSplitMode, unsigned int dot )
{
    KUrl    url      = m_src.url;
    QString filename = m_src.filename;
    if( !m_src.extension.isEmpty() ) 
    {
        filename = filename + "." + m_src.extension;
    }

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
        else if( eSplitMode == eSplitMode_NoExtension )
            splitPos = file.length();
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
        rDescription.extension = "";
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
        rDescription.extension = "";
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

const KFileItem & KRenameFile::fileItem() const
{
    if( m_fileItem.isNull() )
    {
        // No file item has been constructed
        // create one first.
        KIO::UDSEntry entry;
        KIO::NetAccess::stat( m_src.url, entry, NULL );
        KFileItem file( entry, m_src.url );
        
        const_cast<KRenameFile*>(this)->m_fileItem = file;
    }

    /*
    // Update extra as often as possible
    // As the address is changed through sorting and moving files
    // It is only valid if no moving of data has happened!
    const_cast<KRenameFile*>(this)->m_fileItem.setExtraData( KRenameFile::EXTRA_DATA_KEY, 
                                                             const_cast<KRenameFile*>(this) );
    */
    return m_fileItem;
}
