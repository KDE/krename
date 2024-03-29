// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2009 Dominik Seichter <domseichter@web.de>

#include "richtextitemdelegate.h"
#include "krenamefile.h"
#include "krenamemodel.h"

#include <QPainter>
#include <QRectF>
#include <QTextDocument>

RichTextItemDelegate::RichTextItemDelegate(QObject *parent)
    : QItemDelegate(parent)
{
    m_document = new QTextDocument(this);

}

void RichTextItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // prepare
    painter->save();
    painter->setClipRect(option.rect);
    this->drawBackground(painter, option, index);

    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    //QItemDelegate::paint(painter, option, index);
    QPixmap pixmap = index.model()->data(index, Qt::DecorationRole).value<QPixmap>();
    QString text = index.model()->data(index, Qt::DisplayRole).toString();

    if (!pixmap.isNull()) {
        int offsetX = qMax(pixmap.width(), KRenameFile::iconSize());
        QRect pixmapRect(option.rect.x(),
                         option.rect.y(),
                         pixmap.width(),
                         option.rect.height());

        QRectF textRect(0.0, 0.0,
                        static_cast<qreal>(option.rect.width() - offsetX),
                        static_cast<qreal>(option.rect.height()));

        this->drawDecoration(painter, option,
                             pixmapRect,
                             pixmap);

        painter->save();
        painter->translate(option.rect.x() + offsetX,
                           option.rect.y());
        m_document->setHtml(text);
        m_document->drawContents(painter, textRect);
        painter->restore();
    } else {
        painter->setFont(option.font);
        painter->drawText(option.rect,
                          option.displayAlignment,
                          text);
    }

    this->drawFocus(painter, option, option.rect);

    // done
    painter->restore();
}

QSize RichTextItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (static_cast<const KRenameModel *>(index.model())->isPreviewEnabled()) {
        return QSize(KRenameFile::iconSize(), KRenameFile::iconSize());
    } else {
        return QItemDelegate::sizeHint(option, index);
    }
}

#include "moc_richtextitemdelegate.cpp"
