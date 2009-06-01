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
#include <QObject>

class Plugin;
class KConfigGroup;
class KRenameImpl;

class PluginLoader  {
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

    /** Find a plugin by its name
     *
     *  @param name as returned by Plugin::name()
     *  @returns a plugin or NULL
     */
    Plugin* findPluginByName( const QString & name );

    /** A read-only list of all plugins
     *
     *  @returns a list of all plugins;
     */
    inline const QList<Plugin*> & plugins() const { return m_plugins; }

    /** This maybe called by plugins,
     *  if a setting in their UI was changed
     *  so that the preview in KRename
     *  should be updated.
     */
    void sendUpdatePreview();

    /** Save the plugin configuration.
     *
     *  Called when plugins should save their configuration.
     *
     *  @param group config group where the configuration should be stored
     */
    void saveConfig( KConfigGroup & group );

    /** Load the plugin configuration.
     *
     *  Called when plugins should load their configuration.
     *
     *  @param group config group where the configuration should be read from
     */
    void loadConfig( KConfigGroup & group );

    void registerForUpdates( KRenameImpl* kreanme );
    void deregisterForUpdates( KRenameImpl* kreanme );
    
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

    QList<KRenameImpl*>  m_observers;    ///< A list of KRenameImpls that should be notified on updates
};

#endif // _PLUGIN_LOADER_H_
