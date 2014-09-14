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

#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDomDocument>
#include <QDomElement>

#include<iostream>

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

    connect(m_widget.buttonLoadList, SIGNAL(clicked(bool)),      SLOT(slotLoadList()));
    connect(m_widget.buttonSaveList, SIGNAL(clicked(bool)),      SLOT(slotSaveList()));
    
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

void ReplaceDialog::slotSaveList()
{
      QTableWidget * table = m_widget.list;
      
      QString fileName = QFileDialog::getSaveFileName(this,tr("Save Find & Replace Settings as:"),
						      QDir::currentPath(), tr("KRename Find & Replace Settings XML (*.xml)"));
      
      if (fileName.isEmpty()) 
	    return;
      
      QFile f(fileName);  
      
      if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
      {
	    QMessageBox::warning(this, tr("KRename Warning"),
				 tr("Failed to save the Find & Replace Settings File."), QMessageBox::Cancel,
				 QMessageBox::NoButton);
	    return;
      }
      
	    QTextStream data( &f );
	    
	    
	    QDomDocument xmlDocument("KRename");
	    QDomElement root = xmlDocument.createElement("KRename");
	    root.setAttribute("version","1.0");
	    
	    QDomElement frElement = xmlDocument.createElement("FindReplaceList");
	    frElement.setAttribute("rows",table->rowCount());
	    root.appendChild(frElement);
	    
	    
	    for( int r = 0; r < table->rowCount(); ++r )
	    {
		  QDomElement RegExpElement = xmlDocument.createElement("RegularExpression");
		  QDomElement FindElement = xmlDocument.createElement("Find");
		  QDomElement ReplaceWithElement = xmlDocument.createElement("ReplaceWith");
		  QDomElement ProcessTokensElement = xmlDocument.createElement("ProcessTokens");			
		  QDomElement RowElement = xmlDocument.createElement("Row");	
		  RowElement.setAttribute("num",r);
		  
		  int RegExpCheck=static_cast<int>(table->item( r, 0)->checkState());
		  QString RegExpCheckStr=QString::number( RegExpCheck, 10 );
		  
		  RegExpElement.appendChild( xmlDocument.createTextNode(RegExpCheckStr));
		  RowElement.appendChild(RegExpElement);
		  
		  FindElement.appendChild( xmlDocument.createTextNode(table->item( r, 1)->data(Qt::EditRole).toString()  ));
		  RowElement.appendChild(FindElement);
		  ReplaceWithElement.appendChild( xmlDocument.createTextNode(table->item( r, 2)->data(Qt::EditRole).toString()  ));
		  RowElement.appendChild(ReplaceWithElement);
		  
		  int ProcessTokensCheck=static_cast<int>(table->item( r, 3)->checkState());
		  QString ProcessTokensCheckStr=QString::number( ProcessTokensCheck, 10 );
		  ProcessTokensElement.appendChild( xmlDocument.createTextNode(ProcessTokensCheckStr));
		  RowElement.appendChild(ProcessTokensElement);
		  
		  frElement.appendChild(RowElement);
		  
	    }
	    xmlDocument.appendChild(root);

	    data << xmlDocument.toString(-1);
	    f.close();

      
      
      
      
}

