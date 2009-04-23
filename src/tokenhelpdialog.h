/***************************************************************************
                       tokenhelpdialog.h  -  description
                             -------------------
    begin                : Mon Jul 30 2007
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

#ifndef _TOKEN_HELP_DIALOG_H_
#define _TOKEN_HELP_DIALOG_H_

#include <kdialog.h>

#include "ui_tokenhelpwidget.h"

class QLineEdit;
class QTreeWidgetItem;

class TokenHelpDialog : public KDialog {
 Q_OBJECT
 public:
    TokenHelpDialog( QLineEdit* edit, QWidget* parent = NULL );

    /** Adds a category to the help dialog.
     *  The category will be added and all its commands will also be added to the "All" category.
     *
     *  \param headline headline of the category
     *  \param commands list of all tokens
     *  \param icon icon of the category
     *  \param first if true make sure that this is the first category (after all) and is selected by 
     *         default.
     */
    void add( const QString & headline, const QStringList & commands, const QPixmap & icon, bool first = false );

 public slots:
     int exec();

 private slots:
    void slotInsert();

    void slotCategoryChanged( QTreeWidgetItem* item );
    void saveConfig();

 private:
    void loadConfig();

 private:
    Ui::TokenHelpWidget m_widget;

    QLineEdit*          m_edit;

    QMap<QString,QStringList> m_map;

    QString             m_first;
};

#endif // _TOKEN_HELP_DIALOG_H_
