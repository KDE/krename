/***************************************************************************
                     richtextitemdelegate.cpp  -  description
                             -------------------
    begin                : Mon Oct 12 2009
    copyright            : (C) 2009 by Dominik Seichter
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

#include "richtextitemdelegate.h"

#include <QPainter>
#include <QRectF>
#include <QTextDocument>

RichTextItemDelegate::RichTextItemDelegate(QObject* parent)
    : QItemDelegate(parent)
{
    m_document = new QTextDocument(this);

}

void RichTextItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    // prepare
    painter->save();
    painter->setClipRect(option.rect);
    this->drawBackground(painter, option, index);

    if(option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    //QItemDelegate::paint(painter, option, index);
    QPixmap pixmap = index.model()->data(index, Qt::DecorationRole).value<QPixmap>();
    QString text = index.model()->data(index, Qt::DisplayRole).toString();

    if(!pixmap.isNull()) 
    {
        const int DEFAULT_OFFSET = 80;
        int offsetX = qMax(pixmap.width(), DEFAULT_OFFSET);
        QRect pixmapRect(option.rect.x(),
                         option.rect.y(),
                         pixmap.width(),
                         option.rect.height());

        QRectF textRect( 0.0, 0.0,
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
    }
    else
    {
        painter->setFont(option.font);
        painter->drawText(option.rect,
                          option.displayAlignment,
                          text);
    }

    this->drawFocus(painter, option, option.rect);
    
    // done
    painter->restore();
}

QSize RichTextItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    return QItemDelegate::sizeHint(option, index);
}

