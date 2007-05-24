/***************************************************************************
                       numberdialog.h  -  description
                             -------------------
    begin                : The May 24 2007
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

#include "numberdialog.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>

NumberDialog::NumberDialog( QWidget* parent )
    : QDialog( parent )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    QWidget*     widget = new QWidget( this );

    m_widget.setupUi( widget );
    m_buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this );

    layout->addWidget( widget );
    layout->addWidget( m_buttons );

    connect(m_buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

QList<int> NumberDialog::skipNumbers() const
{
    QList<int> list;

    return list;
}


