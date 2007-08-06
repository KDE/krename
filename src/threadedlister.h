/***************************************************************************
                          
                          threadedlister.h  -  description
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
 
#ifndef THREADEDLISTER_H
#define THREADEDLISTER_H

#include <QThread>
#include <QMutex>

#include <kfileitem.h>

class KDirLister;
class KRecursiveLister;
class KRenameModel;
class QMutex;
class QWidget;

/*
class FileList : public KFileItemList {
    public:
        FileList( const KFileItemList & list ) 
            {
                KFileItem* it;
                for( it = list.first(); it; it = list.next() )  
                    this->append( it );
            }

    protected:
        int compareItems( QPtrCollection::Item item1, QPtrCollection::Item item2 ) {
            return static_cast<KFileItem*>(item1)->url().url().compare( static_cast<KFileItem*>(item2)->url().url() );
        }
        
};
*/

class ThreadedLister : public QObject
{
    Q_OBJECT
    public:
        ThreadedLister( QWidget* cache, KRenameModel* model );
        ~ThreadedLister();
                
        inline void start() { this->run(); }
        inline void exit( int v = 0 ) { return; }

        inline const KUrl & dirname();
        inline bool dirnames();
        inline const QString & filter();
        inline bool hidden();
        
        inline void setDirname( const KUrl & dirname );
        inline void setDirnames( bool names );        
        inline void setFilter( const QString & filter );
        inline void setHidden( bool h );
        inline void setRecursive( bool r );
        inline void setRecursiveDirOnlyMode( bool m );
        
    signals:
        void listerDone( ThreadedLister* );
        
    protected:
        void run();
        
    private slots:
        void reclisterFinished();
        void listerFinished();
        
        void newItems( const KFileItemList& items );

    private:        
        static QMutex     s_mutex; ///< Mutex assuring that only one thread at a time will work on m_model
        
        KUrl              m_dirname;
        QString           m_filter;
        bool              m_hidden;
        bool              m_recursive;
        bool              m_dirnames;
        bool              m_dironly;

        KUrl::List        m_files;
                
        KDirLister*       m_lister;
        KRecursiveLister* m_reclister;
      
        QWidget*          m_cache;
        KRenameModel*     m_model;
};

void ThreadedLister::setDirname( const KUrl & dirname )
{
    m_dirname = dirname;
}

void ThreadedLister::setDirnames( bool names )
{
    m_dirnames = names;
}

void ThreadedLister::setFilter( const QString & filter )
{
    m_filter = filter;
}

void ThreadedLister::setHidden( bool h )
{
    m_hidden = h;
}

void ThreadedLister::setRecursive( bool r )
{
    m_recursive = r;
}

void ThreadedLister::setRecursiveDirOnlyMode( bool m )
{
    m_dironly = m;
}

/*
KUrl::List* ThreadedLister::items()
{
    return &m_files;
}
*/
const KUrl & ThreadedLister::dirname()
{
    return m_dirname;
}

bool ThreadedLister::dirnames()
{
    return m_dirnames;
}

const QString & ThreadedLister::filter()
{
    return m_filter;
}

bool ThreadedLister::hidden()
{
    return m_hidden;
}

#endif
