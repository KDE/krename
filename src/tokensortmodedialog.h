// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2010 Dominik Seichter <domseichter@web.de>

#ifndef TOKEN_SORT_MODE_DIALOG_H
#define TOKEN_SORT_MODE_DIALOG_H

#include <QDialog>

#include "ui_tokensortmodedialog.h"
#include "krenametokensorter.h"

/**
 * This is a dialog to define custom sorting.
 */
class TokenSortModeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TokenSortModeDialog(KRenameTokenSorter::ESimpleSortMode eSortMode, QWidget *parent = nullptr);

    /**
     * Get the currently selected sort mode.
     * @returns the sort mode
     */
    KRenameTokenSorter::ESimpleSortMode getSortMode() const;

    /**
     * Get the token selected by the user.
     * @returns the token
     */
    QString getToken() const;

private:
    /**
     * Load all available tokens.
     */
    void initTokens();

private Q_SLOTS:

    void slotEnableControls();

private:
    Ui::TokenSortModeDialog m_dialog;
};

#endif // TOKEN_SORT_MODE_DIALOG_H
