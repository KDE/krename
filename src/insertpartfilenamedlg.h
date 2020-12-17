// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#ifndef INSERTPARTFILENAMEDLG_H
#define INSERTPARTFILENAMEDLG_H

#include <QDialog>

#include "ui_partfilenamewidget.h"

class QDialogButtonBox;

class InsertPartFilenameDlg : public QDialog
{
    Q_OBJECT

public:
    explicit InsertPartFilenameDlg(const QString &filename, QWidget *parent = nullptr);

    /**
     * \returns the command to insert a substring of the original
     *          filename into the new filename
     */
    inline const QString &command() const;

private Q_SLOTS:
    void slotSelectionChanged();
    void slotUpdateKRenameCommand();

private:
    Ui::PartFilenameWidget m_widget;
    QDialogButtonBox      *m_buttons;

    QString m_command;
    int     m_start;
    int     m_end;
};

inline const QString &InsertPartFilenameDlg::command() const
{
    return m_command;
}

#endif // INSERTPARTFILENAMEDLG_H
