/***************************************************************************
                       replacedialog.cpp  -  description
                             -------------------
    begin                : Sun Jul 22 2007
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

#include "replacedialog.h"

#include "ui_replaceitemdlg.h"
#include "batchrenamer.h"

ReplaceDialog::ReplaceDialog( const QList<TReplaceItem> & items, QWidget* parent )
    : QDialog( parent )
{
    m_widget.setupUi( this );

    connect(m_widget.buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_widget.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(m_widget.buttonAdd,    SIGNAL(clicked(bool)),          SLOT(slotAdd()));
    connect(m_widget.buttonRemove, SIGNAL(clicked(bool)),          SLOT(slotRemove()));
    connect(m_widget.buttonEdit,   SIGNAL(clicked(bool)),          SLOT(slotEdit()));

    connect(m_widget.list, SIGNAL(itemSelectionChanged()),         SLOT(slotEnableControls()));

    this->slotEnableControls();

    QList<TReplaceItem>::const_iterator it = items.begin();
    while( it != items.end() )
    {
        int row = m_widget.list->rowCount();
        m_widget.list->setRowCount( row + 1 );

        m_widget.list->setItem( row, 0, this->createTableItem( "", true ) );
        m_widget.list->item( row, 0 )->setCheckState( (*it).reg ? Qt::Checked : Qt::Unchecked ); 
        m_widget.list->setItem( row, 1, this->createTableItem( (*it).find ) );
        m_widget.list->setItem( row, 2, this->createTableItem( (*it).replace ) );
        m_widget.list->setItem( row, 3, this->createTableItem( "", true ) );
        m_widget.list->item( row, 3 )->setCheckState( (*it).doProcessTokens ? Qt::Checked : Qt::Unchecked ); 

        ++it;
    }
}

void ReplaceDialog::slotAdd()
{
    QDialog dlg;
    Ui::ReplaceItemDlg replace;
    replace.setupUi( &dlg );

    if( dlg.exec() == QDialog::Accepted ) 
    {
        int row = m_widget.list->rowCount();
        m_widget.list->setRowCount( row + 1 );
        m_widget.list->setItem( row, 0, this->createTableItem( "", true ) );
        m_widget.list->item( row, 0 )->setCheckState( replace.checkRegular->isChecked() ? Qt::Checked : Qt::Unchecked ); 
        m_widget.list->setItem( row, 1, this->createTableItem( replace.lineFind->text() ) );
        m_widget.list->setItem( row, 2, this->createTableItem( replace.lineReplace->text() ) );
        m_widget.list->setItem( row, 3, this->createTableItem( "", true ) );
        m_widget.list->item( row, 3 )->setCheckState( replace.checkProcess->isChecked() ? Qt::Checked : Qt::Unchecked ); 
    }
}

void ReplaceDialog::slotEdit()
{
    QDialog dlg;
    Ui::ReplaceItemDlg replace;
    replace.setupUi( &dlg );

    int row = m_widget.list->currentRow();
    replace.checkRegular->setChecked( m_widget.list->item( row, 0 )->checkState() == Qt::Checked );
    replace.lineFind->setText( m_widget.list->item( row, 1 )->text() );
    replace.lineReplace->setText( m_widget.list->item( row, 2 )->text() );
    replace.checkProcess->setChecked( m_widget.list->item( row, 3 )->checkState() == Qt::Checked );

    if( dlg.exec() == QDialog::Accepted ) 
    {
        m_widget.list->setItem( row, 0, this->createTableItem( "", true ) );
        m_widget.list->item( row, 0 )->setCheckState( replace.checkRegular->isChecked() ? Qt::Checked : Qt::Unchecked ); 
        m_widget.list->setItem( row, 1, this->createTableItem( replace.lineFind->text() ) );
        m_widget.list->setItem( row, 2, this->createTableItem( replace.lineReplace->text() ) );
        m_widget.list->setItem( row, 3, this->createTableItem( "", true ) );
        m_widget.list->item( row, 3 )->setCheckState( replace.checkProcess->isChecked() ? Qt::Checked : Qt::Unchecked ); 
    }
}

void ReplaceDialog::slotRemove()
{
    m_widget.list->removeRow( m_widget.list->currentRow() );
}

void ReplaceDialog::slotEnableControls()
{
    QList<QTableWidgetItem*> selected = m_widget.list->selectedItems();

    m_widget.buttonEdit->setEnabled( selected.count() );
    m_widget.buttonRemove->setEnabled( selected.count() );
}

QTableWidgetItem* ReplaceDialog::createTableItem( const QString & text, bool isCheckable )
{
    // TODO: Enclose text in quotes, but remove them before the item is edited by the user directly in the table
    //       and add them again if the user presses enter.
    QString t = text; // text.isEmpty() ? QString::null : "\"" + text + "\"";
    QTableWidgetItem* item = new QTableWidgetItem( t );

    if( isCheckable )
        item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
    else
        item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled );

    return item;
}

QList<TReplaceItem> ReplaceDialog::replaceList() const
{
    QList<TReplaceItem> items;

    for( int i=0;i<m_widget.list->rowCount(); i++ ) 
    {
        TReplaceItem item;
        item.reg     = m_widget.list->item( i, 0 )->checkState() == Qt::Checked;
        item.find    = m_widget.list->item( i, 1 )->text();
        item.replace = m_widget.list->item( i, 2 )->text();
        item.doProcessTokens = m_widget.list->item( i, 3 )->checkState() == Qt::Checked;
        items.append( item );
    }

    return items;
}


#include "replacedialog.moc"

