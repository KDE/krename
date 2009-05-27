/***************************************************************************

                          threadedlister.cpp  -  description
                             -------------------
    begin                : Tue Feb 01 2005
    copyright            : (C) 2005 by Dominik Seichter
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
 
#include "threadedlister.h"

#include "krenamemodel.h"

#include <kio/job.h>
#include <kio/jobclasses.h>

#include <kapplication.h>
#include <QMutex>
#include <QRegExp>

QMutex ThreadedLister::s_mutex;

ThreadedLister::ThreadedLister( const KUrl & dirname, QWidget* cache, KRenameModel* model )
    : QObject( NULL ), m_dirname( dirname ), m_cache( cache ), m_model( model )
{
    m_listHiddenFiles  = false;
    m_listRecursive    = false;
    m_listDirnamesOnly = false;
    m_listDirnames     = false;

    qRegisterMetaType<KFileItemList>("KFileItemList");
}

ThreadedLister::~ThreadedLister()
{
}

void ThreadedLister::run()
{
    s_mutex.lock();
    if( m_listDirnames || m_listDirnamesOnly ) 
    {
        QString name = m_dirname.fileName();
        if( !m_listHiddenFiles && !name.startsWith(".") )        
        {
            KRenameFile::List list;
            list.append( KRenameFile( m_dirname, true ) );

            m_model->addFiles( list );
        }
    }
    s_mutex.unlock();

    KIO::ListJob* job   = NULL; // Will delete itself automatically
    KIO::JobFlags flags = KIO::HideProgressInfo;
    if( m_listRecursive ) 
        job = KIO::listRecursive( m_dirname, flags, m_listHiddenFiles );
    else
        job = KIO::listDir( m_dirname, flags, m_listHiddenFiles );

    connect( job, SIGNAL(entries( KIO::Job*, const KIO::UDSEntryList & )), SLOT(foundItem(KIO::Job*, const KIO::UDSEntryList &)));
    connect( job, SIGNAL(result( KJob* )), SLOT( completed() ) );

    job->start();
}

void ThreadedLister::foundItem(KIO::Job*, const KIO::UDSEntryList & list)
{
    QString displayName;
    QRegExp filter( m_filter );
    filter.setPatternSyntax( QRegExp::Wildcard );

    m_files.reserve( m_files.count() + list.count() ); 
    
    KIO::UDSEntryList::const_iterator it = list.begin();
    while( it != list.end() ) 
    {
        displayName = (*it).stringValue( KIO::UDSEntry::UDS_NAME );
        if( !filter.isEmpty() && !filter.exactMatch( displayName ) )
        {
            // does not match filter
            // skip it 
            ++it;
        }
        else
        {
            KUrl url = m_dirname;
            url.addPath( displayName ); // displayName is a relative path

            if( (m_listDirnames || m_listDirnamesOnly) && (*it).isDir() ) 
            {
                // Filter out parent and current directory
                if( displayName != "." && displayName != ".." )
                    m_files.append( KRenameFile( KFileItem( *it, url ) ) );
            }
            else if( !m_listDirnamesOnly && !(*it).isDir() )
            {
                m_files.append( KRenameFile( KFileItem( *it, url ) ) );
            }
 
            ++it;
        }
    }
}

void ThreadedLister::completed()
{
    if( m_files.count() > 0 ) 
    {
        // We add the files in the completed slot
        // and not directly in the foundItem slot,
        // as the latter can produce deadlocks if
        // we get a signal while we keep the mutex!
        s_mutex.lock();
        m_model->addFiles( m_files );
        s_mutex.unlock();
    }

    emit listerDone( this );
}

#include "threadedlister.moc"
