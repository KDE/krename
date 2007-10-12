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

#include "ui_krenamefiles.h"
#include "ui_krenamedestination.h"
#include "ui_krenamesimple.h"
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

// Wizard mode
static const KRenameWindow::TGuiMode tWizardMode = {
    3,
    {
        I18N_NOOP( "Add some files" ),
        I18N_NOOP( "Set the destination" ),
        I18N_NOOP( "Filename" ),
        NULL
    },
    {
        0, 1, 3, -1
    }
};

// Advanced mode
static const KRenameWindow::TGuiMode tAdvancedMode = {
    4,
    {
        I18N_NOOP( "&Files" ),
        I18N_NOOP( "&Destination" ),
        I18N_NOOP( "&Plugins" ),
        I18N_NOOP( "File&name" )
    },
    {
        0, 1, 2, 4
    }
};

KRenameWindow::KRenameWindow( EGuiMode guiMode, QWidget* parent )
    : KMainWindow( parent ), 
      m_eGuiMode( guiMode ), m_curPage( 0 ), m_guiMode( NULL ), 
      m_fileCount( 0 ), m_buttonBack( NULL ), m_buttonNext( NULL )
{
    QWidget*     center = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout( center );

    m_stackTop = new QStackedWidget( center );
    m_stack    = new QStackedWidget( center );
    m_buttons  = new QDialogButtonBox( center );
    
    layout->addWidget( m_stackTop );
    layout->addWidget( m_stack );
    layout->addWidget( new KSeparator( Qt::Horizontal, center ) );
    layout->addWidget( m_buttons );
    layout->setStretchFactor( m_stack, 2 );

    this->setCentralWidget( center );

    QWidget*     title       = new QWidget();
    QHBoxLayout* titleLayout = new QHBoxLayout( title );

    m_lblTitle = new QLabel( title );
    m_lblStep  = new QLabel( title );

    titleLayout->addWidget( m_lblTitle );
    titleLayout->addStretch();
    titleLayout->addWidget( m_lblStep );

    m_stackTop->addWidget( title );

    m_tabBar = new QTabBar( m_stackTop );
    m_stackTop->addWidget( m_tabBar );

    for( int i=0;i<tAdvancedMode.numPages;i++ )
        m_tabBar->addTab( i18n( tAdvancedMode.pageTitles[i] ) );

    m_pageFiles    = new Ui::KRenameFiles();
    m_pageDests    = new Ui::KRenameDestination();
    m_pageSimple   = new Ui::KRenameSimple();
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

    // add simple filename page for wizard mode
    page = new QWidget( m_stack );
    m_pageSimple->setupUi( page );
    m_stack->addWidget( page );

    // add filename page
    page = new QWidget( m_stack );
    m_pageFilename->setupUi( page );
    m_stack->addWidget( page );

    setupGui();

    
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

    m_buttonBack  = NULL;
    m_buttonNext  = NULL;

    if( m_eGuiMode == eGuiMode_Wizard ) 
    {
        m_guiMode    = &tWizardMode;

        m_buttonBack = new KPushButton( KIcon( "back" ), i18n("&Back") );
        m_buttonNext = new KPushButton( KIcon( "next" ), i18n("&Next") );

        m_buttons->addButton( m_buttonBack, QDialogButtonBox::ActionRole );
        m_buttons->addButton( m_buttonNext, QDialogButtonBox::ActionRole );

        connect( m_buttonBack, SIGNAL(clicked(bool)),SLOT(slotBack()));
        connect( m_buttonNext, SIGNAL(clicked(bool)),SLOT(slotNext()));
    
        m_stackTop->setCurrentIndex( 0 ); // m_lblTitle
    }
    else
    {
        m_guiMode    = &tAdvancedMode;
        m_stackTop->setCurrentIndex( 1 ); // m_tabBar
    }

    m_buttonFinish = new KPushButton( KIcon( "finish" ), i18n("&Finish...") );
    m_buttonClose  = new KPushButton( KIcon( "cancel" ), i18n("&Close") );
    
    m_buttons->addButton( m_buttonFinish, QDialogButtonBox::AcceptRole );
    m_buttons->addButton( m_buttonClose, QDialogButtonBox::RejectRole );

    m_pageSimple->listPreview->setHeader( new QHeaderView( Qt::Horizontal ) );

    setupSlots();
    slotEnableControls();
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

    connect( m_pageSimple->comboFilenameCustom,  SIGNAL(delayedTextChanged()), SLOT(slotSimpleTemplateChanged()));
    connect( m_pageSimple->comboSuffixCustom,    SIGNAL(delayedTextChanged()), SLOT(slotSimpleTemplateChanged()));
    connect( m_pageSimple->comboPrefixCustom,    SIGNAL(delayedTextChanged()), SLOT(slotSimpleTemplateChanged()));
    connect( m_pageSimple->comboExtensionCustom, SIGNAL(delayedTextChanged()), SLOT(slotSimpleTemplateChanged()));

    connect( m_pageSimple->comboPrefix,          SIGNAL(currentIndexChanged(int)), SLOT(slotSimpleTemplateChanged()));
    connect( m_pageSimple->comboSuffix,          SIGNAL(currentIndexChanged(int)), SLOT(slotSimpleTemplateChanged()));
    connect( m_pageSimple->comboFilename,        SIGNAL(currentIndexChanged(int)), SLOT(slotSimpleTemplateChanged()));
    connect( m_pageSimple->comboExtension,       SIGNAL(currentIndexChanged(int)), SLOT(slotSimpleTemplateChanged()));

    connect( m_pageSimple->comboExtension,       SIGNAL(currentIndexChanged(int)), SLOT(slotEnableControls()));
    connect( m_pageSimple->comboFilename,        SIGNAL(currentIndexChanged(int)), SLOT(slotEnableControls()));

    connect( m_pageSimple->buttonHelp1,          SIGNAL(clicked(bool)),        SLOT(slotTokenHelpRequestedWizard1()));
    connect( m_pageSimple->buttonHelp2,          SIGNAL(clicked(bool)),        SLOT(slotTokenHelpRequestedWizard2()));
    connect( m_pageSimple->buttonHelp3,          SIGNAL(clicked(bool)),        SLOT(slotTokenHelpRequestedWizard3()));
    connect( m_pageSimple->buttonHelp4,          SIGNAL(clicked(bool)),        SLOT(slotTokenHelpRequestedWizard4()));

    connect( m_pageSimple->spinDigits,           SIGNAL(valueChanged(int)),    SLOT(slotSimpleTemplateChanged()));
    connect( m_pageSimple->spinIndex,            SIGNAL(valueChanged(int)),    SLOT(slotSimpleTemplateChanged()));

    connect( m_pageSimple->buttonUp,             SIGNAL(clicked(bool)),        SLOT( slotMoveUpPreview() ) );
    connect( m_pageSimple->buttonDown,           SIGNAL(clicked(bool)),        SLOT( slotMoveDownPreview() ) );

    connect( m_pageSimple->listPreview,          SIGNAL(addFiles()),    SIGNAL(addFiles()));
}

