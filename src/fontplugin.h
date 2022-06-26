// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2010 Dominik Seichter <domseichter@web.de>

#ifndef FONT_PLUGIN_H
#define FONT_PLUGIN_H

#include "fileplugin.h"

#include <QMap>

#include <ft2build.h>
#include FT_FREETYPE_H

class FontPlugin : public FilePlugin
{
public:
    explicit FontPlugin(PluginLoader *loader);

    ~FontPlugin() override;

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
     * @returns an empty QString if this plugin has nothing to do.
     * @returns A new filename if type is ePluginType_Filename
     * @returns the value of the token if type is ePluginType_Token
     * @returns an error message or an empty QString if type is ePluginType_File
     */
    QString processFile(BatchRenamer *b, int index, const QString &filenameOrToken, EPluginType eCurrentType) override;

    /** Returns help descriptions for the supported tokens
     *
     *  The returned stringlist contains strings that are the tokens
     *  and the description separated by ;;
     *
     *  @returns a stringlist containing help on the supported tokens
     */
    inline const QStringList &help() const override;

private:
    QStringList           m_help;

    FT_Library            m_library;
};

inline const QStringList &FontPlugin::help() const
{
    return m_help;
}

#endif // FONT_PLUGIN_H
