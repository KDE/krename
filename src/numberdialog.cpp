/***************************************************************************
                       numberdialog.h  -  description
                             -------------------
    begin                : The May 24 2007
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

#include "numberdialog.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>

NumberDialog::NumberDialog( int start, int step, bool reset, QList<int> skip, QWidget* parent )
    : QDialog( parent )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    QWidget*     widget = new QWidget( this );

    m_widget.setupUi( widget );
    m_buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this );

    layout->addWidget( widget );
    layout->addWidget( m_buttons );

    connect(m_buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));

    connect(m_widget.buttonAdd,    SIGNAL(clicked(bool)),          SLOT(slotAddNumber()));
    connect(m_widget.buttonRemove, SIGNAL(clicked(bool)),          SLOT(slotRemoveNumber()));
    connect(m_widget.listSkip,     SIGNAL(itemSelectionChanged()), SLOT(slotEnableControls()));
    m_widget.spinStart->setValue( start );
    m_widget.spinStep->setValue( step );
    m_widget.checkReset->setChecked( reset );
    m_widget.listSkip->setSortingEnabled( true ); // TODO: sort numeric

    QList<int>::ConstIterator it = skip.begin();
    while( it != skip.end() ) 
    {
        m_widget.listSkip->addItem( QString::number( *it ) );
        ++it;
    }
}

QList<int> NumberDialog::skipNumbers() const
{
    QList<int> list;

    for( int i = 0; i < m_widget.listSkip->count(); i++ ) 
    {
        QListWidgetItem* item = m_widget.listSkip->item( i );
        list.append( item->data( Qt::DisplayRole ).toInt() );
        
    }
    return list;
}

void NumberDialog::slotEnableControls()
{
    QList<QListWidgetItem*> selected = m_widget.listSkip->selectedItems();
    m_widget.buttonRemove->setEnabled( selected.size() ); 
}

void NumberDialog::slotAddNumber()
{
    m_widget.listSkip->addItem( QString::number( m_widget.spinNumber->value() ) );
}

void NumberDialog::slotRemoveNumber()
{
    delete m_widget.listSkip->takeItem( m_widget.listSkip->currentRow() );
}

#include "numberdialog.moc"
