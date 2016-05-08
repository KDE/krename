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

#include <QDialog>
#include "ui_customfilenamedlg.h"
#include "batchrenamer.h"
#include <KConfigGroup>
#include <KSharedConfig>
 
class KRenameFile;

class CustomDialog : public QDialog {
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

    /**
     * \returns the manual change mode
     */
    EManualChangeMode manualChangeMode() const;

 private slots:
    void slotEnableControls();

 private:
    Ui::CustomFilenameDlg m_widget;
};

#endif // _CUSTOM_DIALOG_H_
