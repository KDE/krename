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

#include "batchrenamer.h"
#include "krenamemodel.h"

#include <kapplication.h>
#include <kpushbutton.h>

#define COLUMN_PREVIEW 2

TokenHelpDialog::TokenHelpDialog( KRenameModel* model, BatchRenamer* renamer,
                                  QLineEdit* edit, QWidget* parent )
    : KDialog( parent ), m_edit( edit ), m_model(model), m_renamer(renamer)
{
    m_widget.setupUi( mainWidget() );

    this->setButtons( KDialog::Close | KDialog::User1 ); 
    this->setButtonText( KDialog::User1, i18n("&Insert") ); 

    KPushButton* insert = this->button( KDialog::User1 );
    KPushButton* close = this->button( KDialog::Close );
    
    m_widget.searchCategory->searchLine()->setTreeWidget( m_widget.listCategories );
    m_widget.searchToken   ->searchLine()->setTreeWidget( m_widget.listTokens );
    m_widget.comboPreview->setModel( model );

    connect(insert, SIGNAL(clicked(bool)), SLOT(slotInsert()));
    connect(this, SIGNAL(rejected()), SLOT(reject()));
    connect(close, SIGNAL(clicked(bool)), SLOT(saveConfig()));

    connect(m_widget.listCategories, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(slotCategoryChanged(QTreeWidgetItem*)));
    connect(m_widget.listTokens,     SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(slotInsert()));
    connect(m_widget.checkPreview,   SIGNAL(clicked(bool)), this, SLOT(slotPreviewClicked(bool)));
    connect(m_widget.comboPreview,   SIGNAL(activated(int)), this, SLOT(slotUpdatePreview()));

    slotEnableControls();
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

    slotUpdatePreview();
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

    bool preview = groupGui.readEntry( "Preview",  m_widget.checkPreview->isChecked() );
    m_widget.checkPreview->setChecked( preview );

    int width = groupGui.readEntry( "Column0", QVariant(m_widget.listTokens->columnWidth( 0 )) ).toInt();
    if( width > 0 )
		m_widget.listTokens->setColumnWidth( 0, width );

    width = groupGui.readEntry( "Column1", QVariant(m_widget.listTokens->columnWidth( 1 )) ).toInt();
    if( width > 0 )
	    m_widget.listTokens->setColumnWidth( 1, width );

    width = groupGui.readEntry( "Column2", QVariant(m_widget.listTokens->columnWidth( 2 )) ).toInt();
    if( width > 0 )
	    m_widget.listTokens->setColumnWidth( 2, width );

    this->restoreDialogSize( groupGui );

    QList<int> sizes = groupGui.readEntry( "Splitter", QList<int>() );
    if( sizes.size() == 2 ) {
        m_widget.splitter->setSizes( sizes );
    }
}

void TokenHelpDialog::saveConfig() 
{
    KSharedConfigPtr config = KGlobal::config();

    KConfigGroup groupGui = config->group( QString("TokenHelpDialog") );

    groupGui.writeEntry( "Column0",  m_widget.listTokens->columnWidth( 0 ) );
    groupGui.writeEntry( "Column1",  m_widget.listTokens->columnWidth( 1 ) );
    groupGui.writeEntry( "Column2",  m_widget.listTokens->columnWidth( 2 ) );
    groupGui.writeEntry( "Splitter", m_widget.splitter->sizes() );
    groupGui.writeEntry( "Preview",  m_widget.checkPreview->isChecked() );
    this->saveDialogSize( groupGui );
}

void TokenHelpDialog::slotEnableControls()
{
    m_widget.comboPreview->setEnabled( m_widget.checkPreview->isChecked() );
    m_widget.labelPreview->setEnabled( m_widget.checkPreview->isChecked() );
}

void TokenHelpDialog::slotPreviewClicked(bool bPreview)
{
    slotEnableControls();

    if( bPreview ) 
    {
        m_widget.listTokens->setColumnHidden( COLUMN_PREVIEW, false );
        slotUpdatePreview();

        // make sure preview column is visible
		m_widget.listTokens->resizeColumnToContents( 0 );
		m_widget.listTokens->resizeColumnToContents( 1 );
    }
    else
    {
        m_widget.listTokens->setColumnHidden( COLUMN_PREVIEW, true );
    }
}

void TokenHelpDialog::slotUpdatePreview()
{
    if( !m_widget.checkPreview->isChecked() )
        return;

    int index = m_widget.comboPreview->currentIndex();
    if( index >= 0 && m_widget.listCategories->currentItem() != NULL ) 
    {
        QString       name   = m_widget.listCategories->currentItem()->text(0);

        const KRenameFile & file = m_model->file( index );
        KApplication::setOverrideCursor( Qt::WaitCursor );

        QString token;
        for( int i=0;i<m_widget.listTokens->topLevelItemCount();i++ )
        {
            QTreeWidgetItem* item = m_widget.listTokens->topLevelItem( i );
            if( item ) 
            { 
                token = m_renamer->processString( item->text( 0 ), file.srcFilename(), index );
                item->setText( COLUMN_PREVIEW, token );
            }
        }
        KApplication::restoreOverrideCursor();
    }
}


#include "tokenhelpdialog.moc"


