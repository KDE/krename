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
    if( m_listDirnames ) 
    {
        QString name = m_dirname.fileName();
        if( !m_listHiddenFiles && !name.startsWith(".") )        
            m_model->addFile( KRenameFile( m_dirname, true ) );
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
    /*
    if( m_recursive ) 
    {
        m_reclister = new KRecursiveLister();
        
        m_reclister->setShowingDotFiles( m_hidden );
        m_reclister->setNameFilter( m_filter );
        m_reclister->setDirOnlyMode( m_dironly );
        m_reclister->setMainWindow( m_cache );
                
        connect( m_reclister, SIGNAL( completed() ), SLOT( reclisterFinished() ) );
        connect( m_reclister, SIGNAL( newItems( const KFileItemList& ) ), SLOT( newItems( const KFileItemList& ) ) );
        
        m_reclister->openUrl( m_dirname );
    } 
    else 
    {
        m_lister = new KDirLister();
    
        m_lister->setAutoUpdate( false );
        m_lister->setShowingDotFiles( m_hidden );
        m_lister->setNameFilter( m_filter );
        m_lister->setMainWindow( m_cache );
    
        connect( m_lister, SIGNAL( completed() ), this, SLOT( listerFinished() ) );
        connect( m_lister, SIGNAL( newItems( const KFileItemList& ) ), this, SLOT( newItems( const KFileItemList& ) ) );

        m_lister->openUrl( m_dirname, false, false );    
    }
    */
}

void ThreadedLister::foundItem(KIO::Job*, const KIO::UDSEntryList & list)
{
    QString displayName;
    QRegExp filter( m_filter );
    filter.setPatternSyntax( QRegExp::Wildcard );
    
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
            if( (m_listDirnames || m_listDirnamesOnly) && (*it).isDir() ) 
            {
                // Filter out parent and current directory
                if( displayName != "." && displayName != ".." )
                {
                    s_mutex.lock();
                    m_model->addFile( KRenameFile( KFileItem( *it, (*it).stringValue( KIO::UDSEntry::UDS_URL )) ));             
                    s_mutex.unlock();
                }
            }
            else if( !m_listDirnamesOnly && !(*it).isDir() )
            {
                s_mutex.lock();
                m_model->addFile( KRenameFile( KFileItem( *it, (*it).stringValue( KIO::UDSEntry::UDS_URL )) ));             
                s_mutex.unlock();
            }
 
            ++it;
        }
    }
}

void ThreadedLister::completed()
{
    emit listerDone( this );
}

#include "threadedlister.moc"
