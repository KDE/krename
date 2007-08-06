/***************************************************************************
                          krecursivelister.h  -  description
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

#ifndef KRECURSIVELISTER_H
#define KRECURSIVELISTER_H

#include <QObject>
#include <QPointer>

#include <kdirlister.h>
#include <kfileitem.h>

class QWidget;

/** A convienience class that recursively lists a directory
 * @author Jonathon Sim
 *
 * Modified by Dominik Seichter to support a name filter,
 * dir only mode, support for showing hidden files on demand
 * and support for listing directories along with files.
 *
 *
 * Ported to KDE4 by Dominik Seichter
 */
class KRecursiveLister : public QObject  {
   Q_OBJECT

public:
        KRecursiveLister(QObject *parent=NULL);
	~KRecursiveLister();

        inline void setMainWindow( QWidget* cache ) { m_cache = cache; }

        /** sets wether hidden files shall be listed */
        inline void setShowingDotFiles( bool dotfiles );
        
        /** filter to be used */
        inline void setNameFilter( const QString & filter );

        /** list only directories */
        inline void setDirOnlyMode( bool dirsOnly );

        /** Starts listing the specified url */
	void openUrl(const KUrl & url);
	
        /** Stops the listing */
	void stop();

 signals: // Signals
	/** Listing is complete */
	void completed();
        void newItems( const KFileItemList& items );

protected slots: // Protected slots
    
        /** handles completion of a listing. */
        void slotListingComplete();
        void listNextDirectory();
        void slotNewItems( const KFileItemList& items );

protected: // Protected methods
	/** Starts listing the specified url */
	void startListing(const KUrl & url);
	
	//Protected variables
	KFileItemList	dirlist;	//Dirs remaining to list
	QPointer<KDirLister>		lister;			//The current KDirLister

        bool m_hidden;
        bool m_dirs;
        QString m_filter;

        QWidget* m_cache;
};

void KRecursiveLister::setShowingDotFiles( bool dotfiles ) 
{
    m_hidden = dotfiles; 
}
        
void KRecursiveLister::setNameFilter( const QString & filter ) 
{ 
    m_filter = filter; 
}

void KRecursiveLister::setDirOnlyMode( bool dirsOnly ) 
{ 
    m_dirs = dirsOnly; 
}


#endif
