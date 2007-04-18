/***************************************************************************
                         krenamemodel.cpp  -  description
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

#include "krenamemodel.h"

KRenameModel::KRenameModel( KRenameFile::List* vector )
    : QAbstractListModel(),
      m_vector( vector )
{

}

KRenameModel::~KRenameModel()
{

}

int KRenameModel::rowCount ( const QModelIndex & parent ) const
{
    return m_vector->size();
}

QVariant KRenameModel::data ( const QModelIndex & index, int role ) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_vector->size())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        return m_vector->at(index.row()).toString();
    }
    else
        return QVariant();
}

Qt::ItemFlags KRenameModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool KRenameModel::setData(const QModelIndex &index,
                               const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
    
        //stringList.replace(index.row(), value.toString());
        emit dataChanged(index, index);
        return true;
    }
 
    return false;
}

#include "krenamemodel.moc"
