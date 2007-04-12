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

#include <kicon.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kseparator.h>

#include <QDialogButtonBox>
#include <QLabel>
#include <QStackedWidget>
#include <QTabBar>
#include <QVBoxLayout>

static const char* pageTitles[] = {
    I18N_NOOP( "Add some files\t\t(Step 1/3)" ),
    I18N_NOOP( "Set the destination\t\t(Step 2/3)" ),
    I18N_NOOP( "Filename\t\t(Step 3/3)" )
};

KRenameWindow::KRenameWindow( EGuiMode guiMode, QWidget* parent )
    : KMainWindow( parent ), 
      m_eGuiMode( guiMode ), m_curPage( 0 ), 
      m_buttonBack( NULL ), m_buttonNext( NULL )
{
    QWidget*     center = new QWidget( center );
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

    m_lblTitle = new QLabel( m_stackTop );
    m_stackTop->addWidget( m_lblTitle );

    m_tabBar = new QTabBar( m_stackTop );
    m_stackTop->addWidget( m_tabBar );

    connect( m_tabBar, SIGNAL(currentChanged(int)), SLOT(showPage(int)));

    QWidget* page = new QWidget( m_stack );
    m_pageFiles = new Ui::KRenameFiles();
    m_pageFiles->setupUi( page );
    m_stack->addWidget( page );
    m_tabBar->addTab( i18n("&Files") );

    page = new QWidget( m_stack );
    m_pageDests = new Ui::KRenameDestination();
    m_pageDests->setupUi( page );
    m_stack->addWidget( page );
    m_tabBar->addTab( i18n("&Destination") );
    
    setupGui();
}

void KRenameWindow::setupGui()
{
    m_buttons->clear();

    m_buttonBack  = NULL;
    m_buttonNext  = NULL;

    if( m_eGuiMode == eGuiMode_Wizard ) 
    {
        m_buttonBack = new KPushButton( KIcon( "back" ), i18n("&Back") );
        m_buttonNext = new KPushButton( KIcon( "next" ), i18n("&Next") );

        m_buttons->addButton( m_buttonBack, QDialogButtonBox::ActionRole );
        m_buttons->addButton( m_buttonNext, QDialogButtonBox::ActionRole );

        connect( m_buttonBack, SIGNAL(clicked(bool)),SLOT(slotBack()));
        connect( m_buttonNext, SIGNAL(clicked(bool)),SLOT(slotNext()));
    
        m_stackTop->setCurrentIndex( 0 ); // m_lblTitle
    }
    else
        m_stackTop->setCurrentIndex( 1 ); // m_tabBar

    m_buttonClose = new KPushButton( KIcon( "cancel" ), i18n("&Close") );
    m_buttons->addButton( m_buttonClose, QDialogButtonBox::RejectRole );

    enableControls();
}

void KRenameWindow::showPage( int index )
{
    if( index >= 0 && index < m_stack->count() )
    {
        m_curPage = index;
        m_stack->setCurrentIndex( index );
        m_lblTitle->setText( i18n( pageTitles[m_curPage] ) );

        enableControls();
    }
}

void KRenameWindow::enableControls()
{
    if( m_buttonNext )
        m_buttonNext->setEnabled( m_curPage < m_stack->count() - 1 );

    if( m_buttonBack )
        m_buttonBack->setEnabled( m_curPage > 0 );
}

void KRenameWindow::slotBack()
{
    this->showPage( m_curPage-1 );
}

void KRenameWindow::slotNext()
{
    this->showPage( m_curPage+1 );
}

#include "krenamewindow.moc"
