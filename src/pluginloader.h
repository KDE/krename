/***************************************************************************
                        pluginloader.h  -  description
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

#ifndef _PLUGIN_LOADER_H_
#define _PLUGIN_LOADER_H_

#include <QHash>
#include <QList>
#include <QMap>

class Plugin;

class PluginLoader {
 public:
    ~PluginLoader();

    static PluginLoader* Instance();

    /** Find a plugin that supports a certain token
     *
     *  This works only for plugins of the type ePluginType_Token
     *
     *  @param token a token
     *  @returns a plugin or NULL
     */
    Plugin* findPlugin( const QString & token );

    /** A read-only list of all plugins
     *
     *  @returns a list of all plugins;
     */
    inline const QList<Plugin*> & plugins() const { return m_plugins; }

 private:
    PluginLoader();

    /** Clear the plugin loader.
     *  I.e reset the object to its initial state and unload all plugins
     */
    void clear();

    /** Load all plugins 
     */
    void load();

 private:

    static PluginLoader* s_instance;     ///< The handle to the only pluginloader instance
    QList<Plugin*>       m_plugins;      ///< The list of all plugins

    QMap<QString,Plugin*>  m_tokenMap;   ///< All supported tokens in brackets are listed here
    QHash<QString,Plugin*> m_tokenCache; ///< All used tokens are listed here
};

#endif // _PLUGIN_LOADER_H_
