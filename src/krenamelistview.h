// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2009 Dominik Seichter <domseichter@web.de>

#ifndef KRENAME_LIST_VIEW_H
#define KRENAME_LIST_VIEW_H

#include <QListView>

/**
 * This is a QListView implementation
 * that shows additional information
 * if there are no items in the list
 * view.
 */
class KRenameListView : public QListView
{
    Q_OBJECT

public:
    explicit KRenameListView(QWidget *parent);
    ~KRenameListView() override { }

    /**
     * Specify the info widget to display
     * if the listview contains no items.
     *
     * The widget will be owned by the KRenameListView.
     *
     * @param w use widget as info widget
     */
    inline void setInfoWidget(QWidget *w);

public Q_SLOTS:
    void slotUpdateCount();

protected:
    void resizeEvent(QResizeEvent *e) override;

private:
    /**
     * Position the information widget
     * and hide/show if necessary.
     */
    void positionLabel();

private:
    QWidget *m_label; ///< The information widget
};

inline void KRenameListView::setInfoWidget(QWidget *w)
{
    m_label = w;
    m_label->setParent(this);
    positionLabel();
}

#endif // KRENAME_LIST_VIEW_H
