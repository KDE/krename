// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#ifndef SELECTIONSAFELINEEDIT_H
#define SELECTIONSAFELINEEDIT_H

#include <QLineEdit>

/** A QLineEdit that does not loose its selection
 *  on focusout events
 */
class SelectionSafeLineEdit : public QLineEdit
{
public:
    /** Create a SeletionSafeLineEdit that keeps its selection
     *  even when the widget does not have the current keyboard focus.
     *
     *  @param parent parent widget
     */
    explicit SelectionSafeLineEdit(QWidget *parent = nullptr)
        : QLineEdit(parent)
    {

    }

protected:
    void focusOutEvent(QFocusEvent *) override
    {
        // ignore
    }

};

#endif // SELECTIONSAFELINEEDIT_H
