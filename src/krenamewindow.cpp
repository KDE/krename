/***************************************************************************
                       krenamewindow.cpp  -  description
                             -------------------
    begin                : Sat Mar 24 2007
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

#include "krenamewindow.h"

#include "krenamemodel.h"
#include "plugin.h"
#include "pluginloader.h"

#include "ui_krenamefiles.h"
#include "ui_krenamedestination.h"
//#include "ui_krenamesimple.h"
#include "ui_krenameplugins.h"
#include "ui_krenamefilename.h"

#include <kicon.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kseparator.h>

#include <QDialogButtonBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QStackedWidget>
#include <QTabBar>
#include <QVBoxLayout>

static const KRenameWindow::TGuiMode tAdvancedMode = {
    4,
    {
        I18N_NOOP( "&Files" ),
        I18N_NOOP( "&Destination" ),
        I18N_NOOP( "&Plugins" ),
        I18N_NOOP( "File&name" )
    },
    {
        0, 1, 2, 3
    }
};

KRenameWindow::KRenameWindow( QWidget* parent )
    : KMainWindow( parent ), 
      m_curPage( 0 ), m_guiMode( NULL ), 
      m_fileCount( 0 )
{
    QWidget*     center = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout( center );

    m_tabBar  = new QTabBar( center );
    m_stack   = new QStackedWidget( center );
    m_buttons = new QDialogButtonBox( center );
    
    layout->addWidget( m_tabBar );
    layout->addWidget( m_stack );
    layout->addWidget( new KSeparator( Qt::Horizontal, center ) );
    layout->addWidget( m_buttons );
    layout->setStretchFactor( m_stack, 2 );

    this->setCentralWidget( center );

    for( int i=0;i<tAdvancedMode.numPages;i++ )
        m_tabBar->addTab( i18n( tAdvancedMode.pageTitles[i] ) );

    m_pageFiles    = new Ui::KRenameFiles();
    m_pageDests    = new Ui::KRenameDestination();
    m_pagePlugins  = new Ui::KRenamePlugins();
    m_pageFilename = new Ui::KRenameFilename();

    // add files page
    QWidget* page = new QWidget( m_stack );
    m_pageFiles->setupUi( page );
    m_stack->addWidget( page );

    // add destination page
    page = new QWidget( m_stack );
    m_pageDests->setupUi( page );
    m_stack->addWidget( page );

    // add plugin page
    page = new QWidget( m_stack );
    m_pagePlugins->setupUi( page );
    m_stack->addWidget( page );

    // add filename page
    page = new QWidget( m_stack );
    m_pageFilename->setupUi( page );
    m_stack->addWidget( page );

    setupGui();
    setupPlugins();
    setupIcons();

    m_pageDests->urlrequester->setMode( KFile::Directory | KFile::ExistingOnly );
    
    // Make sure that now signal occurs before setupGui was called
    connect( m_tabBar, SIGNAL(currentChanged(int)), SLOT(showPage(int)));
    connect( m_buttonClose, SIGNAL(clicked(bool)), SLOT(close()));
    connect( m_buttons, SIGNAL(accepted()), SLOT(slotFinish()));


    // Show the first page in any mode
    showPage( 0 );
}

KRenameWindow::~KRenameWindow()
{
}

void KRenameWindow::setupGui()
{
    m_buttons->clear();

    m_guiMode    = &tAdvancedMode;

    m_buttonFinish = new KPushButton( KIcon( "dialog-ok" ),    i18n("&Finish...") );
    m_buttonClose  = new KPushButton( KIcon( "dialog-close" ), i18n("&Close") );
    
    m_buttons->addButton( m_buttonFinish, QDialogButtonBox::AcceptRole );
    m_buttons->addButton( m_buttonClose,  QDialogButtonBox::RejectRole );

    setupSlots();
    slotEnableControls();
}

void KRenameWindow::setupPlugins()
{
    PluginLoader* loader = PluginLoader::Instance();

    const QList<Plugin*> & list = loader->plugins();
    QList<Plugin*>::const_iterator it = list.begin();

    m_pluginsWidgetHash.reserve( list.count() );
    m_pluginsHash.reserve( list.count() );

    m_pagePlugins->searchPlugins->searchLine()->setTreeWidget( m_pagePlugins->listPlugins );
    
    while( it != list.end() )
    {
        // create plugin gui
        QWidget* widget = new QWidget( m_pagePlugins->stackPlugins );
        (*it)->createUI( widget );
        int idx = m_pagePlugins->stackPlugins->addWidget( widget );
        m_pagePlugins->stackPlugins->setCurrentIndex( idx );

        m_pluginsHash[(*it)->name()] = (*it);
        m_pluginsWidgetHash[(*it)->name()] = widget;

        // add to list of all plugins
        QTreeWidgetItem* item = new QTreeWidgetItem( m_pagePlugins->listPlugins );
        item->setText( 0, (*it)->name() );
        item->setIcon( 0, (*it)->icon() );

        slotPluginChanged( item );

        ++it;
    }

    m_pagePlugins->splitter->setStretchFactor( 0, 0 );
    m_pagePlugins->splitter->setStretchFactor( 1, 8 );
    m_pagePlugins->listPlugins->sortColumn();
}

void KRenameWindow::setupIcons()
{
    QPixmap upIcon = KIconLoader::global()->loadIcon( "arrow-up", KIconLoader::NoGroup, KIconLoader::SizeSmall );
    QPixmap downIcon = KIconLoader::global()->loadIcon( "arrow-down", KIconLoader::NoGroup, KIconLoader::SizeSmall );

    // Page 1 icons

    QPixmap openIcon = KIconLoader::global()->loadIcon( "document-open", KIconLoader::NoGroup, KIconLoader::SizeSmall );
    QPixmap removeIcon = KIconLoader::global()->loadIcon( "list-remove", KIconLoader::NoGroup, KIconLoader::SizeSmall );

    m_pageFiles->buttonAdd->setIcon( openIcon );
    m_pageFiles->buttonRemove->setIcon( removeIcon );
    m_pageFiles->buttonUp->setIcon( upIcon );
    m_pageFiles->buttonDown->setIcon( downIcon );

    // Page 4 icons

    QPixmap helpIcon = KIconLoader::global()->loadIcon( "help-hint", KIconLoader::NoGroup, KIconLoader::SizeSmall );
    QPixmap findIcon = KIconLoader::global()->loadIcon( "edit-find", KIconLoader::NoGroup, KIconLoader::SizeSmall );

    m_pageFilename->buttonHelp1->setIcon( helpIcon ); 
    m_pageFilename->buttonHelp2->setIcon( helpIcon ); 
    m_pageFilename->buttonHelp3->setIcon( helpIcon ); 
    m_pageFilename->buttonHelp4->setIcon( helpIcon ); 
    m_pageFilename->buttonFunctions->setIcon( helpIcon );

    m_pageFilename->buttonFind->setIcon( findIcon );
    m_pageFilename->buttonFindSimple->setIcon( findIcon );

    m_pageFilename->buttonUp->setIcon( upIcon );
    m_pageFilename->buttonDown->setIcon( downIcon );
    
}

void KRenameWindow::setupSlots()
{
    connect( m_pageFiles->buttonAdd,       SIGNAL(clicked(bool)), SIGNAL(addFiles()));
    connect( m_pageFiles->buttonRemove,    SIGNAL(clicked(bool)), SIGNAL(removeFiles()));
    connect( m_pageFiles->buttonRemoveAll, SIGNAL(clicked(bool)), SIGNAL(removeAllFiles()));
    connect( m_pageFiles->checkPreview,    SIGNAL(clicked(bool)), SLOT( slotPreviewChanged()));
    connect( m_pageFiles->checkName,       SIGNAL(clicked(bool)), SLOT( slotPreviewChanged()));
    connect( m_pageFiles->comboSort,       SIGNAL(currentIndexChanged(int)), SLOT( slotSortChanged(int)));
    connect( m_pageFiles->fileList,        SIGNAL(activated(const QModelIndex&)), SLOT( slotOpenFile(const QModelIndex&)));
    connect( m_pageFiles->buttonUp,        SIGNAL(clicked(bool)), SLOT( slotMoveUp() ) );
    connect( m_pageFiles->buttonDown,      SIGNAL(clicked(bool)), SLOT( slotMoveDown() ) );

    connect( m_pageDests->optionRename,    SIGNAL(clicked(bool)), SLOT(slotRenameModeChanged()));
    connect( m_pageDests->optionCopy,      SIGNAL(clicked(bool)), SLOT(slotRenameModeChanged()));
    connect( m_pageDests->optionMove,      SIGNAL(clicked(bool)), SLOT(slotRenameModeChanged()));
    connect( m_pageDests->optionLink,      SIGNAL(clicked(bool)), SLOT(slotRenameModeChanged()));
    connect( m_pageDests->checkOverwrite,  SIGNAL(clicked(bool)), SIGNAL(overwriteFilesChanged(bool)));

    connect( m_pagePlugins->listPlugins,   SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(slotPluginChanged(QTreeWidgetItem*)));
    connect( m_pagePlugins->checkEnablePlugin, SIGNAL(clicked(bool)), SLOT(slotPluginEnabled()));

    connect( m_pageFilename->checkExtension,     SIGNAL(clicked(bool))       , SLOT(slotEnableControls()));
    connect( m_pageFilename->buttonNumbering,    SIGNAL(clicked(bool))       , SIGNAL(showAdvancedNumberingDialog()));
    connect( m_pageFilename->buttonInsert,       SIGNAL(clicked(bool))       , SIGNAL(showInsertPartFilenameDialog()));
    connect( m_pageFilename->buttonFind,         SIGNAL(clicked(bool))       , SIGNAL(showFindReplaceDialog()));

    connect( m_pageFilename->filenameTemplate,   SIGNAL(delayedTextChanged()), SLOT(slotTemplateChanged()));
    connect( m_pageFilename->extensionTemplate,  SIGNAL(delayedTextChanged()), SLOT(slotTemplateChanged()));
    connect( m_pageFilename->checkExtension,     SIGNAL(clicked(bool))       , SLOT(slotTemplateChanged()));
    connect( m_pageFilename->buttonFunctions,    SIGNAL(clicked(bool))       , SLOT(slotTokenHelpRequested()));
    connect( m_pageFilename->comboExtension,     SIGNAL(currentIndexChanged(int)), SLOT(slotExtensionSplitModeChanged(int)));

    connect( m_pageFilename->buttonUp,           SIGNAL(clicked(bool)), SLOT( slotMoveUpPreview() ) );
    connect( m_pageFilename->buttonDown,         SIGNAL(clicked(bool)), SLOT( slotMoveDownPreview() ) );

    connect( m_pageFilename->listPreview,        SIGNAL(addFiles()),    SIGNAL(addFiles()));

    connect( m_pageFilename->comboFilenameCustom,  SIGNAL(delayedTextChanged()), SLOT(slotSimpleTemplateChanged()));
    connect( m_pageFilename->comboSuffixCustom,    SIGNAL(delayedTextChanged()), SLOT(slotSimpleTemplateChanged()));
    connect( m_pageFilename->comboPrefixCustom,    SIGNAL(delayedTextChanged()), SLOT(slotSimpleTemplateChanged()));
    connect( m_pageFilename->comboExtensionCustom, SIGNAL(delayedTextChanged()), SLOT(slotSimpleTemplateChanged()));

    connect( m_pageFilename->comboPrefix,          SIGNAL(currentIndexChanged(int)), SLOT(slotSimpleTemplateChanged()));
    connect( m_pageFilename->comboSuffix,          SIGNAL(currentIndexChanged(int)), SLOT(slotSimpleTemplateChanged()));
    connect( m_pageFilename->comboFilenameSimple,  SIGNAL(currentIndexChanged(int)), SLOT(slotSimpleTemplateChanged()));
    connect( m_pageFilename->comboExtensionSimple, SIGNAL(currentIndexChanged(int)), SLOT(slotSimpleTemplateChanged()));

    connect( m_pageFilename->comboExtensionSimple, SIGNAL(currentIndexChanged(int)), SLOT(slotEnableControls()));
    connect( m_pageFilename->comboFilenameSimple,  SIGNAL(currentIndexChanged(int)), SLOT(slotEnableControls()));

    connect( m_pageFilename->buttonHelp1,          SIGNAL(clicked(bool)),        SLOT(slotTokenHelpRequestedWizard1()));
    connect( m_pageFilename->buttonHelp2,          SIGNAL(clicked(bool)),        SLOT(slotTokenHelpRequestedWizard2()));
    connect( m_pageFilename->buttonHelp3,          SIGNAL(clicked(bool)),        SLOT(slotTokenHelpRequestedWizard3()));
    connect( m_pageFilename->buttonHelp4,          SIGNAL(clicked(bool)),        SLOT(slotTokenHelpRequestedWizard4()));
    connect( m_pageFilename->buttonFindSimple,     SIGNAL(clicked(bool))       , SIGNAL(showFindReplaceDialog()));

    connect( m_pageFilename->spinDigits,           SIGNAL(valueChanged(int)),    SLOT(slotSimpleTemplateChanged()));
    connect( m_pageFilename->spinIndex,            SIGNAL(valueChanged(int)),    SLOT(slotSimpleStartIndexChanged()));
}

void KRenameWindow::showPage( int index )
{
    if( index >= 0 && index < m_guiMode->numPages )
    {
        m_curPage = index;
        m_stack->setCurrentIndex( m_guiMode->mapIndex[index] );
        
        slotEnableControls();
    }
}

void KRenameWindow::slotEnableControls()
{
    if( m_buttonFinish )
        m_buttonFinish->setEnabled( m_curPage == m_guiMode->numPages - 1 );

    // enable gui controls
    m_pageFiles->buttonRemove->setEnabled( m_fileCount );
    m_pageFiles->buttonRemoveAll->setEnabled( m_fileCount );
    m_pageFiles->checkName->setEnabled( m_pageFiles->checkPreview->isChecked() );
   
    m_pageFiles->buttonUp->setEnabled( m_fileCount );
    m_pageFiles->buttonMove->setEnabled( m_fileCount );
    m_pageFiles->buttonDown->setEnabled( m_fileCount );

    m_pageDests->urlrequester->setEnabled( !m_pageDests->optionRename->isChecked() );

    m_pageFilename->extensionTemplate->setEnabled( !m_pageFilename->checkExtension->isChecked() );
    m_pageFilename->buttonFind->setEnabled( m_fileCount != 0 );
    m_pageFilename->buttonNumbering->setEnabled( m_fileCount != 0 );
    m_pageFilename->buttonInsert->setEnabled( m_fileCount != 0 );

    m_pageFilename->buttonFindSimple->setEnabled( m_fileCount != 0 );
    m_pageFilename->buttonHelp3->setEnabled( m_pageFilename->comboFilenameSimple->currentIndex() == 
                                             m_pageFilename->comboFilenameSimple->count() - 1 );
    m_pageFilename->comboFilenameCustom->setEnabled( m_pageFilename->comboFilenameSimple->currentIndex() == 
                                                     m_pageFilename->comboFilenameSimple->count() - 1 );

    m_pageFilename->buttonHelp4->setEnabled( m_pageFilename->comboExtensionSimple->currentIndex() == 
                                                        m_pageFilename->comboExtensionSimple->count() - 1 );
    m_pageFilename->comboExtensionCustom->setEnabled( m_pageFilename->comboExtensionSimple->currentIndex() == 
                                                      m_pageFilename->comboExtensionSimple->count() - 1 );

    m_pageFilename->buttonUp->setEnabled( m_fileCount );
    m_pageFilename->buttonDown->setEnabled( m_fileCount );
}

void KRenameWindow::setCount( unsigned int count )
{
    m_fileCount = count;
    m_pageFiles->labelCount->setText( i18n("<b>Files:<b> %1", m_fileCount ) );

    this->slotEnableControls();
}

void KRenameWindow::setFilenameTemplate( const QString & templ, bool insert )
{
    if( insert )
        m_pageFilename->filenameTemplate->lineEdit()->insert( templ );
    else
        m_pageFilename->filenameTemplate->lineEdit()->setText( templ );
}

void KRenameWindow::resetFileList() 
{
    m_pageFiles->fileList->reset();
}

void KRenameWindow::setModel( KRenameModel* model )
{
    m_pageFiles->fileList->setModel( model );

    m_pageFilename->listPreview->setKRenameModel( model );

    connect( model, SIGNAL( maxDotsChanged(int) ), SLOT( slotMaxDotsChanged(int)) );
}

void KRenameWindow::setPreviewModel( KRenamePreviewModel* model )
{
    m_pageFilename->listPreview->setModel( model );
}

const KUrl KRenameWindow::destinationUrl() const
{
    return m_pageDests->urlrequester->url();
}

QList<int> KRenameWindow::selectedFileItems() const
{
    QList<int> selected;

    QItemSelectionModel* selection = m_pageFiles->fileList->selectionModel();
    QModelIndexList      indeces = selection->selectedIndexes();
    QModelIndexList::const_iterator it = indeces.begin();
    
    while( it != indeces.end() )
    {
        selected.append( (*it).row() );
        ++it;
    }

    return selected;
}

QList<int> KRenameWindow::selectedFileItemsPreview() const
{
    QList<int> selected;

    QItemSelectionModel* selection = m_pageFilename->listPreview->selectionModel();

    QModelIndexList      indeces = selection->selectedIndexes();
    QModelIndexList::const_iterator it = indeces.begin();
    
    while( it != indeces.end() )
    {
        selected.append( (*it).row() );
        ++it;
    }

    return selected;
}

bool KRenameWindow::isPreviewEnabled() const
{
    return m_pageFiles->checkPreview->isChecked();
}

void KRenameWindow::setPreviewEnabled( bool bPreview )
{
    m_pageFiles->checkPreview->setChecked( bPreview );

    slotPreviewChanged();
}

bool KRenameWindow::isPreviewNamesEnabled() const
{
    return m_pageFiles->checkName->isChecked();
}

void KRenameWindow::setPreviewNamesEnabled( bool bPreview )
{
    m_pageFiles->checkName->setChecked( bPreview );

    slotPreviewChanged();
}

int KRenameWindow::numberStartIndex() const
{
    return m_pageFilename->spinIndex->value();
}

void KRenameWindow::setNumberStartIndex( int index )
{
    m_pageFilename->spinIndex->setValue( index );
}

int KRenameWindow::sortMode() const
{
    return m_pageFiles->comboSort->currentIndex();
}

void KRenameWindow::setSortMode( int sortMode )
{
    m_pageFiles->comboSort->setCurrentIndex( sortMode );
}

int KRenameWindow::previewColumnWidth( int index )
{
    return m_pageFilename->listPreview->columnWidth( index );
}

void KRenameWindow::setPreviewColumnWidth( int index, int width )
{
    m_pageFilename->listPreview->setColumnWidth( index, width );
}

bool KRenameWindow::isAdvancedMode() const
{
    return (m_pageFilename->tabWidget->currentIndex() == 0);
}

void KRenameWindow::setAdvancedMode( bool bAdvanced )
{
    m_pageFilename->tabWidget->setCurrentIndex( bAdvanced ? 0 : 1 );
}

void KRenameWindow::setPrefixSuffixSimple( QComboBox* combo, QComboBox* comboCustom, const QString & templ ) 
{
    if( templ.isEmpty() ) 
    {
        comboCustom->lineEdit()->setText( templ );
        combo->setCurrentIndex( 0 );
    }
    else
    {
        QString number = "#";
        int c = m_pageFilename->spinDigits->value()-1;
        
        while( c-- > 0 )
            number += "#";
        
        number += QString("{%1}").arg( m_pageFilename->spinIndex->value() );

        if( templ.startsWith( number ) ) 
        {
            QString value = templ.right( templ.length() - number.length() );
            combo->setCurrentIndex( 1 );
            comboCustom->lineEdit()->setText( value );
        }
        else if( templ.startsWith( "[date]" ) ) 
        {
            QString value = templ.right( templ.length() - 6 );
            combo->setCurrentIndex( 2 );
            comboCustom->lineEdit()->setText( value );
        }
        else
        {
            combo->setCurrentIndex( 0 );
            comboCustom->lineEdit()->setText( templ );
        }
    }
}

QString KRenameWindow::getPrefixSuffixSimple( QComboBox* combo, QComboBox* comboCustom ) 
{
    QString str;
    QString number = "#";
    int c = m_pageFilename->spinDigits->value()-1;

    while( c-- > 0 )
        number += "#";

    number += QString("{%1}").arg( m_pageFilename->spinIndex->value() );
    
    if( combo->currentIndex() == 1 ) 
        str = number;
    else if( combo->currentIndex() == 2 ) 
        str = "[date]"; // TODO date

    str += comboCustom->currentText();

    return str;
}

QString KRenameWindow::getFilenameSimple( QComboBox* combo, QComboBox* comboCustom ) 
{
    QString str;

    // TODO: Replace strings with constants
    switch( combo->currentIndex() ) 
    {
        default:
        case 0:
            str = "$"; break;
        case 1:
            str = "%"; break;
        case 2:
            str = "&"; break;
        case 3:
            str = "*"; break;
        case 4:
            str = comboCustom->currentText();
    }

    return str;
}

void KRenameWindow::slotBack()
{
    this->showPage( m_curPage-1 );
}

void KRenameWindow::slotNext()
{
    this->showPage( m_curPage+1 );
}

void KRenameWindow::slotFinish()
{
    emit accepted();
}

void KRenameWindow::slotRenameModeChanged()
{
    ERenameMode mode = eRenameMode_Rename;

    if( m_pageDests->optionRename->isChecked() )
        mode = eRenameMode_Rename;
    else if( m_pageDests->optionCopy->isChecked() )
        mode = eRenameMode_Copy;
    else if( m_pageDests->optionMove->isChecked() ) 
        mode = eRenameMode_Move;
    else if( m_pageDests->optionLink->isChecked() )
        mode = eRenameMode_Link;

    emit renameModeChanged( mode );

    this->slotEnableControls();
}

void KRenameWindow::slotSimpleStartIndexChanged()
{
    emit startIndexChanged( m_pageFilename->spinIndex->value() );

    updatePreview();
}

void KRenameWindow::slotTemplateChanged()
{
    QString filename;
    QString extension;

    filename  = m_pageFilename->filenameTemplate->currentText();
    extension = m_pageFilename->checkExtension->isChecked() ? "$" : 
        m_pageFilename->extensionTemplate->currentText();

    // set the new templates also for simple mode
    blockSignalsRecursive( this, true );
    this->setSimpleTemplate( filename, extension );
    blockSignalsRecursive( this, false );

    this->templatesChanged( filename, extension );
}

void KRenameWindow::slotSimpleTemplateChanged()
{
    QString filename  = getFilenameSimple( m_pageFilename->comboFilenameSimple, m_pageFilename->comboFilenameCustom );
    QString extension = getFilenameSimple( m_pageFilename->comboExtensionSimple, m_pageFilename->comboExtensionCustom );
    QString prefix    = getPrefixSuffixSimple( m_pageFilename->comboPrefix, m_pageFilename->comboPrefixCustom );
    QString suffix    = getPrefixSuffixSimple( m_pageFilename->comboSuffix, m_pageFilename->comboSuffixCustom );

    filename = prefix + filename + suffix;

    // set the new templates, but make sure signals 
    // are blockes so that slotTemplateChanged emits updatePreview()
    // which is calculation intensive only once!
    blockSignalsRecursive( this, true );
    m_pageFilename->filenameTemplate->lineEdit()->setText( filename );
    m_pageFilename->extensionTemplate->lineEdit()->setText( extension );
    m_pageFilename->checkExtension->setChecked( false );
    blockSignalsRecursive( this, false );

    this->templatesChanged( filename, extension );
}

void KRenameWindow::templatesChanged( const QString & filename, const QString & extension ) 
{
    emit filenameTemplateChanged( filename );
    emit extensionTemplateChanged( extension );

    emit updatePreview();

    m_pageFilename->buttonNumbering->setEnabled( filename.contains('#') || extension.contains('#') );
    this->slotEnableControls();
}

void KRenameWindow::setSimpleTemplate( const QString & filename, const QString & extension ) 
{
    // First set the simple extension from a template string
    if( extension == "$" ) 
        m_pageFilename->comboExtensionSimple->setCurrentIndex( 0 );
    else if( extension == "%" )
        m_pageFilename->comboExtensionSimple->setCurrentIndex( 1 );
    else if( extension == "&" )
        m_pageFilename->comboExtensionSimple->setCurrentIndex( 2 );
    else if( extension == "*" )
        m_pageFilename->comboExtensionSimple->setCurrentIndex( 3 );
    else 
    {
        m_pageFilename->comboExtensionSimple->setCurrentIndex( 4 );
        m_pageFilename->comboExtensionCustom->lineEdit()->setText( extension );
    }

    // Now split the filename in prefix and suffix and set it as template
    // TODO: Make sure we do not find something like [*5-] or \$
    int index = 4;
    int pos   = filename.indexOf( "$" );
    if( pos == -1 ) 
    {
        pos = filename.indexOf( "%" );
        if( pos == -1 )
        {
            pos = filename.indexOf( "&" );
            if( pos == -1 ) 
            {
                pos = filename.indexOf( "*" );
                if( pos != -1 )
                    index = 3;
            }
            else 
                index = 2;
        }
        else
            index = 1;
    }
    else
        index = 0;

    m_pageFilename->comboFilenameSimple->setCurrentIndex( index );
    if( pos == -1 ) 
    {
        // No token found, so we have no prefix or suffix but 
        // a custom name.
        m_pageFilename->comboPrefixCustom->lineEdit()->setText( QString::null );
        m_pageFilename->comboPrefix->setCurrentIndex( 0 );

        m_pageFilename->comboSuffixCustom->lineEdit()->setText( QString::null );
        m_pageFilename->comboSuffix->setCurrentIndex( 0 );

        m_pageFilename->comboFilenameCustom->lineEdit()->setText( filename );
    }
    else 
    {
        QString prefix = ( pos > 0 ? filename.left( pos ) : QString::null);
        QString suffix = ( pos < filename.length() ? filename.right( filename.length() - pos - 1 ) : QString::null);

        setPrefixSuffixSimple( m_pageFilename->comboPrefix, m_pageFilename->comboPrefixCustom, prefix );
        setPrefixSuffixSimple( m_pageFilename->comboSuffix, m_pageFilename->comboSuffixCustom, suffix );
    }
}

void KRenameWindow::slotTokenHelpRequested()
{
    emit showTokenHelpDialog( m_pageFilename->filenameTemplate->lineEdit() );
}

void KRenameWindow::slotTokenHelpRequestedWizard1()
{
    emit showTokenHelpDialog( m_pageFilename->comboPrefixCustom->lineEdit() );
}

void KRenameWindow::slotTokenHelpRequestedWizard2()
{
    emit showTokenHelpDialog( m_pageFilename->comboSuffixCustom->lineEdit() );
}

void KRenameWindow::slotTokenHelpRequestedWizard3()
{
    emit showTokenHelpDialog( m_pageFilename->comboFilenameCustom->lineEdit() );
}

void KRenameWindow::slotTokenHelpRequestedWizard4()
{
    emit showTokenHelpDialog( m_pageFilename->comboExtensionCustom->lineEdit() );
}

void KRenameWindow::slotExtensionSplitModeChanged( int index )
{
    ESplitMode splitMode;
    switch( index ) 
    {
        case 0:
            splitMode = eSplitMode_FirstDot;
            break;
        case 1:
            splitMode = eSplitMode_LastDot;
            break;
        default:
            splitMode = eSplitMode_CustomDot;
            break;
    }

    emit extensionSplitModeChanged( splitMode, index-1 );
}

void KRenameWindow::slotPreviewChanged()
{
    KRenameModel* model = static_cast<KRenameModel*>(m_pageFiles->fileList->model());

    if( m_pageFiles->checkPreview->isChecked() && !m_pageFiles->checkName->isChecked() )
        m_pageFiles->fileList->setViewMode( QListView::IconMode );
    else
        m_pageFiles->fileList->setViewMode( QListView::ListMode );

    model->setEnablePreview( m_pageFiles->checkPreview->isChecked(), m_pageFiles->checkName->isChecked() );
    emit filePreviewChanged( m_pageFiles->checkPreview->isChecked(), m_pageFiles->checkName->isChecked() );

    m_pageFiles->fileList->setAcceptDrops( true );
    m_pageFiles->fileList->repaint();
    this->slotEnableControls();
}

void KRenameWindow::slotSortChanged( int index )
{
    ESortMode eMode;

    switch( index ) 
    {
        default:
        case 0:
            eMode = eSortMode_Unsorted;  break;
        case 1:
            eMode = eSortMode_Ascending; break;
        case 2: 
            eMode = eSortMode_Descending; break;
        case 3:
            eMode = eSortMode_Numeric; break;
        case 4:
            eMode = eSortMode_Random; break;
    }

    KRenameModel* model = static_cast<KRenameModel*>(m_pageFiles->fileList->model());
    model->sortFiles( eMode );
}

void KRenameWindow::slotMaxDotsChanged( int dots )
{
    int i;

    for( i=2;i<m_pageFilename->comboExtension->count();i++ )
        m_pageFilename->comboExtension->removeItem( i );

    for( i=1;i<=dots;i++ )
        m_pageFilename->comboExtension->addItem( QString::number( i ) );

}

void KRenameWindow::slotOpenFile(const QModelIndex& index)
{
    KRenameModel* model = static_cast<KRenameModel*>(m_pageFiles->fileList->model());
    model->run( index, this );
}

void KRenameWindow::slotMoveUp()
{
    QList<int> sel = this->selectedFileItems();

    KRenameModel* model = static_cast<KRenameModel*>(m_pageFiles->fileList->model());    
    model->moveFilesUp( sel );

    QItemSelectionModel* selection = m_pageFiles->fileList->selectionModel();
    QList<int>::const_iterator it  = sel.begin();
    while( it != sel.end() )
    {
        if( *it - 1 > 0 ) 
            selection->select( model->createIndex( *it - 1 ), QItemSelectionModel::Select );

        ++it;
    }

    // make sure that the first item is visible
    // TODO: Maybe it is better to calculate the minimum index here
    if( sel.size() )
        m_pageFiles->fileList->scrollTo( model->createIndex( sel.front() - 1 ), QAbstractItemView::EnsureVisible );
}

void KRenameWindow::slotMoveDown()
{
    QList<int> sel = this->selectedFileItems();

    KRenameModel* model = static_cast<KRenameModel*>(m_pageFiles->fileList->model());    
    model->moveFilesDown( sel );


    QItemSelectionModel* selection = m_pageFiles->fileList->selectionModel();
    QList<int>::const_iterator it  = sel.begin();
    while( it != sel.end() )
    {
        if( *it + 1 < model->rowCount() ) 
            selection->select( model->createIndex( *it + 1 ), QItemSelectionModel::Select );

        ++it;
    }

    // make sure that the last item is visible
    // TODO: Maybe it is better to calculate the maximum index here
    if( sel.size() )
        m_pageFiles->fileList->scrollTo( model->createIndex( sel.back() + 1 ), QAbstractItemView::EnsureVisible );
}

void KRenameWindow::slotMoveUpPreview()
{
    QList<int>         sel  = this->selectedFileItemsPreview();
    QAbstractItemView* view = m_pageFilename->listPreview;

    moveUp( sel, view );
}

void KRenameWindow::slotMoveDownPreview()
{
    QList<int>         sel  = this->selectedFileItemsPreview();
    QAbstractItemView* view = m_pageFilename->listPreview;

    moveDown( sel, view );
}

void KRenameWindow::moveUp( const QList<int> & selected, QAbstractItemView* view ) 
{
    KRenameModel* model = static_cast<KRenameModel*>(m_pageFiles->fileList->model());    
    model->moveFilesUp( selected );

    updatePreview();

    QItemSelectionModel* selection = view->selectionModel();
    QList<int>::const_iterator it  = selected.begin();
    while( it != selected.end() )
    {
        if( *it - 1 > 0 ) 
            selection->select( model->createIndex( *it - 1 ), QItemSelectionModel::Select );

        ++it;
    }

    // make sure that the first item is visible
    // TODO: Maybe it is better to calculate the minimum index here
    if( selected.size() )
        view->scrollTo( model->createIndex( selected.front() - 1 ), QAbstractItemView::EnsureVisible );
}

void KRenameWindow::moveDown( const QList<int> & selected, QAbstractItemView* view )
{
    KRenameModel* model = static_cast<KRenameModel*>(m_pageFiles->fileList->model());    
    model->moveFilesDown( selected );

    updatePreview();

    QItemSelectionModel* selection = view->selectionModel();
    QList<int>::const_iterator it  = selected.begin();
    while( it != selected.end() )
    {
        if( *it + 1 < model->rowCount() ) 
            selection->select( model->createIndex( *it + 1 ), QItemSelectionModel::Select );

        ++it;
    }


    // make sure that the last item is visible
    // TODO: Maybe it is better to calculate the maximum index here
    if( selected.size() )
        view->scrollTo( model->createIndex( selected.back() + 1 ), QAbstractItemView::EnsureVisible );
}

void KRenameWindow::slotPluginChanged(QTreeWidgetItem* selected)
{
    QWidget* w = m_pluginsWidgetHash[selected->text(0)];
    Plugin*  p = m_pluginsHash[selected->text(0)];

    if( p->alwaysEnabled() )
        m_pagePlugins->checkEnablePlugin->setChecked( true );
    else
        m_pagePlugins->checkEnablePlugin->setChecked( p->isEnabled() );

    m_pagePlugins->checkEnablePlugin->setEnabled( !p->alwaysEnabled() );
    m_pagePlugins->stackPlugins->setCurrentWidget( w );

    slotPluginEnabled();
}

void KRenameWindow::slotPluginEnabled()
{
    QTreeWidgetItem* selected = m_pagePlugins->listPlugins->currentItem();
    if( selected ) 
    {
        QWidget* w = m_pluginsWidgetHash[selected->text(0)];
        Plugin*  p = m_pluginsHash[selected->text(0)];
        
        p->setEnabled( m_pagePlugins->checkEnablePlugin->isChecked() );
        w->setEnabled( p->alwaysEnabled() || m_pagePlugins->checkEnablePlugin->isChecked() );
    }
}

void KRenameWindow::blockSignalsRecursive( QObject* obj, bool b ) 
{
    if( obj )
    {
        obj->blockSignals(b);
        
        QList<QObject*> list = obj->children();
        QList<QObject*>::iterator it = list.begin();
        QObject* o = NULL;
        
        while( it != list.end() ) 
        {
            o = *it;
            if( o && o != obj) 
                blockSignalsRecursive(o, b);
            
            ++it;
        }
    }
}

#include "krenamewindow.moc"
