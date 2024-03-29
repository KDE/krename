// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#ifndef TOKEN_HELP_DIALOG_H
#define TOKEN_HELP_DIALOG_H

#include <QDialog>

#include "ui_tokenhelpwidget.h"
#include <KTreeWidgetSearchLineWidget>

class QLineEdit;
class QTreeWidgetItem;

class BatchRenamer;
class KRenameModel;
class KRenameUnformattedWrappedModel;

class TokenHelpDialog : public QDialog
{
    Q_OBJECT
public:
    TokenHelpDialog(KRenameModel *model, BatchRenamer *renamer,
                    QLineEdit *edit, QWidget *parent = nullptr);

    ~TokenHelpDialog() override;

    /** Adds a category to the help dialog.
     *  The category will be added and all its commands will also be added to the "All" category.
     *
     *  \param headline headline of the category
     *  \param commands list of all tokens
     *  \param icon icon of the category
     *  \param first if true make sure that this is the first category (after all) and is selected by
     *         default.
     */
    void add(const QString &headline, const QStringList &commands, const QIcon &icon, bool first = false);

    /**
     * Retrieve the separator between token and help
     * \returns the token separator
     */
    static const QString getTokenSeparator();

public Q_SLOTS:
    int exec() override;

private Q_SLOTS:
    void slotInsert();

    void slotCategoryChanged(QTreeWidgetItem *item);
    void saveConfig();

    void slotEnableControls();
    void slotPreviewClicked(bool bPreview);
    void slotUpdatePreview();

private:
    void loadConfig();

    /**
     * Select a category by its name.
     * \param category user visible name of a category
     */
    void selectCategory(const QString &category);

    /**
     * Add all tokens stored in the list
     * m_recent to the list of tokens and to categories.
     */
    void addRecentTokens();

    /**
     * Add a token to the list of recent tokens.
     * \param token the token itself
     * \param help the help string for the token
     */
    void addToRecentTokens(const QString &token, const QString &help);

private:
    KRenameUnformattedWrappedModel *m_model;

    Ui::TokenHelpWidget m_widget;

    QLineEdit          *m_edit;
    BatchRenamer       *m_renamer;

    QMap<QString, QStringList> m_map;

    QString             m_first;

    QString             m_lastSelected;
    QStringList         m_recent;

    static const int    S_MAX_RECENT; ///< Maximum number of recent tokens
};

#endif // TOKEN_HELP_DIALOG_H
