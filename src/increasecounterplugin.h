// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2008 Dominik Seichter <domseichter@web.de>

#ifndef INCREASE_COUNTER_PLUGIN_H
#define INCREASE_COUNTER_PLUGIN_H

#include <plugin.h>

namespace Ui
{
class IncreaseCounterPluginWidget;
};

/** This is the abstract interface that has to be implemented
 *  by all KRename plugins.
 */
class IncreaseCounterPlugin : public QObject, public Plugin
{

    Q_OBJECT

public:
    explicit IncreaseCounterPlugin(PluginLoader *loader);
    ~IncreaseCounterPlugin() override;

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
    QString processFile(BatchRenamer *b, int index, const QString &filenameOrToken, EPluginType eCurrentType) override;

    /** Get a list of all tokens supported by this plugin.
     *
     *  If the token type != ePluginType_Token you have to return an empty list
     *
     *  @returns a list of all supported tokens. The returned strings will be treated
     *           as regular expressions to find a plugin which supports a token.
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
     */
    void createUI(QWidget *parent) const override;

private Q_SLOTS:
    /**
     * Called when the user changes the offset through the UI
     *
     * @param offset the new offset
     */
    void slotOffsetChanged(int offset);

private:
    Ui::IncreaseCounterPluginWidget *m_widget;

    int m_offset;         ///< Increase counter by this offset

    QStringList m_tmp;    ///< Dummy empty list so that we can return a reference for supported tokens and help
    QStringList m_users;  ///< List of all usernames on the system
    QStringList m_groups; ///< List of all groups on the system
};

inline int IncreaseCounterPlugin::type() const
{
    return ePluginType_Filename;
}

inline bool IncreaseCounterPlugin::enabledByDefault() const
{
    return false;
}

inline const QStringList &IncreaseCounterPlugin::supportedTokens() const
{
    return m_tmp;
}

inline const QStringList &IncreaseCounterPlugin::help() const
{
    return m_tmp;
}

#endif // INCREASE_COUNTER_PLUGIN_H

