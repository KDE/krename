/***************************************************************************
                       previewlist.cpp  -  description
                             -------------------
    begin                : Sat Oct 06 2007
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

#include "previewlist.h"

#include "customdialog.h"
#include "krenamemodel.h"

#include "klocale.h"
#include "kmenu.h"

#include <QContextMenuEvent>

PreviewList::PreviewList( QWidget* parent )
    : QTreeView( parent ), m_model( NULL )
{
    m_menu = new KMenu( "KRename", this ); // we need any text here so that we have a title
    m_menu->addAction( QIcon(), i18n("&Change filename manually..."), this, SLOT(slotManually()), QKeySequence("F2") );
    m_menu->addSeparator();
    m_menu->addAction( i18n("&Open"), this, SLOT( slotOpen() ) );
    m_menu->addSeparator();
    m_menu->addAction( i18n("&Add..."), this, SIGNAL( addFiles() ) );
    m_menu->addAction( i18n("&Remove"), this, SLOT( slotRemove() ) );

    connect( this, SIGNAL( activated(const QModelIndex&)), this, SLOT( slotManually() ) );
}

PreviewList::~PreviewList()
{

}

void PreviewList::contextMenuEvent( QContextMenuEvent* e )
{
    // only show a context menu if we have model and contents
    if( m_model && m_model->rowCount() )
    {
        const KRenameFile& file  = m_model->file( this->currentIndex().row() );
        
        m_menu->setTitle( file.srcUrl().prettyUrl() );
        m_menu->popup( e->globalPos() );
    }
}

void PreviewList::slotOpen()
{
    m_model->run( this->currentIndex(), this );
}

void PreviewList::slotRemove()
{
    QList<int> list;

    list.append( this->currentIndex().row() );

    m_model->removeFiles( list );

    emit updateCount();
}

void PreviewList::slotManually()
{
    CustomDialog dialog(m_model->file( this->currentIndex().row() ), this);
    if( dialog.exec() == QDialog::Accepted ) 
    {
        QString manual = QString::null;
        EManualChangeMode mode = eManualChangeMode_None;
        if( dialog.hasManualChanges() ) 
        {
            manual = dialog.manualChanges();
            mode = dialog.manualChangeMode();
        }

        m_model->file( this->currentIndex().row() ).setManualChanges( manual, mode );
    }
}

#include "previewlist.moc"

