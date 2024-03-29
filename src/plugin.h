// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2001 Dominik Seichter <domseichter@web.de>

#ifndef PLUGIN_H
#define PLUGIN_H

#include <QPixmap>
#include <QString>

#include <KConfigGroup>
#include <KSharedConfig>

class BatchRenamer;
class KConfigGroup;
class PluginLoader;

/** An enum to determine the correct plugin type.
 *
 *  A plugin may be of different types at a time.
 */
enum EPluginType {
    ePluginType_Token     = 0x01, ///< A plugin that handles a token in brackets [ ]
    ePluginType_Filename  = 0x02, ///< A plugin that transforms the complete final filename
    ePluginType_File      = 0x04  ///< A plugin that changes the finally renamed file
};

/** This is the abstract interface that has to be implemented
 *  by all KRename plugins.
 */
class Plugin
{
public:
    explicit Plugin(PluginLoader *loader);
    virtual ~Plugin();

    /**
     * Creates a help entry for the QStringList returned by help,
     * adds correct separator and brackets where necessary.
     *
     * \param token the token ([ brackets ] will be added to surround it)
     * \help help for the token
     *
     * \see help()
     */
    static QString createHelpEntry(const QString &token, const QString &help);

    /**
     * @returns a name of the plugin that can be displayed
     *          to the user. This name should be internationalized.
     */
    virtual const QString name() const = 0;

    /**
     * Determines the type of the plugin.
     * Different enum values may be or'ed together.
     *
     * @returns the type of the plugin.
     */
    virtual int type() const = 0;

    /**
     * @returns an icon for this plugin.
     */
    virtual const QIcon icon() const = 0;

    /** Set the enabled state of a plugin
     *  so that it can be used.
     *
     *  \param b the enabled state of the plugin.
     */
    inline void setEnabled(bool b);

    /**
     * @returns true if this plugin is enabled.
     * Only use it if it is enabled.
     */
    inline bool isEnabled() const;

    /**
     * @returns true if this plugins is always enabled
     *
     * Warning: If you return true here, the user has no possibility to
     *          disable this plugin.
     */
    virtual bool enabledByDefault() const = 0;

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
    virtual QString processFile(BatchRenamer *b, int index, const QString &filenameOrToken, EPluginType eCurrentType) = 0;

    /** Get a list of all tokens supported by this plugin.
     *
     *  If the token type != ePluginType_Token you have to return an empty list
     *
     *  @returns a list of all supported tokens. The returned strings will be treated
     *           as regular expressions to find a plugin which supports a token.
     */
    virtual const QStringList &supportedTokens() const = 0;

    /** Returns help descriptions for the supported tokens
     *
     *  The returned stringlist contains strings that are the tokens
     *  and the description separated by ;;
     *
     *  @returns a stringlist containing help on the supported tokens
     */
    virtual const QStringList &help() const = 0;

    /** Create a user interface for this plugin
     *
     *  @param parent the parent widget of this plugin
     */
    virtual void createUI(QWidget *parent) const = 0;

    /** Load the plugin configuration.
     *
     *  Called when plugins should load their configuration.
     *
     *  @param group config group where the configuration should be read from
     */
    virtual void loadConfig(KConfigGroup &group);

    /** Save the plugin configuration.
     *
     *  Called when plugins should save their configuration.
     *
     *  @param group config group where the configuration should be stored
     */
    virtual void saveConfig(KConfigGroup &group) const;

    /*
        virtual bool checkError() = 0;
        virtual void drawInterface( QWidget* w, QVBoxLayout* l ) = 0;
        virtual void fillStructure() { }
        virtual QString processFile( BatchRenamer* b, int i, QString token, int mode ) = 0;
        virtual void finished() { }

        virtual void addHelp( HelpDialogData* data );
        virtual void removeHelp(  HelpDialogData* data );

        virtual void clearCache();

        virtual const QPixmap getIcon() const;
        virtual const QStringList getKeys() const;
    */

protected:
    PluginLoader *m_pluginLoader;

private:
    bool m_enabled;
};

inline void Plugin::setEnabled(bool b)
{
    m_enabled = b;
}

inline bool Plugin::isEnabled() const
{
    return this->enabledByDefault() || m_enabled;
}

#endif // PLUGIN_H
