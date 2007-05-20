/***************************************************************************
                       exthistorycombo.cpp  -  description
                             -------------------
    begin                : Sun May 20 2007
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

#include "exthistorycombo.h"

#define EXT_HISTORY_COMBO_MAX_COUNT 10 
#define EXT_HISTORY_COMBO_TIMER_DELAY 500

ExtHistoryCombo::ExtHistoryCombo( QWidget* parent )
    : KHistoryComboBox( parent )
{
    connect( this, SIGNAL( activated( const QString& )), SLOT( addToHistory( const QString& )));
    connect( this, SIGNAL( textChanged(const QString &)), SLOT( slotTextChanged()));
    connect( &m_timer, SIGNAL( timeout() ), this, SIGNAL( delayedTextChanged() ) );

    this->setMaxCount( EXT_HISTORY_COMBO_MAX_COUNT );
    this->setDuplicatesEnabled( false );

    m_timer.setSingleShot( true );
}

void ExtHistoryCombo::slotTextChanged()
{
    m_timer.stop();
    m_timer.start( EXT_HISTORY_COMBO_TIMER_DELAY );
}


#include "exthistorycombo.moc"
