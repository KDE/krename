/***************************************************************************
                          krenamemodel.h  -  description
                             -------------------
    begin                : Sun Apr 25 2007
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

#ifndef KRENAMEMODEL_H
#define KRENAMEMODEL_H

#include "krenamefile.h"

#include <QAbstractListModel>

class KRenameModel : public QAbstractListModel {
 Q_OBJECT
 public:
   KRenameModel( KRenameFile::List* vector );
   ~KRenameModel();

   void addFile( const KRenameFile & file );

   virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
   virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

   Qt::ItemFlags flags(const QModelIndex &index) const;
   bool setData(const QModelIndex &index, const QVariant &value, int role);

   void append( const KRenameFile & file );
 
 private:
   KRenameFile::List* m_vector;
};

class KRenamePreviewModel : public QAbstractTableModel {
 Q_OBJECT
 public:
   KRenamePreviewModel( KRenameFile::List* vector );
   ~KRenamePreviewModel();

   void refresh();

   virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
   virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
   virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

   virtual QVariant headerData ( int section, Qt::Orientation orientation, int role ) const;
 private:
   KRenameFile::List* m_vector;
};

#endif // KRENAMEMODEL_H

