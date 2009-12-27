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
    : KDialog( parent )
{
    m_widget.setupUi( this->mainWidget() );

    connect( m_widget.radioKRename, SIGNAL(clicked(bool)), this, SLOT(slotEnableControls()));
    connect( m_widget.radioInput,   SIGNAL(clicked(bool)), this, SLOT(slotEnableControls()));
    connect( m_widget.radioCustom,  SIGNAL(clicked(bool)), this, SLOT(slotEnableControls()));

    // Set default vallues
    m_widget.radioCustom->setChecked( true );
    m_widget.radioKRename->setChecked( false );
    m_widget.radioInput->setChecked( false );

    QString srcFilename = file.srcFilename();
    if( !file.srcExtension().isEmpty() )
    {
        srcFilename += ".";
        srcFilename += file.srcExtension();
    }

    QString krenameFilename = file.dstFilename();
    if( !file.dstExtension().isEmpty() ) 
    {
        krenameFilename += ".";
        krenameFilename += file.dstExtension();
    }

    if( !file.manualChanges().isNull() )
    {
        switch( file.manualChangeMode() ) 
        {
            case eManualChangeMode_Custom:
                krenameFilename = file.manualChanges();
                break;
            case eManualChangeMode_Input:
                m_widget.radioInput->setChecked( true );
                m_widget.radioKRename->setChecked( false );
                m_widget.radioCustom->setChecked( false );
                srcFilename = file.manualChanges();
                break;
            case eManualChangeMode_None:
            default:
                break;
        }
    }

    m_widget.labelPreview->setPixmap( file.icon() );
    m_widget.lineEditInput->setText( srcFilename );
    m_widget.lineEditCustom->setText( krenameFilename );
    slotEnableControls();

    KSharedConfigPtr config = KGlobal::config();
    KConfigGroup group = config->group( QString("CustomDialogGroup") );
    this->restoreDialogSize(group);
}

CustomDialog::~CustomDialog()
{
    KSharedConfigPtr config = KGlobal::config();
    KConfigGroup group = config->group( QString("CustomDialogGroup") );
    this->saveDialogSize(group);
}

void CustomDialog::slotEnableControls()
{
    m_widget.lineEditCustom->setEnabled( m_widget.radioCustom->isChecked() );
    m_widget.lineEditInput->setEnabled( m_widget.radioInput->isChecked() );
}

bool CustomDialog::hasManualChanges() const
{
    return !(m_widget.radioKRename->isChecked());
}

const QString CustomDialog::manualChanges() const
{
    if( m_widget.radioCustom->isChecked() )
        return m_widget.lineEditCustom->text();
    else if( m_widget.radioInput->isChecked() )
        return m_widget.lineEditInput->text();
    else
        return QString::null;
}

EManualChangeMode CustomDialog::manualChangeMode() const
{
    if( m_widget.radioCustom->isChecked() )
        return eManualChangeMode_Custom;
    else if( m_widget.radioInput->isChecked() )
        return eManualChangeMode_Input;
    else
        return eManualChangeMode_None;

}

#include "customdialog.moc"
