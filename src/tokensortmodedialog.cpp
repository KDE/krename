/***************************************************************************
                       tokensortmodedialog.cpp  -  description
                             -------------------
    begin                : Wed Sep 15 2010
    copyright            : (C) 2010 by Dominik Seichter
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

#include "tokensortmodedialog.h"

#include "pluginloader.h"
#include "plugin.h"

TokenSortModeDialog::TokenSortModeDialog( KRenameTokenSorter::ESimpleSortMode eSortMode, QWidget* parent )
    : QDialog( parent )
{
    m_dialog.setupUi(this);

    m_dialog.radioAscending->setChecked( eSortMode == KRenameTokenSorter::eSimpleSortMode_Ascending );
    m_dialog.radioDescending->setChecked( eSortMode == KRenameTokenSorter::eSimpleSortMode_Descending );
    m_dialog.radioNumeric->setChecked( eSortMode == KRenameTokenSorter::eSimpleSortMode_Numeric );

    initTokens();
}

void TokenSortModeDialog::initTokens()
{
    QStringList allTokens;

    QList<Plugin*>::const_iterator it = PluginLoader::Instance()->plugins().begin();
    while( it != PluginLoader::Instance()->plugins().end() )
    {
        if( ((*it)->type() & ePluginType_Token) ) 
        {
            const QStringList & tokens = (*it)->supportedTokens(); 
            allTokens.append( tokens );
        }

        ++it;
    }

    allTokens.sort();
    m_dialog.comboToken->insertItems( -1, allTokens );
}

void TokenSortModeDialog::slotEnableControls()
{

}

KRenameTokenSorter::ESimpleSortMode TokenSortModeDialog::getSortMode() const
{
    KRenameTokenSorter::ESimpleSortMode eMode = KRenameTokenSorter::eSimpleSortMode_Ascending;

    if( m_dialog.radioAscending->isChecked() )
    {
        eMode = KRenameTokenSorter::eSimpleSortMode_Ascending;
    }
    else if( m_dialog.radioDescending->isChecked() )
    {
        eMode = KRenameTokenSorter::eSimpleSortMode_Descending;
    }
    else if( m_dialog.radioNumeric->isChecked() )
    {
        eMode = KRenameTokenSorter::eSimpleSortMode_Numeric;
    }

    return eMode;
}

QString TokenSortModeDialog::getToken() const
{
    return m_dialog.comboToken->currentText();
}
