/***************************************************************************
                     customdialog.cpp  -  description
                             -------------------
    begin                : Sat Oct 06 2007
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

#include "customdialog.h"

#include "krenamefile.h"

CustomDialog::CustomDialog( const KRenameFile & file, QWidget* parent )
    : QDialog( parent )
{
    m_widget.setupUi( this );

    connect( m_widget.radioKRename, SIGNAL(clicked(bool)), this, SLOT(slotEnableControls()));
    connect( m_widget.radioInput,   SIGNAL(clicked(bool)), this, SLOT(slotEnableControls()));
    connect( m_widget.radioCustom,  SIGNAL(clicked(bool)), this, SLOT(slotEnableControls()));


    QString filename = file.srcFilename();
    if( !file.srcExtension().isEmpty() )
    {
        filename += ".";
        filename += file.srcExtension();
    }

    m_original = filename;

    if( !file.manualChanges().isNull() )
    {
        filename = file.manualChanges();
        m_widget.radioCustom->setChecked( true );
        m_widget.radioKRename->setChecked( false );
        m_widget.radioInput->setChecked( false );
    }

    m_widget.lineEdit->setText( filename );
    slotEnableControls();
}

CustomDialog::~CustomDialog()
{

}

void CustomDialog::slotEnableControls()
{
    m_widget.lineEdit->setEnabled( m_widget.radioCustom->isChecked() );
}

bool CustomDialog::hasManualChanges() const
{
    return !(m_widget.radioKRename->isChecked());
}

const QString & CustomDialog::manualChanges() const
{
    if( m_widget.radioCustom->isChecked() )
        return m_widget.lineEdit->text();
    else if( m_widget.radioInput->isChecked() )
        return m_original;
    else
        return QString::null;
}

#include "customdialog.moc"
