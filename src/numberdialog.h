/***************************************************************************
                       numberdialog.h  -  description
                             -------------------
    begin                : The May 24 2007
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

#ifndef _NUMBER_DIALOG_H_
#define _NUMBER_DIALOG_H_

#include <QDialog>

#include "ui_numberwidget.h"

class QDialogButtonBox;

/**
 * This is a dialog with advanced numbering settings
 *  - start index
 *  - stepping
 *  - reset counter on each new directory
 *  - skip numbers list
 *
 */
class NumberDialog : public QDialog {
 Q_OBJECT
 public:
    NumberDialog( int start, int step, bool reset, QList<int> skip, QWidget* parent = NULL );

    /** 
     * @returns the start index the user has selected
     */
    inline int startIndex() const;

    /** 
     * @returns the stepping value for counters the user has selected
     */
    inline int numberStepping() const;

    /**
     * @returns true if the counter should be reset for each new directory
     */
    inline bool resetCounter() const;

    /** 
     * @returns the list of number that should be skipped in counters
     */
    QList<int> skipNumbers() const;

 private slots:
     
     void slotAddNumber();
     void slotRemoveNumber();

     void slotEnableControls();

 private:
    Ui::NumberWidget    m_widget;
    QDialogButtonBox*   m_buttons;
};

inline int NumberDialog::startIndex() const
{
    return m_widget.spinStart->value();
}

inline int NumberDialog::numberStepping() const
{
    return m_widget.spinStep->value();
}

inline bool NumberDialog::resetCounter() const
{
    return m_widget.checkReset->isChecked();
}


#endif // _NUMBER_DIALOG_H_
