/***************************************************************************
                       tokenhelpdialog.cpp  -  description
                             -------------------
    begin                : Mon Jul 30 2007
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

#include "tokenhelpdialog.h"

#include <kpushbutton.h>

TokenHelpDialog::TokenHelpDialog( QLineEdit* edit, QWidget* parent )
    : KDialog( parent ), m_edit( edit )
{
    m_widget.setupUi( mainWidget() );

    this->setButtons( KDialog::Close | KDialog::User1 ); 
    this->setButtonText( KDialog::User1, i18n("&Insert") ); 

    KPushButton* insert = this->button( KDialog::User1 );
    KPushButton* close = this->button( KDialog::Close );
    
    m_widget.searchCategory->searchLine()->setTreeWidget( m_widget.listCategories );
    m_widget.searchToken   ->searchLine()->setTreeWidget( m_widget.listTokens );

    connect(insert, SIGNAL(clicked(bool)), SLOT(slotInsert()));
    connect(this, SIGNAL(rejected()), SLOT(reject()));
    connect(close, SIGNAL(clicked(bool)), SLOT(saveConfig()));

    connect(m_widget.listCategories, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(slotCategoryChanged(QTreeWidgetItem*)));
    connect(m_widget.listTokens,     SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(slotInsert()));
}

void TokenHelpDialog::add( const QString & headline, const QStringList & commands, const QPixmap & icon, bool first )
{
    m_map[headline] = commands;

    QTreeWidgetItem* item = new QTreeWidgetItem( m_widget.listCategories );
    item->setText( 0, headline );
    item->setIcon( 0, icon );

    if( first ) 
        m_first = headline;
}

int TokenHelpDialog::exec()
{
    loadConfig();

    if( !m_first.isEmpty() ) 
    {
        for( int i=0;i<m_widget.listCategories->topLevelItemCount(); i++ ) 
            if( m_widget.listCategories->topLevelItem( i )->text(0) == m_first )
            {
                m_widget.listCategories->topLevelItem( i )->setSelected( true );
                this->slotCategoryChanged( m_widget.listCategories->topLevelItem( i ) );
                break;
            }
    }

    return QDialog::exec();
}

void TokenHelpDialog::slotCategoryChanged( QTreeWidgetItem* item )
{
    m_widget.listTokens->clear();

    const QStringList & commands = m_map[item->text(0)];
    for( int i=0;i<commands.count(); i++ )
    {
        QTreeWidgetItem* item = new QTreeWidgetItem( m_widget.listTokens );
        item->setText( 0, commands[i].section(";;", 0, 0 ) );
        item->setText( 1, commands[i].section(";;", 1, 1 ) );
    }
}

void TokenHelpDialog::slotInsert()
{
    saveConfig();

    QTreeWidgetItem* item = m_widget.listTokens->currentItem();
    
    if( item ) 
        m_edit->insert( item->text( 0 ) );

    this->accept();
}

void TokenHelpDialog::loadConfig() 
{
    KSharedConfigPtr config = KGlobal::config();

    KConfigGroup groupGui = config->group( QString("TokenHelpDialog") );

    int width = groupGui.readEntry( "Column0", QVariant(m_widget.listTokens->columnWidth( 0 )) ).toInt();
    if( width > 0 )
		m_widget.listTokens->setColumnWidth( 0, width );

    width = groupGui.readEntry( "Column1", QVariant(m_widget.listTokens->columnWidth( 1 )) ).toInt();
    if( width > 0 )
	    m_widget.listTokens->setColumnWidth( 1, width );

    this->restoreDialogSize( groupGui );
}

void TokenHelpDialog::saveConfig() 
{
    KSharedConfigPtr config = KGlobal::config();

    KConfigGroup groupGui = config->group( QString("TokenHelpDialog") );

    groupGui.writeEntry( "Column0", m_widget.listTokens->columnWidth( 0 ) );
    groupGui.writeEntry( "Column1", m_widget.listTokens->columnWidth( 1 ) );
    
    this->saveDialogSize( groupGui );
}

#include "tokenhelpdialog.moc"


