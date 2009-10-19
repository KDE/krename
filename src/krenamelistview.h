/***************************************************************************
                     krenamelistview.h  -  description
                             -------------------
    begin                : Tue Oct 13 2009
    copyright            : (C) 2009 b Dominik Seichter
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


#ifndef KRENAME_LIST_VIEW_H_
#define KRENAME_LIST_VIEW_H_

#include <QListView>

class QPaintEvent;
class QTextDocument;

/**
 * This is a QListView implementation
 * that shows additional information
 * if there are no items in the list
 * view.
 */
class KRenameListView : public QListView {
    Q_OBJECT

public:
    KRenameListView(QWidget* parent);
    virtual ~KRenameListView() { }

    /**
     * Specify the info widget to display
     * if the listview contains no items.
     *
     * The widget will be owned by the KRenameListView.
     *
     * @param w use widget as info widget
     */
    inline void setInfoWidget(QWidget* w);

public slots:
    void slotUpdateCount();

protected:
    virtual void resizeEvent(QResizeEvent* e);

private:
    /**
     * Position the information widget
     * and hide/show if necessary.
     */
    void positionLabel();

private:
    QWidget* m_label; ///< The information widget
};

inline void KRenameListView::setInfoWidget(QWidget* w)
{
    m_label = w;
    m_label->setParent(this);
    positionLabel();
}

#endif // KRENAME_LIST_VIEW_H_
