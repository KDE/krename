/***************************************************************************
                        progressdialog.cpp  -  description
                             -------------------
    begin                : Sun Jul 1 2007
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

#include "progressdialog.h"

#include <kiconloader.h>

ProgressDialog::ProgressDialog( QWidget* parent ) 
    : QDialog( parent ), m_canceled( false )
{
    m_widget.setupUi( this );

    connect( m_widget.buttonCancel, SIGNAL(clicked(bool)), SLOT(slotCancelled()));
}

void ProgressDialog::slotCancelled()
{
    m_canceled = true;
}

void ProgressDialog::print( const QString & text, const QString & pixmap )
{
    QPixmap icon = SmallIcon(pixmap);

    new QListWidgetItem( icon, text, m_widget.display );
}

void ProgressDialog::error( const QString & text )
{
    this->print( text, "cancel" );
}

void ProgressDialog::warning( const QString & text )
{
    this->print( text, "idea" );
}

#include "progressdialog.moc"
