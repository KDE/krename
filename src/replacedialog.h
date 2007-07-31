/***************************************************************************
                       replacedialog.h  -  description
                             -------------------
    begin                : Sun Jul 22 2007
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

#ifndef _REPLACE_DIALOG_H_
#define _REPLACE_DIALOG_H_

#include <QDialog>

#include "ui_replacewidget.h"
#include "batchrenamer.h"

class QDialogButtonBox;
class QTableWidgetItem;

/**
 * This is a dialog with a list of strings for find and replace
 */
class ReplaceDialog : public QDialog {
 Q_OBJECT
 public:
     ReplaceDialog( const QList<TReplaceItem> & items, QWidget* parent = NULL );

     QList<TReplaceItem> replaceList() const;

 private slots:
     
     void slotAdd();
     void slotEdit();
     void slotRemove();

     void slotEnableControls();

 private:
     QTableWidgetItem* createTableItem( const QString & text, bool isCheckable = false );

 private:
    Ui::ReplaceWidget   m_widget;
    QDialogButtonBox*   m_buttons;
};

#endif // _REPLACE_DIALOG_H_