void KRenameWindow::showPage( int index )
{
    if( index >= 0 && index < m_guiMode->numPages )
    {
        m_curPage = index;
        m_stack->setCurrentIndex( m_guiMode->mapIndex[index] );
        
        if( m_eGuiMode == eGuiMode_Wizard )
        {
            m_lblTitle->setText( i18n( m_guiMode->pageTitles[index] ) );
            m_lblStep->setText( i18n("(Step %1/%2)", index+1, m_guiMode->numPages ) );
        }

        slotEnableControls();
    }
}

void KRenameWindow::slotEnableControls()
{
    if( m_buttonNext )
        m_buttonNext->setEnabled( m_curPage < m_guiMode->numPages - 1 );

    if( m_buttonBack )
        m_buttonBack->setEnabled( m_curPage > 0 );

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

    m_pageSimple->buttonFind->setEnabled( m_fileCount != 0 );
    m_pageSimple->buttonHelp3->setEnabled( m_pageSimple->comboFilename->currentIndex() == 
                                           m_pageSimple->comboFilename->count() - 1 );
    m_pageSimple->comboFilenameCustom->setEnabled( m_pageSimple->comboFilename->currentIndex() == 
                                                   m_pageSimple->comboFilename->count() - 1 );

    m_pageSimple->buttonHelp4->setEnabled( m_pageSimple->comboExtension->currentIndex() == 
                                           m_pageSimple->comboExtension->count() - 1 );
    m_pageSimple->comboExtensionCustom->setEnabled( m_pageSimple->comboExtension->currentIndex() == 
                                                    m_pageSimple->comboExtension->count() - 1 );
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
    m_pageSimple->listPreview->setKRenameModel( model );

    connect( model, SIGNAL( maxDotsChanged(int) ), SLOT( slotMaxDotsChanged(int)) );
}

