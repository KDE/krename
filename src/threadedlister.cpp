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

#include "krecursivelister.h"
#include "krenamemodel.h"

#include <kapplication.h>
#include <QMutex>

QMutex ThreadedLister::s_mutex;

ThreadedLister::ThreadedLister( QWidget* cache, KRenameModel* model )
 : QObject( NULL ), m_cache( cache ), m_model( model )
{
    m_reclister = NULL;
    m_lister = NULL;

    m_hidden = false;
    m_recursive = false;
    m_dirnames = false;
    m_dironly = false;

    qRegisterMetaType<KFileItemList>("KFileItemList");
}

ThreadedLister::~ThreadedLister()
{
    if( m_reclister )
        delete m_reclister;
        
    if( m_lister )
        delete m_lister;
}

void ThreadedLister::run()
{
    s_mutex.lock();
    if( m_dirnames ) 
    {
        QString name = m_dirname.fileName();
        if( !m_hidden && !name.startsWith(".") )        
            m_model->addFile( KRenameFile( m_dirname, true ) );
    }
    s_mutex.unlock();

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
}

void ThreadedLister::newItems( const KFileItemList& items )
{
    s_mutex.lock();
        
    if( m_dirnames ) 
    {
        QString name = m_dirname.fileName();
        if( !m_hidden && name.right( 1 ) != QString::fromLatin1(".") )        
            m_model->addFile( KRenameFile( m_dirname, true ) );
    }
    
    KFileItemList::const_iterator it = items.begin();
    while( it != items.end() ) 
    {
        if( !(*it)->isDir() || ((*it)->isDir() && m_dirnames ) )
            m_model->addFile( KRenameFile( *(*it) ) );
        ++it;
    }

    s_mutex.unlock();
}

void ThreadedLister::reclisterFinished()
{
    emit listerDone( this );
}

void ThreadedLister::listerFinished()
{
    newItems( m_lister->items( KDirLister::FilteredItems ) );

    emit listerDone( this );
}

#include "threadedlister.moc"
