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
#include <QLabel>
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

    // Make sure that now signal occurs before setupGui was called
    connect( m_tabBar, SIGNAL(currentChanged(int)), SLOT(showPage(int)));
    connect( m_buttonClose, SIGNAL(clicked(bool)), SLOT(close()));

    // Show the first page in any mode
    showPage( 0 );
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

    setupSlots();
    slotEnableControls();
}

void KRenameWindow::setupSlots()
{
    connect( m_pageDests->optionRename,    SIGNAL(clicked(bool)), SLOT(slotRenameModeChanged()));
    connect( m_pageDests->optionCopy,      SIGNAL(clicked(bool)), SLOT(slotRenameModeChanged()));
    connect( m_pageDests->optionMove,      SIGNAL(clicked(bool)), SLOT(slotRenameModeChanged()));
    connect( m_pageDests->optionLink,      SIGNAL(clicked(bool)), SLOT(slotRenameModeChanged()));
    connect( m_pageDests->checkUndoScript, SIGNAL(clicked(bool)), SLOT(slotEnableControls()));

    connect( m_pageFilename->checkExtension,     SIGNAL(clicked(bool))       , SLOT(slotEnableControls()));
    connect( m_pageFilename->buttonNumbering,    SIGNAL(clicked(bool))       , SIGNAL(showAdvancedNumberingDialog()));

    connect( m_pageFilename->filenameTemplate,   SIGNAL(delayedTextChanged()), SLOT(slotTemplateChanged()));
    connect( m_pageFilename->extensionTemplate,  SIGNAL(delayedTextChanged()), SLOT(slotTemplateChanged()));
    connect( m_pageSimple->comboFilenameCustom,  SIGNAL(delayedTextChanged()), SLOT(slotTemplateChanged()));
    connect( m_pageSimple->comboSuffixCustom,    SIGNAL(delayedTextChanged()), SLOT(slotTemplateChanged()));
    connect( m_pageSimple->comboPrefixCustom,    SIGNAL(delayedTextChanged()), SLOT(slotTemplateChanged()));
    connect( m_pageSimple->comboExtensionCustom, SIGNAL(delayedTextChanged()), SLOT(slotTemplateChanged()));
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
    m_pageDests->groupUndo->setEnabled( !m_pageDests->optionCopy->isChecked() );
    m_pageDests->undorequester->setEnabled( m_pageDests->checkUndoScript->isChecked() );

    m_pageFilename->extensionTemplate->setEnabled( !m_pageFilename->checkExtension->isChecked() );
}

void KRenameWindow::setCount( unsigned int count )
{
    m_fileCount = count;
    m_pageFiles->labelCount->setText( i18n("<b>Files:<b> %1", m_fileCount ) );

    this->slotEnableControls();
}

void KRenameWindow::slotBack()
{
    this->showPage( m_curPage-1 );
}

void KRenameWindow::slotNext()
{
    this->showPage( m_curPage+1 );
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

    if( m_eGuiMode == eGuiMode_Wizard ) 
    {


    }
    else
    {
        filename  = m_pageFilename->filenameTemplate->currentText();
        extension = m_pageFilename->checkExtension->isChecked() ? "$" : 
            m_pageFilename->extensionTemplate->currentText();
    }

    emit filenameTemplateChanged( filename );
    emit extensionTemplateChanged( extension );

    emit updatePreview();

    m_pageFilename->buttonNumbering->setEnabled( filename.contains('#') || extension.contains('#') );
    this->slotEnableControls();
}

#include "krenamewindow.moc"
