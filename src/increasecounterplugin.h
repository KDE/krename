/***************************************************************************
                   increasecounterplugin.h  -  description
                             -------------------
    begin                : Tue Jul 15 2008
    copyright            : (C) 2008 by Dominik Seichter
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

#ifndef _INCREASE_COUNTER_PLUGIN_H_
#define _INCREASE_COUNTER_PLUGIN_H_

#include <plugin.h>

namespace Ui {
    class IncreaseCounterPluginWidget;
};

/** This is the abstract interface that has to be implemented
 *  by all KRename plugins.
 */
class IncreaseCounterPlugin : public QObject, public Plugin {

 Q_OBJECT

 public:
    IncreaseCounterPlugin( PluginLoader* loader );
    virtual ~IncreaseCounterPlugin();

    /** 
     * @returns a name of the plugin that can be displayed
     *          to the user. This name should be internationalized.
     */
    virtual const QString name() const;

    /** 
     * Determines the type of the plugin.
     * Different enum values may be or'ed together.
     *
     * @returns the type of the plugin.
     */
    inline virtual int type() const;

    /**
     * @returns an icon for this plugin.
     */
    virtual const QPixmap icon() const;

    /**
     * @returns true if this plugins is always enabled
     *
     * Warning: If you return true here, the user has no possibility to
     *          disable this plugin.
     */
    inline virtual bool alwaysEnabled() const;
    
    /**
     * This function is the core of your plugin.
     *
     * It does the actual processing of a file, filename or token depending of the type 
     * of your plugin.
     *
     * \see type()
     *
     * @param b the parent BatchRenamer instance calling this plugin
     * @param index the index of the current file (i.e. the first file has index 0,
     *              the second file to be renamed has index 1 ....)
     * @param filenameOrToken this parameter depends on the type of your plugin.
     *                        If type is ePluginType_File, this is the absolute path
     *                        or URL to the renamed file.
     *                        If type is ePluginType_Filename, this is the filename
     *                        (without path) as created by KRename.
     *                        If type is ePluginType_Token, this is the contents of a token
     *                        in brackets. If your plugin supports the token [example],
     *                        KRename will pass the strign "example" to your method.
     * @param eCurrentType the current type of plugin that is requested (for plugins that support more than one type)
     *
     * @returns the result of the function, depending on type().
     * @returns QString::null if this plugin has nothing to do.
     * @returns A new filename if type is ePluginType_Filename
     * @returns the value of the token if type is ePluginType_Token
     * @returns an error message or QString::null if type is ePluginType_File
     */
    virtual QString processFile( BatchRenamer* b, int index, const QString & filenameOrToken, EPluginType eCurrentType );

    /** Get a list of all tokens supported by this plugin. 
     *
     *  If the token type != ePluginType_Token you have to return an empty list
     *
     *  @returns a list of all supported tokens. The returned strings will be treated
     *           as regular expressions to find a plugin which supports a token.
     */
    inline virtual const QStringList & supportedTokens() const;

    /** Returns help descriptions for the supported tokens
     *
     *  The returned stringlist contains strings that are the tokens
     *  and the description separated by ;;
     *
     *  @returns a stringlist containing help on the supported tokens
     */
    inline virtual const QStringList & help() const;

    /** Create a user interface for this plugin
     *
     *  @param parent the parent widget of this plugin
     */
    virtual void createUI( QWidget* parent ) const;

 private slots:
    /**
     * Called when the user changes the offset through the UI
     *
     * @param offset the new offset
     */
    void slotOffsetChanged( int offset );

 private:
    Ui::IncreaseCounterPluginWidget* m_widget;
    
    int m_offset;         ///< Increase counter by this offset

    QStringList m_tmp;    ///< Dummy empty list so that we can return a reference for supported tokens and help
    QStringList m_users;  ///< List of all usernames on the system
    QStringList m_groups; ///< List of all groups on the system
};

inline int IncreaseCounterPlugin::type() const
{
    return ePluginType_Filename;
}

inline bool IncreaseCounterPlugin::alwaysEnabled() const
{
    return false;
}

inline const QStringList & IncreaseCounterPlugin::supportedTokens() const
{
    return m_tmp;
}

inline const QStringList & IncreaseCounterPlugin::help() const
{
    return m_tmp;
}

#endif // _INCREASE_COUNTER_PLUGIN_H_

