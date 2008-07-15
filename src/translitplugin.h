/***************************************************************************
                         translitplugin.h  -  description
                             -------------------
    begin                : Sat Oct 13 2007
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

#ifndef _TRANSLIT_PLUGIN_H_
#define _TRANSLIT_PLUGIN_H_

#include "fileplugin.h"

#include <QHash>

class TranslitPlugin : public FilePlugin {
 public:
    TranslitPlugin( PluginLoader* loader );
    
    ~TranslitPlugin();

    /** 
     * @returns the type of the plugin.
     */
    inline virtual int type() const;

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

    /** Returns help descriptions for the supported tokens
     *
     *  The returned stringlist contains strings that are the tokens
     *  and the description separated by ;;
     *
     *  @returns a stringlist containing help on the supported tokens
     */
    inline const QStringList & help() const;

    /** Create a user interface for this plugin
     *
     *  @param parent the parent widget of this plugin
     */
    //virtual void createUI( QWidget* parent ) const;

 private:
    /** Transliterate a string and return the 
     *  transliterated version.
     *
     *  @param unicoded the string which is to be transliterated
     *
     *  @returns a transliterated copy of the string
     */
    QString transliterate(const QString & unicoded);

 private:
    QStringList m_help;

    static const QString s_strUtf8[];
    static const QString s_strEngl[];

    static QHash<QString,QString> s_mapFromUTF8;
};

inline int TranslitPlugin::type() const
{
    //return ePluginType_Filename | ePluginType_Token;
    return ePluginType_Token;
}

inline const QStringList & TranslitPlugin::help() const
{
    return m_help;
}

#endif // _TRANSLIT_PLUGIN_H_
