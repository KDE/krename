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

ThreadedLister::ThreadedLister( KRenameModel* model )
 : QThread(), m_model( model )
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

    if( false && m_recursive ) 
    {
        m_reclister = new KRecursiveLister();
        
        m_reclister->setShowingDotFiles( m_hidden );
        m_reclister->setNameFilter( m_filter );
        m_reclister->setDirOnlyMode( m_dironly );
        //m_reclister->setMainWindow( NULL ); // TODO
                
        connect( m_reclister, SIGNAL( completed() ), SLOT( reclisterFinished() ) );
        connect( m_reclister, SIGNAL( newItems( const KFileItemList& ) ), SLOT( newItems( const KFileItemList& ) ) );
        
        m_reclister->openUrl( m_dirname );
    } else {
        m_lister = new KDirLister();
    
        m_lister->setAutoUpdate( false );
        m_lister->setShowingDotFiles( m_hidden );
        m_lister->setNameFilter( m_filter );
        //m_lister->setMainWindow( NULL ); // TODO

        //connect( m_lister, SIGNAL( completed() ), this, SLOT( listerFinished() ) );
        connect( m_lister, SIGNAL( newItems( const KFileItemList& ) ), this, SLOT( newItems( const KFileItemList& ) ) );

        qDebug("LISTER: openUrl %s", m_dirname.url().toLatin1().data() );
        m_lister->openUrl( m_dirname, false, false );    
    }
    
    // This will block run as long as *listerFinished() does not exit 
    //qDebug("Thread exec started");
    //this->exec();
    
    //emit listerDone( this );
}

void ThreadedLister::reclisterFinished()
{
    KFileItemList list = m_reclister->items();
    if( m_dirnames ) 
        list += m_reclister->dirs();

    qSort( list );

    s_mutex.lock();
 
    if( m_dirnames ) 
    {
        QString name = m_dirname.fileName();
        if( !m_hidden && name.right( 1 ) != QString::fromLatin1(".") )        
            m_model->addFile( KRenameFile( m_dirname, true ) );
    }
    
    KFileItemList::const_iterator it = list.begin();
    while( it != list.end() ) 
    {
        qDebug("adding file");
        m_model->addFile( KRenameFile( *(*it) ) );
        ++it;
    }
            
    s_mutex.unlock();
    
    qDebug("exit rec");
    emit listerDone( this );
    this->exit( 0 );
}


void ThreadedLister::newItems( const KFileItemList& items )
{
    qDebug("LISTER: Got new items of length: %i", items.count());
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
        m_model->addFile( KRenameFile( *(*it) ) );
        ++it;
    }
    
    s_mutex.unlock();
}


void ThreadedLister::listerFinished()
{
    qDebug("LISTER: DONE %i", m_lister->isFinished());
    
    newItems( m_lister->items( KDirLister::FilteredItems ) );

    this->exit( 0 );
    emit listerDone( this );
    return;

    /*
    if( m_lister->isFinished() ) {
        qDebug("finished!");
        KFileItemList list = m_lister->items( KDirLister::FilteredItems );
        qSort( list );

        s_mutex.lock();
        
        KFileItemList::const_iterator it = list.begin();
        while( it != list.end() ) 
        {
            m_model->addFile( KRenameFile( *(*it) ) );
            ++it;
        }

        s_mutex.unlock();
    
        qDebug("exit lister");
        this->exit( 0 );
    }
    else
        qDebug("Lister not finished!");
    */
}

#include "threadedlister.moc"
