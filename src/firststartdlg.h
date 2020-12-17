// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#ifndef FIRSTSTARTDLG_H
#define FIRSTSTARTDLG_H

#include <QDialog>

#include "ui_guimodeselector.h"
#include "krenamewindow.h" // for EGuiMode

class QDialogButtonBox;

/**
 * This dialog is shown when KRename is started the first time.
 * The user can select whether he wants to use KRename
 * in "Assistant" or "Advanced" mode.
 *
 * It is a thin wrapper around GuiModeSelector and a QDialogButtonBox
 * so that the user has an OK button.
 */
class FirstStartDlg : public QDialog
{
public:
    explicit FirstStartDlg(QWidget *parent = nullptr);

    /**
     * @returns the selected Gui Mode
     */
    inline EGuiMode guiMode() const;

private:
    Ui::GuiModeSelector m_selector;
    QDialogButtonBox   *m_buttons;
};

EGuiMode FirstStartDlg::guiMode() const
{
    return m_selector.optionWizard->isChecked() ? eGuiMode_Wizard : eGuiMode_Advanced;
}

#endif // FIRSTSTARTDLG_H
