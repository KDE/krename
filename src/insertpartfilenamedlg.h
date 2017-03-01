/***************************************************************************
               insertpartfilenamedlg.h  -  description
                             -------------------
    begin                : Sat Jun 30 2007
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

#ifndef INSERTPARTFILENAMEDLG_H
#define INSERTPARTFILENAMEDLG_H

#include <QDialog>

#include "ui_partfilenamewidget.h"

class QDialogButtonBox;

class InsertPartFilenameDlg : public QDialog {
 Q_OBJECT

 public:
    explicit InsertPartFilenameDlg( const QString & filename, QWidget* parent = NULL );

    /**
     * \returns the command to insert a substring of the original
     *          filename into the new filename
     */
    inline const QString & command() const;

 private Q_SLOTS:
    void slotSelectionChanged();
    void slotUpdateKRenameCommand();

 private:
    Ui::PartFilenameWidget m_widget;
    QDialogButtonBox*      m_buttons;

    QString m_command;
    int     m_start;
    int     m_end;
};

inline const QString & InsertPartFilenameDlg::command() const
{
    return m_command;
}

#endif // INSERTPARTFILENAMEDLG_H
