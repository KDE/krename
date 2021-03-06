// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#ifndef NUMBER_DIALOG_H
#define NUMBER_DIALOG_H

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
class NumberDialog : public QDialog
{
    Q_OBJECT
public:
    NumberDialog(int start, int step, bool reset, const QList<int> &skip, QWidget *parent = nullptr);

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

private Q_SLOTS:

    void slotAddNumber();
    void slotRemoveNumber();

    void slotEnableControls();

private:
    Ui::NumberWidget    m_widget;
    QDialogButtonBox   *m_buttons;
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

#endif // NUMBER_DIALOG_H
