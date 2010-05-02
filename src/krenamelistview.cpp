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
}

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
