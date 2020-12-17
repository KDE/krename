// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2009 Dominik Seichter <domseichter@web.de>

#ifndef RICHTEXT_ITEM_DELEGATE_H
#define RICHTEXT_ITEM_DELEGATE_H

#include <QItemDelegate>

class QTextDocument;

class RichTextItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit RichTextItemDelegate(QObject *parent = nullptr);
    ~RichTextItemDelegate() override { }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QTextDocument *m_document;

};

#endif // RICHTEXT_ITEM_DELEGATE_H
