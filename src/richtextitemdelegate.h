/***************************************************************************
                     richtextitemdelegate.h  -  description
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

#ifndef RICHTEXT_ITEM_DELEGTATE_H_
#define RICHTEXT_ITEM_DELEGTATE_H_

#include <QItemDelegate>

class QTextDocument;

class RichTextItemDelegate : public QItemDelegate {

public:
    RichTextItemDelegate(QObject* parent = NULL);
    virtual ~RichTextItemDelegate() { }

    virtual void paint (QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const;


private:
    QTextDocument* m_document;

};

#endif // RICHTEXT_ITEM_DELEGTATE_H_
