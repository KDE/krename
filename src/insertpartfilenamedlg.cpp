/***************************************************************************
               insertpartfilenamedlg.cpp  -  description
                             -------------------
    begin                : Sat Jun 30 2007
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

#include "insertpartfilenamedlg.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>

InsertPartFilenameDlg::InsertPartFilenameDlg( const QString & filename, QWidget* parent )
    : QDialog( parent ), m_start( -1 ), m_end( 0 )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    QWidget*     widget = new QWidget( this );

    m_widget.setupUi( widget );
    m_buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Close, Qt::Horizontal, this );

    m_widget.textFilename->setText( filename );
    m_widget.textFilename->setCursorPosition( 0 );

    layout->addWidget( widget );
    layout->addWidget( m_buttons );

    connect(m_buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));

    connect(m_widget.checkInvert,  SIGNAL(clicked(bool)),      this, SLOT( slotUpdateKRenameCommand() ));
    connect(m_widget.textFilename, SIGNAL(selectionChanged()), this, SLOT( slotSelectionChanged() )); 

    this->slotUpdateKRenameCommand();
}

void InsertPartFilenameDlg::slotSelectionChanged()
{
    m_start      = m_widget.textFilename->selectionStart();
    QString text = m_widget.textFilename->selectedText();
    m_end        = m_start + text.length();

    slotUpdateKRenameCommand();
}

void InsertPartFilenameDlg::slotUpdateKRenameCommand()
{
    m_command        = "";    
    bool hasSelected = m_start != -1;
    int  cursorPos   = m_widget.textFilename->cursorPosition();
    int  end         = m_end;
    int  start       = m_start;

    if( !m_widget.textFilename->text().isEmpty() )
    {
        if( m_widget.checkInvert->isChecked() && hasSelected )
        {
            // inverted
            if( end ) 
            {
                start++;
                end++;
                if( start > 1 )
                    m_command = QString("[$1;%1]").arg(start-1);

                if( end <= (signed int)m_widget.textFilename->text().length() )
                    m_command.append( QString("[$%1-[length]]").arg(end) );
            } 
        } 
        else if( m_widget.checkInvert->isChecked() && !hasSelected )
        {
            m_command = QString("[$1;%1][$%2-[length]]").arg(cursorPos).arg(cursorPos+1);            
        }
        else if( !m_widget.checkInvert->isChecked() && hasSelected )
        {
            if( end ) 
            {
                start++;
                end++;
                if( end <= (signed int)m_widget.textFilename->text().length() )
                    m_command = QString("[$%1;%2]").arg(start).arg(end-start);
                else
                    m_command = QString("[$%1-[length]]").arg(start);
            }
        } 
        else if( !m_widget.checkInvert->isChecked() && !hasSelected )
        {
            m_command = QString("[$%1-[length]]").arg( cursorPos );
        }

    }
    
    m_widget.labelPreview->setText( m_command );
}

#include "insertpartfilenamedlg.moc"
