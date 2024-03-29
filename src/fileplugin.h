// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2002 Dominik Seichter <domseichter@web.de>

#ifndef FILE_PLUGIN_H
#define FILE_PLUGIN_H

#include "plugin.h"

class KService;

class FilePlugin : public Plugin
{
public:
    /** Create a new FilePlugin from a KService
     *
     *  @param service pointer to a KService
     */
    FilePlugin(PluginLoader *loader, KService *service);

    ~FilePlugin() override;

    /**
     * @returns a name of the plugin that can be displayed
     *          to the user. This name should be internationalized.
     */
    inline const QString name() const override;

    /**
     * @returns the type of the plugin.
     */
    inline int type() const override;

    /**
     * @returns an icon for this plugin.
     */
    const QIcon icon() const override;

    /**
     * @returns true if this plugins is always enabled
     *
     * Warning: If you return true here, the user has no possibility to
     *          disable this plugin.
     */
    inline bool enabledByDefault() const override;

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
    QString processFile(BatchRenamer *b, int index, const QString &filenameOrToken, EPluginType eCurrentType) override = 0;

    /** Get a list of all tokens supported by this plugin.
     *
     *  If the token type != ePluginType_Token you have to return an empty list
     *
     *  @returns a list of all supported tokens.
     */
    inline const QStringList &supportedTokens() const override;

    /** Returns help descriptions for the supported tokens
     *
     *  The returned stringlist contains strings that are the tokens
     *  and the description separated by ;;
     *
     *  @returns a stringlist containing help on the supported tokens
     */
    inline const QStringList &help() const override;

    /** Create a user interface for this plugin
     *
     *  @param parent the parent widget of this plugin
     *
     *  This is implemented here for all FilePlugin based classed
     */
    void createUI(QWidget *parent) const override;

protected:
    FilePlugin(PluginLoader *loader);

    /**
     *  Checks if a token is supported by this plugin.
     *
     *  @param token a token
     *  @returns true if the token is supported
     *
     *  @see addSupportedToken
     */
    bool supports(const QString &token);

    /**
     *  Adds a token to the list of supported tokens
     *
     *  @param token will be a supported token from now on
     *
     *  @see supports
     */
    inline void addSupportedToken(const QString &token)
    {
        m_keys.append(token);
    }

protected:

    QString m_name;
    QString m_comment;
    QString m_icon;

private:
    QStringList m_keys;
};

inline const QString FilePlugin::name() const
{
    return m_name;
}

inline bool FilePlugin::enabledByDefault() const
{
    return true;
}

inline int FilePlugin::type() const
{
    return ePluginType_Token;
}

inline const QStringList &FilePlugin::supportedTokens() const
{
    return m_keys;
}

inline const QStringList &FilePlugin::help() const
{
    return m_keys;
}

#endif // FILE_PLUGIN_H
