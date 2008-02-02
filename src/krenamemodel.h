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

/** This enum is used to specify a sorting mode
 */
enum ESortMode {
    eSortMode_Unsorted,
    eSortMode_Ascending,
    eSortMode_Descending,
    eSortMode_Numeric
};

class KRenameModel : public QAbstractListModel {
 Q_OBJECT
 public:
   KRenameModel( KRenameFile::List* vector );
   ~KRenameModel();

   /** Add a KRenameFile to the model
    *
    *  @param file a KRenameFile which is added to the list
    */
   void addFile( const KRenameFile & file );

   /** Remove items in the model
    *
    *  @param remove list of indexes to remove
    */
   void removeFiles( const QList<int> & remove );

   /** Sort the data in the model
    *  using the selected sort mode.
    *
    *  @param mode the sort mode to use
    */
   void sortFiles( ESortMode mode );

   /** Move each file in a list of indeces upwards
    *  @param files list of file indeces. Each file is moved up one position
    */
   void moveFilesUp( const QList<int> & files );

   /** Move each file in a list of indeces downwards
    *  @param files list of file indeces. Each file is moved down one position
    */
   void moveFilesDown( const QList<int> & files );

   /** Creates a new model index
    *
    *  @param row the index of the requested file
    *  @returns the model index for a certain row
    */
   const QModelIndex createIndex( int row ) const;

   /** Get the file at position index.
    *
    *  @param a valid index in the internal vector
    *
    *  @returns a KRenameFile object
    */
   inline const KRenameFile & file( int index ) const;

   /** Get the file at position index.
    *
    *  @param a valid index in the internal vector
    *
    *  @returns a KRenameFile object
    */
   inline KRenameFile & file( int index );



   virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
   virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

   Qt::ItemFlags flags(const QModelIndex &index) const;
   bool setData(const QModelIndex &index, const QVariant &value, int role);

   /** Enable the preview of KRenameFile objects.
    *
    *  @param preview enable/disable preview
    *  @param filenames if preview is true this items decides
    *                   if the filename text is displayed next to the preview
    */
   inline void setEnablePreview( bool preview, bool filenames );

   /** Run/open the file which is pointed to by the passed modelinex
    *
    *  @param index a modelindex specifying a file to open
    *  @param window a window used to cache authentication information
    */
   void run(const QModelIndex & index, QWidget* window ) const;

 signals:
   /** This signal is emitted when the maximum number of 
    *  dots in a filename that can be used to separate
    *  filename and extension has changed (by adding a new file).
    *
    *  @param dots the maximum number of dots in a filename
    */
   void maxDotsChanged( int dots );

 private:
   KRenameFile::List* m_vector;

   bool               m_preview;
   bool               m_text;

   int                m_maxDots; ///< The maximum number of dots in a filename which can be used to separate filename and extension
};

const KRenameFile & KRenameModel::file( int index ) const
{
    return m_vector->at(index);
}

KRenameFile & KRenameModel::file( int index )
{
    return (*m_vector)[index];
}

void KRenameModel::setEnablePreview( bool preview, bool filenames )
{
    bool update = false;
    if( m_preview != preview || filenames != m_text ) 
        update = true;

    m_preview = preview;
    m_text    = filenames;

    if( update )
        // TODO: update the model
        ;
}

class KRenamePreviewModel : public QAbstractTableModel {
 Q_OBJECT
 public:
   KRenamePreviewModel( KRenameFile::List* vector );
   ~KRenamePreviewModel();

   void refresh();

   virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
   virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
   virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

   virtual QModelIndex parent ( const QModelIndex & index ) const;
   virtual QModelIndex sibling ( int row, int column, const QModelIndex & index ) const;

   virtual QVariant headerData ( int section, Qt::Orientation orientation, int role ) const;
 private:
   KRenameFile::List* m_vector;
};

#endif // KRENAMEMODEL_H

