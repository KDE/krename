/***************************************************************************
                       firststartdlg.h  -  description
                             -------------------
    begin                : Sat Mar 24 2007
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

#ifndef _FIRSTSTARTDLG_H_
#define _FIRSTSTARTDLG_H_

#include <QDialog>

#include "ui_guimodeselector.h"

class QDialogButtonBox;

/**
 * This dialog is shown when KRename is started the first time.
 * The user can select wether he wants to use KRename
 * in "Wizard" or "Advanced" mode.
 *
 * It is a thin wrapper around GuiModeSelector and a QDialogButtonBox
 * so that the user has an OK button.
 */
class FirstStartDlg : public QDialog {
 public:
    FirstStartDlg( QWidget* parent = NULL );

    /** 
     * @returns true if the user selected the wizard mode
     *          false if the user want to use advanced mode.
     */
    inline bool useWizardMode() const;

 private:
    Ui::GuiModeSelector m_selector;
    QDialogButtonBox*   m_buttons;
};

bool FirstStartDlg::useWizardMode() const
{
    return m_selector.optionWizard->isChecked();
}

#endif // _FIRSTSTARTDLG_H_
