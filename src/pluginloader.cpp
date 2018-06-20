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

#include <QtGlobal>

#include "pluginloader.h"

#include "krenameimpl.h"
#include "plugin.h"

#include "datetimeplugin.h"
#include "dirsortplugin.h"
#include "fileplugin.h"
#include "increasecounterplugin.h"
#include "permissionsplugin.h"
#include "scriptplugin.h"
#include "systemplugin.h"
#include "translitplugin.h"
#include "snumplugin.h"

#include <kservice.h>

#include <../config-krename.h>

#if HAVE_TAGLIB
# include "taglibplugin.h"
#endif // HAVE_TAGLIB
#if HAVE_EXIV2
#include "exiv2plugin.h"
#endif // HAVE_EXIV2
#if HAVE_PODOFO
# include "podofoplugin.h"
#endif // HAVE_PODOFO
#if HAVE_KARCHIVE
# include "odfplugin.h"
#endif // HAVE_KARCHIVE
#if HAVE_FREETYPE
# include "fontplugin.h"
#endif // HAVE_FREETYPE

PluginLoader *PluginLoader::s_instance = nullptr;

PluginLoader *PluginLoader::Instance()
{
    if (!s_instance) {
        s_instance = new PluginLoader();
    }

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

Plugin *PluginLoader::findPlugin(const QString &token)
{
    QString lower = token.toLower();

    // first search in the cache
    Plugin *p = m_tokenCache[lower];
    if (p) {
        return p;
    }

    // now search in all tokens
    QMap<QString, Plugin *>::const_iterator it = m_tokenMap.constBegin();
    while (it != m_tokenMap.constEnd()) {
        if (QRegExp(it.key()).exactMatch(lower)) {
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
    m_tokenCache.insert(lower, nullptr);
    return nullptr;
}

Plugin *PluginLoader::findPluginByName(const QString &name)
{
    QList<Plugin *>::iterator it = m_plugins.begin();

    while (it != m_plugins.end()) {
        if ((*it)->name() == name) {
            return *it;
        }

        ++it;
    }

    return nullptr;
}

void PluginLoader::clear()
{
    QList<Plugin *>::iterator it = m_plugins.begin();

    while (it != m_plugins.end()) {
        delete *it;

        ++it;
    }

    m_plugins.clear();
    m_tokenMap.clear();
    m_tokenCache.clear();
}

void PluginLoader::load()
{
#ifndef Q_OS_WIN
    m_plugins.append(new DateTimePlugin(this));
#endif
    m_plugins.append(new DirSortPlugin(this));
#if HAVE_EXIV2
    m_plugins.append(new Exiv2Plugin(this));
#endif // HAVE_EXIV2
#if HAVE_FREETYPE
    m_plugins.append(new FontPlugin(this));
#endif // HAVE_FREETYPE
    m_plugins.append(new IncreaseCounterPlugin(this));
#ifndef Q_OS_WIN
    m_plugins.append(new PermissionsPlugin(this));
#endif
    m_plugins.append(new ScriptPlugin(this));
    m_plugins.append(new SystemPlugin(this));

#if HAVE_TAGLIB
    m_plugins.append(new TagLibPlugin(this));
#endif // HAVE_TAGLIB
#if HAVE_PODOFO
    m_plugins.append(new PodofoPlugin(this));
#endif // HAVE_PODOFO
#if HAVE_KARCHIVE
    m_plugins.append(new OdfPlugin(this));
#endif // HAVE_KARCHIVE
    m_plugins.append(new TranslitPlugin(this));
    m_plugins.append(new SnumPlugin(this));
    //this->loadFilePlugins();

    // Fill the token map
    QList<Plugin *>::iterator it = m_plugins.begin();
    while (it != m_plugins.end()) {
        if (((*it)->type() & ePluginType_Token)) {
            const QStringList &tokens = (*it)->supportedTokens();
            QStringList::const_iterator itList = tokens.begin();
            while (itList != tokens.end()) {
                m_tokenMap.insert((*itList).toLower(), *it);
                ++itList;
            }
        }

        ++it;
    }
}

void PluginLoader::registerForUpdates(KRenameImpl *krename)
{
    m_observers.prepend(krename);
}

void PluginLoader::deregisterForUpdates(KRenameImpl *krename)
{
    m_observers.removeOne(krename);
}

void PluginLoader::sendUpdatePreview()
{
    QList<KRenameImpl *>::iterator it = m_observers.begin();

    while (it != m_observers.end()) {
        (*it)->slotUpdatePreview();

        ++it;
    }
}

void PluginLoader::loadConfig(KConfigGroup &group)
{
    QList<Plugin *>::const_iterator it = m_plugins.constBegin();
    while (it != m_plugins.constEnd()) {
        (*it)->loadConfig(group);
        ++it;
    }

}

void PluginLoader::saveConfig(KConfigGroup &group)
{
    QList<Plugin *>::const_iterator it = m_plugins.constBegin();
    while (it != m_plugins.constEnd()) {
        (*it)->saveConfig(group);
        ++it;
    }

}
