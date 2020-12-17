// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#ifndef REPLACE_DIALOG_H
#define REPLACE_DIALOG_H

#include <QDialog>

#include "ui_replacewidget.h"
#include "batchrenamer.h"

class QDialogButtonBox;
class QTableWidgetItem;

/**
 * This is a dialog with a list of strings for find and replace
 */
class ReplaceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ReplaceDialog(const QList<TReplaceItem> &items, QWidget *parent = nullptr);

    QList<TReplaceItem> replaceList() const;

private Q_SLOTS:

    void slotAdd();
    void slotEdit();
    void slotRemove();

    void slotSaveList();
    void slotLoadList();

    void slotEnableControls();

private:
    QTableWidgetItem *createTableItem(const QString &text, bool isCheckable = false);

private:
    Ui::ReplaceWidget   m_widget;
    QDialogButtonBox   *m_buttons;
};

#endif // REPLACE_DIALOG_H
