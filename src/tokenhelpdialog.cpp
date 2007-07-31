/***************************************************************************
                       tokenhelpdialog.cpp  -  description
                             -------------------
    begin                : Mon Jul 30 2007
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

#include "tokenhelpdialog.h"

#include <QDialogButtonBox>
#include <QPushButton>

TokenHelpDialog::TokenHelpDialog( QLineEdit* edit, QWidget* parent )
    : QDialog( parent ), m_edit( edit )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    QWidget*     widget = new QWidget( this );

    m_widget.setupUi( widget );
    m_buttons = new QDialogButtonBox( QDialogButtonBox::Close, Qt::Horizontal, this );
    QPushButton* insert = m_buttons->addButton( i18n("&Insert"), QDialogButtonBox::YesRole );
    

    layout->addWidget( widget );
    layout->addWidget( m_buttons );

    connect(insert, SIGNAL(clicked(bool)), SLOT(slotInsert()));
    connect(m_buttons, SIGNAL(rejected()), SLOT(reject()));
}

void TokenHelpDialog::add( const QString & headline, const QStringList & commands, const QPixmap & icon )
{
    m_map[headline] = commands;

    QTreeWidgetItem* item = new QTreeWidgetItem( m_widget.listCategories );
    item->setText( 0, headline );
}

void TokenHelpDialog::slotInsert()
{

    this->accept();
}

#include "tokenhelpdialog.moc"


