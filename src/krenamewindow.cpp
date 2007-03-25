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

#include <QDialogButtonBox>
#include <QStackedWidget>
#include <QVBoxLayout>

KRenameWindow::KRenameWindow( bool wizardMode, QWidget* parent )
    : KMainWindow( parent )
{
    QWidget*     center = new QWidget( this );
    QVBoxLayout* layout = new QVBoxLayout( center );

    m_stack   = new QStackedWidget( center );
    m_buttons = new QDialogButtonBox( center );
    
    layout->addWidget( m_stack );
    layout->addWidget( m_buttons );

    this->setCentralWidget( center );
}

