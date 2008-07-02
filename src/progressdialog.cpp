/***************************************************************************
                        progressdialog.cpp  -  description
                             -------------------
    begin                : Sun Jul 1 2007
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

#include "progressdialog.h"

#include "batchrenamer.h"
#include "krenameimpl.h"

#include <QMenu>

#include <kiconloader.h>
#include <krun.h>

ProgressDialog::ProgressDialog( QWidget* parent ) 
    : QDialog( parent ), m_canceled( false ), m_renamer( NULL )
{
    m_widget.setupUi( this );

    m_buttonMore = m_widget.buttonBox->addButton( i18n("&Rename more..."), QDialogButtonBox::ActionRole );
    m_buttonUndo = m_widget.buttonBox->addButton( i18n("&Undo"), QDialogButtonBox::ActionRole );
    m_buttonDest = m_widget.buttonBox->addButton( i18n("&Open Destination..."), QDialogButtonBox::ActionRole );

    m_buttonUndo->setEnabled( false );
    m_buttonMore->setEnabled( false );

    connect( m_widget.buttonCancel, SIGNAL(clicked(bool)), SLOT(slotCancelled()));
    connect( m_buttonDest,          SIGNAL(clicked(bool)), SLOT(slotOpenDestination()));
    connect( m_buttonUndo,          SIGNAL(clicked(bool)), SLOT(slotUndo()));

    QMenu* menu = new QMenu( this );
    menu->addAction( i18n("Restart &KRename..."), this, SLOT(slotRestartKRename()) );
    menu->addSeparator();
    m_actProcessed   = menu->addAction( i18n("Rename Processed Files &Again..."),    this, SLOT(slotRenameProcessedAgain()));
    m_actUnprocessed = menu->addAction( i18n("Rename &Unprocessed Files &Again..."), this, SLOT(slotRenameUnprocessedAgain()));
    menu->addAction( i18n("&Rename All Files Again..."),          this, SLOT(slotRenameAllAgain()));

    m_buttonMore->setMenu( menu );
}

void ProgressDialog::slotCancelled()
{
    m_canceled = true;
}

void ProgressDialog::slotOpenDestination()
{
    new KRun( m_dest, this );
}

void ProgressDialog::slotRestartKRename()
{
    (void*)KRenameImpl::launch( QRect( 0, 0, 0, 0 ), KRenameFile::List() );
    QDialog::done( 0 );
}

void ProgressDialog::slotRenameProcessedAgain()
{
    KRenameFile::List list;
    KRenameFile::List::const_iterator it = m_renamer->files()->begin();

    list.reserve( m_renamer->files()->count() );
    while( it != m_renamer->files()->end() )
    {
        if( !(*it).hasError() )
        {
            KRenameFile file( m_renamer->buildDestinationUrl( *it ), (*it).isDirectory() );
            file.setIcon( file.icon() );
            list.append( file );
        }

        ++it;
    }

    (void*)KRenameImpl::launch( QRect( 0, 0, 0, 0 ), list );
    QDialog::done( 0 );
}

void ProgressDialog::slotRenameUnprocessedAgain()
{
    KRenameFile::List list;
    KRenameFile::List::const_iterator it = m_renamer->files()->begin();

    list.reserve( m_renamer->files()->count() );
    while( it != m_renamer->files()->end() )
    {
        if( (*it).hasError() )
        {
            KRenameFile file( *it );
            file.setManualChanges( QString::null ); // reset manual changes!!
            list.append( file );
        }

        ++it;
    }

    (void*)KRenameImpl::launch( QRect( 0, 0, 0, 0 ), list );
    QDialog::done( 0 );
}

void ProgressDialog::slotRenameAllAgain()
{
    KRenameFile::List list;
    KRenameFile::List::const_iterator it = m_renamer->files()->begin();

    list.reserve( m_renamer->files()->count() );
    while( it != m_renamer->files()->end() )
    {
        KRenameFile file( m_renamer->buildDestinationUrl( *it ), (*it).isDirectory() );
        file.setIcon( file.icon() );
        list.append( file );
        ++it;
    }


    (void*)KRenameImpl::launch( QRect( 0, 0, 0, 0 ), list );
    QDialog::done( 0 );
}

void ProgressDialog::slotUndo()
{
    if( m_renamer )
    {
        m_widget.buttonCancel->setEnabled( true );

        m_buttonUndo->setEnabled( false );
        m_buttonMore->setEnabled( false );

        m_renamer->undoFiles( this );
    }
}

void ProgressDialog::renamingDone( bool enableUndo, BatchRenamer* renamer, int errors )
{
    m_widget.buttonCancel->setEnabled( false );

    m_buttonUndo->setEnabled( enableUndo );
    m_buttonMore->setEnabled( true );

    m_actProcessed->setEnabled  ( renamer->files()->count() != errors );
    m_actUnprocessed->setEnabled( 0 != errors );

    m_renamer = renamer;
} 

void ProgressDialog::print( const QString & text, const QString & pixmap )
{
    QPixmap icon = SmallIcon(pixmap);

    new QListWidgetItem( icon, text, m_widget.display );
}

void ProgressDialog::error( const QString & text )
{
    this->print( text, "cancel" );
}

void ProgressDialog::warning( const QString & text )
{
    this->print( text, "help-hint" );
}

#include "progressdialog.moc"
