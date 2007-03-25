/***************************************************************************
                       firststartdlg.cpp  -  description
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

#include "firststartdlg.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>

FirstStartDlg::FirstStartDlg( QWidget* parent )
    : QDialog( parent )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    QWidget*     widget = new QWidget( this );

    m_selector.setupUi( widget );
    m_buttons = new QDialogButtonBox( QDialogButtonBox::Ok, Qt::Horizontal, this );

    layout->addWidget( widget );
    layout->addWidget( m_buttons );

    connect(m_buttons, SIGNAL(accepted()), this, SLOT(accept()));
}

