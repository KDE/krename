/***************************************************************************
                          fileplugin.h  -  description
                             -------------------
    begin                : Mon Jul 1 2002
    copyright            : (C) 2002 by Dominik Seichter
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

#ifndef _FILE_PLUGIN_H_
#define _FILE_PLUGIN_H_

#include "plugin.h"

class KService;

class FilePlugin : public Plugin {
 public:
    /** Create a new FilePlugin from a KService
     *
     *  @param service pointer to a KService
     */
    FilePlugin( KService* service );

    virtual ~FilePlugin();

    /** 
     * @returns a name of the plugin that can be displayed
     *          to the user. This name should be internationalized.
     */
    inline virtual const QString name() const;

    /** 
     * @returns the type of the plugin.
     */
    inline virtual EPluginType type() const;

    /**
     * @returns an icon for this plugin.
     */
    virtual const QPixmap icon() const;

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
     *
     * @returns the result of the function, depending on type().
     * @returns QString::null if this plugin has nothing to do.
     * @returns A new filename if type is ePluginType_Filename
     * @returns the value of the token if type is ePluginType_Token
     * @returns an error message or QString::null if type is ePluginType_File
     */
    virtual QString processFile( BatchRenamer* b, int index, const QString & filenameOrToken );

    /** Get a list of all tokens supported by this plugin. 
     *
     *  If the token type != ePluginType_Token you have to return an empty list
     *
     *  @returns a list of all supported tokens.
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

 protected:
    FilePlugin();

    /** 
     *  Checks if a token is supported by this plugin.
     *
     *  @param token a token
     *  @returns true if the token is supported
     *
     *  @see addSupportedToken
     */
    bool supports( const QString & token );
    
    /**
     *  Adds a token to the list of supported tokens
     *
     *  @param token will be a supported token from now on
     *
     *  @see supports
     */
    inline void addSupportedToken( const QString & token ) { m_keys.append( token ); }
 
 protected:

    QString m_name;
    QString m_icon;

 private:
    QStringList m_keys;
};


inline const QString FilePlugin::name() const
{
    return m_name;
}

inline EPluginType FilePlugin::type() const
{
    return ePluginType_Token;
}

inline const QStringList & FilePlugin::supportedTokens() const 
{ 
    return m_keys; 
}

inline const QStringList & FilePlugin::help() const 
{ 
    return m_keys; 
}

#endif // _FILE_PLUGIN_H_
