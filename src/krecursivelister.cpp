/***************************************************************************
                          krecursivelister.cpp  -  description
                             -------------------
    begin                : Fri Aug 31 2001
    copyright            : (C) 2001 by Jonathon Sim
    email                : jonathonsim@iname.com
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qtimer.h>

#include "krecursivelister.h"

KRecursiveLister::KRecursiveLister(QObject *parent ) : QObject(parent), m_cache( NULL ) {
    lister = 0L;
    m_hidden = false;
    m_dirs = false;
    m_filter = QString::null;
}

KRecursiveLister::~KRecursiveLister(){
    delete lister;
}


/** Starts listing the specified url */
void KRecursiveLister::openUrl(const KUrl& url ){
    dirlist.clear();
    startListing(url);
}

void KRecursiveLister::slotNewItems( const KFileItemList& items )
{
    KFileItem * newitem;

    qDebug("??? Got Rec items: %i", items.count() );

    KFileItemList::const_iterator it = items.begin();
    while( it != items.end() )
    {
        if ((*it)->isDir()) {
            newitem= new KFileItem(*(*it));
            dirlist.append(newitem);//Used for recursing the directories
        }

        ++it;
    }

    emit newItems( items );
}

/** handles completion of a listing. */
void KRecursiveLister::slotListingComplete(){
    this->slotNewItems( lister->items( KDirLister::FilteredItems ) );
    QTimer::singleShot( 0, this, SLOT( listNextDirectory() ));
}

/** Starts listing the specified url */
void KRecursiveLister::startListing(const KUrl& url){
    if (!lister) {
        lister=new KDirLister();
        lister->setShowingDotFiles( m_hidden );
        lister->setNameFilter( m_filter );
        //lister->setDirOnlyMode( m_dirs );
        lister->setMainWindow( m_cache ); 
        lister->setAutoUpdate( false );

        //connect(lister,SIGNAL(newItems(const KFileItemList&)), this, SLOT(slotNewItems(const KFileItemList&)));
        connect(lister,SIGNAL(completed()), this, SLOT(slotListingComplete()) );
    }

    lister->openUrl( url, false, false );
}

void KRecursiveLister::listNextDirectory()
{
    if ( dirlist.isEmpty() )
        emit completed();
    else
    {
        KFileItem * nextdir=dirlist.last();
        KUrl url = nextdir->url();
        dirlist.removeLast();
        startListing( url );
        delete nextdir;
    }
}

/** Stops the listing */
void KRecursiveLister::stop(){
    lister->stop();
}

#include "krecursivelister.moc"