void KRenameWindow::setPreviewModel( KRenamePreviewModel* model )
{
    m_pageSimple->listPreview->setModel( model );
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

    QItemSelectionModel* selection;

    if( m_eGuiMode == eGuiMode_Wizard ) 
        selection = m_pageSimple->listPreview->selectionModel();
    else
        selection = m_pageFilename->listPreview->selectionModel();

    QModelIndexList      indeces = selection->selectedIndexes();
    QModelIndexList::const_iterator it = indeces.begin();
    
    while( it != indeces.end() )
    {
        selected.append( (*it).row() );
        ++it;
    }

    return selected;
}

QString KRenameWindow::getPrefixSuffixSimple( QComboBox* combo, QComboBox* comboCustom ) 
{
    QString str;
    QString number = "#";
    int c = m_pageSimple->spinDigits->value()-1;

    while( c-- > 0 )
        number += "#";

    number += QString("{%1}").arg( m_pageSimple->spinIndex->value() );
    
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

void KRenameWindow::slotTemplateChanged()
{
    QString filename;
    QString extension;

    filename  = m_pageFilename->filenameTemplate->currentText();
    extension = m_pageFilename->checkExtension->isChecked() ? "$" : 
        m_pageFilename->extensionTemplate->currentText();

    emit filenameTemplateChanged( filename );
    emit extensionTemplateChanged( extension );

    emit updatePreview();

    m_pageFilename->buttonNumbering->setEnabled( filename.contains('#') || extension.contains('#') );
    this->slotEnableControls();
}

void KRenameWindow::slotSimpleTemplateChanged()
{
    QString filename  = getFilenameSimple( m_pageSimple->comboFilename, m_pageSimple->comboFilenameCustom );
    QString extension = getFilenameSimple( m_pageSimple->comboExtension, m_pageSimple->comboExtensionCustom );
    QString prefix    = getPrefixSuffixSimple( m_pageSimple->comboPrefix, m_pageSimple->comboPrefixCustom );
    QString suffix    = getPrefixSuffixSimple( m_pageSimple->comboSuffix, m_pageSimple->comboSuffixCustom );

    filename = prefix + filename + suffix;

    // set the new templates, but make sure signals 
    // are blockes so that slotTemplateChanged emits updatePreview()
    // which is calculation intensive only once!
    this->blockSignals( true );
    m_pageFilename->filenameTemplate->lineEdit()->setText( filename );
    m_pageFilename->extensionTemplate->lineEdit()->setText( extension );
    m_pageFilename->checkExtension->setChecked( false );
    this->blockSignals( false );
    this->slotTemplateChanged();
}

void KRenameWindow::slotTokenHelpRequested()
{
    emit showTokenHelpDialog( m_pageFilename->filenameTemplate->lineEdit() );
}

void KRenameWindow::slotTokenHelpRequestedWizard1()
{
    emit showTokenHelpDialog( m_pageSimple->comboPrefixCustom->lineEdit() );
}

void KRenameWindow::slotTokenHelpRequestedWizard2()
{
    emit showTokenHelpDialog( m_pageSimple->comboSuffixCustom->lineEdit() );
}

void KRenameWindow::slotTokenHelpRequestedWizard3()
{
    emit showTokenHelpDialog( m_pageSimple->comboFilenameCustom->lineEdit() );
}

void KRenameWindow::slotTokenHelpRequestedWizard4()
{
    emit showTokenHelpDialog( m_pageSimple->comboExtensionCustom->lineEdit() );
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
    }

    KRenameModel* model = static_cast<KRenameModel*>(m_pageFiles->fileList->model());
    model->sort( eMode );
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
    QAbstractItemView* view;

    if( m_eGuiMode == eGuiMode_Wizard ) 
        view = m_pageSimple->listPreview;
    else
        view = m_pageFilename->listPreview;

    moveUp( sel, view );
}

void KRenameWindow::slotMoveDownPreview()
{
    QList<int>         sel  = this->selectedFileItemsPreview();
    QAbstractItemView* view;

    if( m_eGuiMode == eGuiMode_Wizard ) 
        view = m_pageSimple->listPreview;
    else
        view = m_pageFilename->listPreview;

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


#include "krenamewindow.moc"
