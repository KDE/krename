// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>
// SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>

#ifndef SCRIPT_PLUGIN_H
#define SCRIPT_PLUGIN_H

#include "plugin.h"

#include <QJSEngine>

class QMenu;

class KRenameFile;

namespace Ui
{
class ScriptPluginWidget;
};

class ScriptPlugin : public QObject, public Plugin
{
    Q_OBJECT
public:
    /** Create a new ScriptPlugin from
     *
     */
    explicit ScriptPlugin(PluginLoader *loader);

    ~ScriptPlugin() override;

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
    QString processFile(BatchRenamer *b, int index, const QString &filenameOrToken, EPluginType eCurrentType) override;

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

    /** Load the plugin configuration.
     *
     *  Called when plugins should load their configuration.
     *
     *  @param group config group where the configuration should be read from
     */
    void loadConfig(KConfigGroup &group) override;

    /** Save the plugin configuration.
     *
     *  Called when plugins should save their configuration.
     *
     *  @param group config group where the configuration should be stored
     */
    void saveConfig(KConfigGroup &group) const override;

protected:
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

    /**
     * Set all KRename internal variables on the internal
     * Interpreter object.
     *
     * @param file the KRenameFile where the values can be retrieved
     * @param index index of the current file
     */
    void initKRenameVars(const KRenameFile &file, int index);

    /**
     * Insert a variable in the definitions textfield
     * at the current cursor position
     *
     * @param name variable name
     */
    void insertVariable(const char *name);

private Q_SLOTS:
    void slotEnableControls();
    void slotAdd();
    void slotRemove();
    void slotSave();
    void slotLoad();
    void slotTest();

    void slotInsertIndex();
    void slotInsertUrl();
    void slotInsertFilename();
    void slotInsertExtension();
    void slotInsertDirectory();

private:
    QString             m_name;
    QString             m_icon;

    QStringList         m_keys;
    QStringList         m_help;
    QJSEngine           m_engine;
    QWidget            *m_parent;
    QMenu              *m_menu;

    Ui::ScriptPluginWidget *m_widget;

    static const char  *s_pszFileDialogLocation; ///< Static URL for KFileDialog
    static const char *s_pszVarNameIndex;
    static const char *s_pszVarNameUrl;
    static const char *s_pszVarNameFilename;
    static const char *s_pszVarNameExtension;
    static const char *s_pszVarNameDirectory;
};

inline const QString ScriptPlugin::name() const
{
    return m_name;
}

inline bool ScriptPlugin::enabledByDefault() const
{
    return true;
}

inline int ScriptPlugin::type() const
{
    return ePluginType_Token;
}

inline const QStringList &ScriptPlugin::supportedTokens() const
{
    return m_keys;
}

inline const QStringList &ScriptPlugin::help() const
{
    return m_help;
}

#endif // FILE_PLUGIN_H
