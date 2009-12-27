/***************************************************************************
                        pluginloader.cpp  -  description
                             -------------------
    begin                : Sun Oct 7 2007
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

#include "pluginloader.h"

#include "krenameimpl.h"
#include "plugin.h"

#include "datetimeplugin.h"
#include "dirsortplugin.h"
#ifdef WITH_EXIV2
#include "exiv2plugin.h"
#endif // WITH_EXIV2
#include "fileplugin.h"
#include "increasecounterplugin.h"
#include "permissionsplugin.h"
#include "scriptplugin.h"
#include "systemplugin.h"
#ifdef WITH_TAGLIB 
# include "taglibplugin.h"
#endif // WITH_TAGLIB
#include "translitplugin.h"

#include "../config-krename.h"

#include <kservice.h>

PluginLoader* PluginLoader::s_instance = NULL;

PluginLoader* PluginLoader::Instance()
{
    if( !s_instance ) 
        s_instance = new PluginLoader();

    return s_instance;
}


PluginLoader::PluginLoader()
{
    this->load();
}

PluginLoader::~PluginLoader()
{
    this->clear();
}

Plugin* PluginLoader::findPlugin( const QString & token )
{
    QString lower = token.toLower();

    // first search in the cache
    Plugin* p = m_tokenCache[lower];
    if( p ) 
        return p;


    // now search in all tokens
    QMap<QString,Plugin*>::const_iterator it = m_tokenMap.begin();
    while( it != m_tokenMap.end() )
    {
        if( QRegExp( it.key() ).exactMatch( lower ) )
        {
            // we found a plugin
            // put the token into the cache for quick access
            p = it.value();
            m_tokenCache[lower] = p;
            return p;
        }

        ++it;
    }

    // add typos to the cache, too:
    // So that we find immediately that this key is not supported.
    m_tokenCache.insert( lower, NULL );
    return NULL;
}

Plugin* PluginLoader::findPluginByName( const QString & name )
{
    QList<Plugin*>::iterator it = m_plugins.begin();

    while( it != m_plugins.end() )
    {
        if( (*it)->name() == name ) 
            return *it;

        ++it;
    }

    return NULL;
}

void PluginLoader::clear()
{
    QList<Plugin*>::iterator it = m_plugins.begin();

    while( it != m_plugins.end() )
    {
        delete *it;

        ++it;
    }

    m_plugins.clear();
    m_tokenMap.clear();
    m_tokenCache.clear();
}

void PluginLoader::load()
{
#ifndef _WIN32
    m_plugins.append( new DateTimePlugin( this ) );
#endif // _WIN32
    m_plugins.append( new DirSortPlugin( this ) );
#ifdef WITH_EXIV2
    m_plugins.append( new Exiv2Plugin( this ) );
#endif // WITH_EXIV2
	m_plugins.append( new IncreaseCounterPlugin( this ) );
#ifndef _WIN32
	m_plugins.append( new PermissionsPlugin( this ) );
#endif // _WIN32
    m_plugins.append( new ScriptPlugin( this ) );
    m_plugins.append( new SystemPlugin( this ) );

#ifdef WITH_TAGLIB 
    m_plugins.append( new TagLibPlugin( this ) );
#endif // WITH_TAGLIB 
    m_plugins.append( new TranslitPlugin( this ) );
    //this->loadFilePlugins();


    // Fill the token map
    QList<Plugin*>::iterator it = m_plugins.begin();
    while( it != m_plugins.end() )
    {
        if( ((*it)->type() & ePluginType_Token) ) 
        {
            const QStringList & tokens = (*it)->supportedTokens(); 
            QStringList::const_iterator itList = tokens.begin();
            while( itList != tokens.end() )
            {
                m_tokenMap.insert( (*itList).toLower(), *it );
                ++itList;
            }
        }

        ++it;
    }
}

void PluginLoader::registerForUpdates( KRenameImpl* krename )
{
    m_observers.prepend( krename );
}

void PluginLoader::deregisterForUpdates( KRenameImpl* krename )
{
    m_observers.removeOne( krename );
}

void PluginLoader::sendUpdatePreview()
{
    QList<KRenameImpl*>::iterator it = m_observers.begin();

    while( it != m_observers.end() )
    {
	(*it)->slotUpdatePreview();
	
	++it;
    }
}


void PluginLoader::loadConfig( KConfigGroup & group ) 
{
    QList<Plugin*>::const_iterator it = m_plugins.begin();
    while( it != m_plugins.end() )
    {
	(*it)->loadConfig( group );
        ++it;
    }

}

void PluginLoader::saveConfig( KConfigGroup & group ) 
{
    QList<Plugin*>::const_iterator it = m_plugins.begin();
    while( it != m_plugins.end() )
    {
	(*it)->saveConfig( group );
        ++it;
    }

}
