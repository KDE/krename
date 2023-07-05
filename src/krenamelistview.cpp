// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2009 Dominik Seichter <domseichter@web.de>

#include "krenamelistview.h"

KRenameListView::KRenameListView(QWidget *parent)
    : QListView(parent),
      m_label(nullptr)
{
}

void KRenameListView::resizeEvent(QResizeEvent *e)
{
    QListView::resizeEvent(e);
    positionLabel();
}

void KRenameListView::slotUpdateCount()
{
    this->positionLabel();
}

void KRenameListView::positionLabel()
{
    if (m_label == nullptr) {
        return;
    }

    if (!this->model() || this->model()->rowCount()) {
        m_label->hide();
    } else {
        int x = (width() - m_label->minimumSizeHint().width()) / 2;
        int y = (height() - m_label->minimumSizeHint().height()) / 2;
        m_label->setGeometry(x, y,
                             m_label->minimumSizeHint().width(), m_label->minimumSizeHint().height());
        m_label->show();
    }
}

#include "moc_krenamelistview.cpp"
