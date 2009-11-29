/***************************************************************************
                     customdialog.h  -  description
                             -------------------
    begin                : Sat Oct 06 2007
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

#ifndef _CUSTOM_DIALOG_H_
#define _CUSTOM_DIALOG_H_

#include <kdialog.h>
#include "ui_customfilenamedlg.h"

class KRenameFile;

class CustomDialog : public KDialog {
 Q_OBJECT
 public:
    CustomDialog( const KRenameFile & file, QWidget* parent );
    ~CustomDialog();

    /** 
     *  \returns true if the current user settings
     *           specify manual changes to the filename
     */
    bool hasManualChanges() const;

    /**
     * \returns the manually changed filename
     */
    const QString manualChanges() const;

 private slots:
    void slotEnableControls();

 private:
    Ui::CustomFilenameDlg m_widget;
    QString               m_original;
};

#endif // _CUSTOM_DIALOG_H_
