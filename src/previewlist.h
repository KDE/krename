/***************************************************************************
                       previewlist.h  -  description
                             -------------------
    begin                : Sat Oct 06 2007
    copyright            : (C) 2007 by Dominik Seichter
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

#ifndef _PREVIEW_LIST_H_
#define _PREVIEW_LIST_H_

#include <QTreeView>

class KRenameModel;
class KMenu;

class PreviewList : public QTreeView {
    Q_OBJECT

 public:
    PreviewList( QWidget* parent );
    ~PreviewList();

    inline void setKRenameModel( KRenameModel* model ) 
    {
        m_model = model;
    }

 signals:
    void addFiles();
    void updateCount();

 private slots:
     void slotOpen();
     void slotRemove();
     void slotManually();

 protected:
    void contextMenuEvent( QContextMenuEvent* e );

 private:
    KRenameModel* m_model;
    KMenu*        m_menu;
};

#endif // _PREVIEW_LIST_H_

