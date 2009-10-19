/***************************************************************************
                     krenamelistview.cpp  -  description
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

#include "krenamelistview.h"

#include <kiconloader.h>

#include <QPainter>
#include <QPaintEvent>
#include <QTextDocument>

KRenameListView::KRenameListView(QWidget* parent)
    : QListView(parent),
      m_label(NULL)
{
    /*
    QString krename = KIconLoader::global()->iconPath("krename", KIconLoader::Desktop);

    const QString message =
        "<qt><img src=\"" + krename + "\" /><h1>Welcome to KRename</h1>"
        "<p>KRename is a batch renamer (mass file renamer) for KDE.</p>"
        "<p><ul><li>Please add some files</li></ul>" 
        "</qt>";

    m_document = new QTextDocument(this);
    m_document->setHtml(message);
    */
}

/*
void KRenameListView::paintEvent(QPaintEvent* event)
{
    if(this->model()->rowCount() == 0) 
    {
        int width = this->width() / 2;
        int height = this->height() / 2;
        QRect rect((this->width() - width)/2,
                   (this->height() - height)/2,
                   width,
                   height);

        QPainter painter(this->viewport());
        painter.save();
        painter.setBrush(this->palette().window());
        painter.drawRoundedRect(rect, 15.0, 15.0);
        painter.restore();

        painter.save();
        const int offset = 10;
        painter.translate(rect.x() + offset,
                          rect.y() + offset);
        m_document->drawContents(&painter, 
                                 QRect(0, 0,
                                       width - offset * 2, 
                                       height - offset * 2)); 
        painter.restore();

        event->accept();
    }
    else
    {
        QListView::paintEvent(event);
    }

}
*/

void KRenameListView::resizeEvent(QResizeEvent* e)
{
    QListView::resizeEvent( e );
    positionLabel();
}

void KRenameListView::slotUpdateCount()
{
    this->positionLabel();
}

void KRenameListView::positionLabel()
{
    if( m_label == NULL ) 
    {
        return;
    }

    if( !this->model() || this->model()->rowCount() )
    {
        m_label->hide();
    }
    else
    {
        int x = (width() - m_label->minimumSizeHint().width()) / 2;
        int y = (height() - m_label->minimumSizeHint().height()) / 2;
        m_label->setGeometry( x, y, 
                              m_label->minimumSizeHint().width(), m_label->minimumSizeHint().height() );
        m_label->show();
    }
}
