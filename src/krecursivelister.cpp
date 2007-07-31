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

KRecursiveLister::KRecursiveLister(QObject *parent ) : QObject(parent) {
    lister = 0L;
    filelist.clear();
    dirlist.clear();
    dirtree.clear();
    m_hidden = false;
    m_dirs = false;
    m_filter = QString::null;
}

KRecursiveLister::~KRecursiveLister(){
    delete lister;

    while (!allItems.isEmpty())
     delete allItems.takeFirst();
}

void KRecursiveLister::cleanUp()
{
    filelist.clear();
    dirlist.clear();
}
/** Starts listing the specified url */
void KRecursiveLister::openUrl(const KUrl& url ){
    filelist.clear();
    dirlist.clear();
    startListing(url);
}

/** Returns the list of fileitems found. */
const KFileItemList & KRecursiveLister::items(){
    return filelist;
}

/** handles completion of a listing. */
void KRecursiveLister::slotListingComplete(){
    KFileItemList templist=lister->items();

    KFileItem * newitem;

    KFileItemList::const_iterator it = templist.begin();
    while( it != templist.end() )
    {
        if ((*it)->isDir()) {
            newitem= new KFileItem(*(*it));
            dirlist.append(newitem);//Used for recursing the directories
            dirtree.append(newitem);//Returned to user on request.
            allItems.append(newitem);
        }
        else {
            qDebug("GOT: %s", (*it)->url().url().toLatin1().data());

            newitem= new KFileItem(*(*it));
            filelist.append(newitem);
            allItems.append(newitem);
        }

        ++it;
    }

    QTimer::singleShot( 0, this, SLOT( listNextDirectory() ));
}

/** Starts listing the specified url */
void KRecursiveLister::startListing(const KUrl& url){
    if (!lister) {
        lister=new KDirLister();
        lister->setShowingDotFiles( m_hidden );
        lister->setNameFilter( m_filter );
        lister->setDirOnlyMode( m_dirs );
        connect(lister,SIGNAL(completed()), this, SLOT(slotListingComplete()) );
    }


    qDebug("REC: listing: %s", url.url().toLatin1().data());
    lister->openUrl( url, false, false );
}

void KRecursiveLister::listNextDirectory()
{
    qDebug("REC: listNextDirectory()");
    if ( dirlist.isEmpty() )
        emit completed();
    else
    {
        KFileItem * nextdir=dirlist.last();
        KUrl url = nextdir->url();
        dirlist.removeLast();
        startListing( url );
    }
}

/** Stops the listing */
void KRecursiveLister::stop(){
    lister->stop();
}

/** Returns the subdirectories found by the listing */
const KFileItemList& KRecursiveLister::dirs(){
    return dirtree;
}
#include "krecursivelister.moc"
