// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2009 Dominik Seichter <domseichter@web.de>

#ifndef DIR_SORT_PLUGIN_H
#define DIR_SORT_PLUGIN_H

#include "plugin.h"
#include "ui_dirsortpluginwidget.h"

#include <QUrl>

class DirSortPlugin : public Plugin
{
public:
    explicit DirSortPlugin(PluginLoader *loader);
    ~DirSortPlugin() override;

    /**
     * @returns a name of the plugin that can be displayed
     *          to the user. This name should be internationalized.
     */
    const QString name() const override;

    /**
     * Determines the type of the plugin.
     * Different enum values may be or'ed together.
     *
     * @returns the type of the plugin.
     */
    int type() const override;

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
    bool enabledByDefault() const override;

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

    /** Get a list of all tokens supported by this plugin.
     *
     *  If the token type != ePluginType_Token you have to return an empty list
     *
     *  @returns a list of all supported tokens. The returned strings will be treated
     *           as regular expressions to find a plugin which supports a token.
     */
    const QStringList &supportedTokens() const override;

    /** Returns help descriptions for the supported tokens
     *
     *  The returned stringlist contains strings that are the tokens
     *  and the description separated by ;;
     *
     *  @returns a stringlist containing help on the supported tokens
     */
    const QStringList &help() const override;

    /** Create a user interface for this plugin
     *
     *  @param parent the parent widget of this plugin
     */
    void createUI(QWidget *parent) const override;

private:
    /**
     * Create a new subdirectory with the current setting
     * from m_digits and m_dirCounter.
     *
     * \returns the URL of the new subdirectory.
     */
    QUrl createNewSubdirectory() const;

protected:
    int m_dirCounter;
    int m_fileCounter;
    int m_filesPerDir;
    int m_digits;

    QUrl m_baseDirectory;
    QUrl m_currentDirectory;

    Ui::DirSortPluginWidget *m_widget;
    QStringList m_emptyList;

    bool m_valid;

};

#endif // DIR_SORT_PLUGIN
