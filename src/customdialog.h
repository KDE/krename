// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#ifndef CUSTOM_DIALOG_H
#define CUSTOM_DIALOG_H

#include <QDialog>
#include "ui_customfilenamedlg.h"
#include "batchrenamer.h"
#include <KSharedConfig>

class KRenameFile;

class CustomDialog : public QDialog
{
    Q_OBJECT
public:
    CustomDialog(const KRenameFile &file, QWidget *parent);
    ~CustomDialog() override;

    /**
     *  \returns true if the current user settings
     *           specify manual changes to the filename
     */
    bool hasManualChanges() const;

    /**
     * \returns the manually changed filename
     */
    const QString manualChanges() const;

    /**
     * \returns the manual change mode
     */
    EManualChangeMode manualChangeMode() const;

private Q_SLOTS:
    void slotEnableControls();

private:
    Ui::CustomFilenameDlg m_widget;
};

#endif // CUSTOM_DIALOG_H
