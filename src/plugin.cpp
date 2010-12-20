/***************************************************************************
                          plugin.h  -  description
                             -------------------
    begin                : Sun Dec 30 2001
    copyright            : (C) 2001 by Dominik Seichter
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

#include "plugin.h"

#include "tokenhelpdialog.h"

const QString Plugin::S_TOKEN_SEPARATOR = TokenHelpDialog::S_TOKEN_SEPARATOR;

Plugin::Plugin( PluginLoader* loader )
    : m_pluginLoader( loader ), m_enabled( false )
{
}

Plugin::~Plugin()
{

}

void Plugin::loadConfig( KConfigGroup & )
{
    /* Do nothing by default */
}

void Plugin::saveConfig( KConfigGroup & ) const
{
    /* Do nothing by default */
}

QString Plugin::createHelpEntry( const QString & token, const QString & help )
{
  QString cmd = "[";
  cmd = cmd + token + "]" + Plugin::S_TOKEN_SEPARATOR;
  cmd = cmd + help;

  return cmd;
}