void ReplaceDialog::slotLoadList()
{
      QTableWidget * table = m_widget.list;
      
      QString fileName =  QFileDialog::getOpenFileName(this, tr("Save Find and Replace Settings File"),
						       QDir::currentPath(),
						       tr("KRename Find and Replace Settings XML (*.xml)"));
      if (fileName.isEmpty())
	    return;
//open file
      QFile f(fileName);
      if (!f.open(QFile::ReadOnly | QFile::Text)) 
      {
	    QMessageBox::warning(this, tr("KRename Warning"),
				 tr("Failed to open the Find and Replace Settings File. Cannot read file %1:\n%2.").arg(fileName).arg(f.errorString()), QMessageBox::Cancel,
				 QMessageBox::NoButton);
	    return;
      }
//load XML
      QIODevice * device = &f;
      QDomDocument xmlDocument;
      QString errorStr;
      int errorLine;
      int errorColumn;

      if (!xmlDocument.setContent(device, true, &errorStr, &errorLine, &errorColumn)) 
      {
	    QMessageBox::information(window(), tr("KRename Find and Replace Settings XML File"),
				     tr("Parse error at line %1, column %2:\n%3")
				     .arg(errorLine)
				     .arg(errorColumn)
				     .arg(errorStr));
	    return;
      }
	    
	    QDomElement root = xmlDocument.documentElement();
// check if valid
      if (root.tagName() != "KRename") {
	    QMessageBox::information(window(), tr("KRename Find and Replace Settings XML File"),
				     tr("The file is not an KRename XML file."));
	    return;
      } else if (root.hasAttribute("version") && root.attribute("version") != "1.0") 
      {
	    QMessageBox::information(window(), tr("KRename Find and Replace Settings XML File"),
				     tr("The file is not an KRename XML version 1.0 file."));
	    return;
      }
//parse XML file

QDomElement frElement = root.firstChild().toElement();

      int Rows;
      bool ok;
      Rows = frElement.attribute("rows").toInt(&ok, 10);
      table->setRowCount(Rows);
      int rc;
      rc=0;
      
      QDomElement RowElement = frElement.firstChildElement("Row");
      while (!RowElement.isNull()) {
	    
	    
	    QString RegExpCheckStr = RowElement.firstChildElement("RegularExpression").text();
	    int RegExpCheck=RegExpCheckStr.toInt();
	    
	    switch ( RegExpCheck ) 
	    {
		  case 0 :
			table->setItem( rc, 0, this->createTableItem( "", true ) );
			table->item( rc, 0 )->setCheckState(Qt::Unchecked);
			break;	
		  case 1 :
			table->setItem( rc, 0, this->createTableItem( "", true ) );
			table->item( rc, 0 )->setCheckState(Qt::PartiallyChecked);			
			break;
		  case 2 :
			table->setItem( rc, 0, this->createTableItem( "", true ) );
			table->item( rc, 0 )->setCheckState(Qt::Checked);
			break;
		  default :
			break;
	    }

	    table->setItem( rc, 1, this->createTableItem(RowElement.firstChildElement("Find").text()) );
	    table->setItem( rc, 2, this->createTableItem(RowElement.firstChildElement("ReplaceWith").text()) );
	    
	    QString ProcessTokensCheckStr = RowElement.firstChildElement("ProcessTokens").text();
	    int ProcessTokensCheck=ProcessTokensCheckStr.toInt();
   
	    switch ( ProcessTokensCheck ) 
	    {
		  case 0 :
			table->setItem( rc, 3, this->createTableItem( "", true ) );
			table->item( rc, 3 )->setCheckState(Qt::Unchecked);
			break;	
		  case 1 :
			table->setItem( rc, 3, this->createTableItem( "", true ) );
			table->item( rc, 3 )->setCheckState(Qt::PartiallyChecked);			
			break;
		  case 2 :
			table->setItem( rc, 3, this->createTableItem( "", true ) );
			table->item( rc, 3 )->setCheckState(Qt::Checked);
			break;
		  default :
			break;
	    }
	    
	    RowElement = RowElement.nextSiblingElement("Row");
	    rc=rc+1;
      }
      if(rc!=Rows)
      {
	    QMessageBox::information(window(), tr("KRename Find and Replace Settings XML File"),
				     tr("Problem with loading KRename XML file."));
	    return;    
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
    int row = m_widget.list->rowCount();
    m_widget.buttonSaveList->setEnabled(row);
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
    int row = m_widget.list->rowCount();
    m_widget.buttonSaveList->setEnabled(row);
}

void ReplaceDialog::slotEnableControls()
{
    QList<QTableWidgetItem*> selected = m_widget.list->selectedItems();

    m_widget.buttonEdit->setEnabled( selected.count() );
    m_widget.buttonRemove->setEnabled( selected.count() );
    
    m_widget.buttonLoadList->setEnabled(true);
    
    int row = m_widget.list->rowCount();
    m_widget.buttonSaveList->setEnabled(row);
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

