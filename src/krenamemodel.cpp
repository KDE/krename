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

#include <QMimeData>
#include <QPixmap>

#include <klocale.h>
#include <krandom.h>
#include <krun.h>

// Helper functions for sorting
const QString findNumInString( unsigned int pos, const QString & s )
{
    QString num;
    
    for( int i = static_cast<int>(pos); i >= 0; i-- )
        if( s[i].isDigit() )
            num.prepend( s[i] );
        else
            break;
            

    for( int i = pos + 1; i < s.length(); i++ )
        if( s[i].isDigit() )
            num.append( s[i] );
        else
            break;
    
    return num;
}

int compareNummeric( const QString & s1, const QString & s2 )
{
    int z = 0;
    int max = ( s1.length() > s2.length() ? s1.length() : s2.length() );
    
    QString num1;
    QString num2;
    for( z=0;z<max;z++)
    {
        //if( z >= s1.length() || z >= s2.length() )
        //    break;
            
        if( (z < s1.length() && z < s2.length() && s1[z] != s2[z])  )
        {
            if( z < s1.length() && s1[z].isDigit() )
                num1 = findNumInString( z, s1 );
            
            if( z < s2.length() && s2[z].isDigit() )
                num2 = findNumInString( z, s2 );
            
            if( num1.isNull() && num2.isNull() )    
                break;
                
            int a = num1.toInt();
            int b = num2.toInt();
            if( a == b )
                return s1.compare( s2 );
            else
                return ( a > b ) ? 1 : -1;
        }
    }
        
    return s1.compare( s2 );
}

// Less than functions for sorting
bool ascendingKRenameFileLessThan( const KRenameFile & file1, const KRenameFile & file2 ) 
{
    return file1.srcUrl() < file2.srcUrl();
}

bool descendingKRenameFileLessThan( const KRenameFile & file1, const KRenameFile & file2 ) 
{
    return !(file1.srcUrl() < file2.srcUrl());
}

bool numericKRenameFileLessThan( const KRenameFile & file1, const KRenameFile & file2 ) 
{
    KUrl url1 = file1.srcUrl();
    KUrl url2 = file2.srcUrl();
    if( url1.directory() != url2.directory() )
    {
        // not in the same directory so do lexical comparison
        return url1 < url2;
    }
    else
        return (compareNummeric( file1.srcFilename(), file2.srcFilename() ) < 0);

    return false;
}

bool randomKRenameFileLessThan( const KRenameFile &, const KRenameFile & ) 
{
    return static_cast<double>(KRandom::random()) / static_cast<double>(RAND_MAX) < 0.5;
}

KRenameModel::KRenameModel( KRenameFile::List* vector )
    : QAbstractListModel(),
      m_vector( vector ),
      m_preview( false ),
      m_text( false ),
      m_maxDots( 0 ),
      m_mimeType("text/uri-list")
{

}

KRenameModel::~KRenameModel()
{

}

int KRenameModel::rowCount ( const QModelIndex & index ) const
{
    if( !index.isValid() )
        return m_vector->size();

    return 0;
}

QVariant KRenameModel::data ( const QModelIndex & index, int role ) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_vector->size())
        return QVariant();

    if (role == Qt::DisplayRole && (!m_preview || (m_preview && m_text)) )
    {
        return m_vector->at(index.row()).toString();
    }
    else if( role == Qt::DecorationRole && m_preview ) 
    {
        return m_vector->at(index.row()).icon();
    }
    else
        return QVariant();
}

Qt::ItemFlags KRenameModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsDropEnabled;
    
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
}

Qt::DropActions KRenameModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

QStringList KRenameModel::mimeTypes() const
{
    QStringList types;
    types << m_mimeType;
    return types;
}

bool KRenameModel::dropMimeData(const QMimeData *data,
                                Qt::DropAction action,
                                int row, int,
                                const QModelIndex &)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat( m_mimeType ))
        return false;

    KRenameFile::List           dirs;
    KRenameFile::List           files;
    QList<QUrl>                 urls = data->urls();
    QList<QUrl>::const_iterator it   = urls.begin();

    while( it != urls.end() )
    {
        KRenameFile file( *it );

        if( file.isValid() && !file.isDir() )
            files.append( file );
        else if( file.isValid() && file.isDir() )
            // Add directories recursively
            dirs.append( file );

        ++it;
    }

    this->addFiles( files );
    // TODO: Wait cursor, add directories recursively.
    return true;
}

bool KRenameModel::setData(const QModelIndex &index,
                           const QVariant &, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
    
        //stringList.replace(index.row(), value.toString());
        emit dataChanged(index, index);
        return true;
    }
 
    return false;
}

void KRenameModel::addFiles( const KRenameFile::List & files )
{
    if( files.count() )
    {
        int oldMaxDots = m_maxDots;
        m_vector->reserve( m_vector->count() + files.count() );
        
        this->beginInsertRows( QModelIndex(), m_vector->size(), m_vector->size() + files.count() - 1 );

        KRenameFile::List::const_iterator it = files.begin();
        while( it != files.end() )
        {
            m_vector->push_back( *it );
            
            int dots  = (*it).dots();
            if( dots > m_maxDots ) 
                m_maxDots = dots;

            ++it;
        }
        this->endInsertRows();

        if( m_maxDots > oldMaxDots ) 
            emit maxDotsChanged( m_maxDots );
    }
}

void KRenameModel::removeFiles( const QList<int> & remove )
{
    int offset = 0;

    QList<int> copy( remove );
    qSort( copy );

    QList<int>::const_iterator it = copy.begin();
    this->beginRemoveRows( QModelIndex(), *it, copy.back() );
    while( it != copy.end() )
    {
        m_vector->erase( m_vector->begin() + *it - offset );

        ++offset;
        ++it;
    }

    this->endRemoveRows();
}

void KRenameModel::sortFiles( ESortMode mode )
{
    if( mode == eSortMode_Ascending ) 
        qSort( m_vector->begin(), m_vector->end(), ascendingKRenameFileLessThan );
    else if( mode == eSortMode_Descending ) 
        qSort( m_vector->begin(), m_vector->end(), descendingKRenameFileLessThan );
    else if( mode == eSortMode_Numeric ) 
        qSort( m_vector->begin(), m_vector->end(), numericKRenameFileLessThan );
    else if( mode == eSortMode_Random ) 
        qSort( m_vector->begin(), m_vector->end(), randomKRenameFileLessThan );
    else
        return;

    this->reset();
}

void KRenameModel::run(const QModelIndex & index, QWidget* window ) const
{
    KRenameFile file = m_vector->at(index.row());
    new KRun( file.srcUrl(), window );
}

const QModelIndex KRenameModel::createIndex( int row ) const
{
    return QAbstractItemModel::createIndex( row, 0 );
}

void KRenameModel::moveFilesUp( const QList<int> & files )
{
    int         index;
    KRenameFile tmp;

    QList<int> copy( files );
    qSort( copy );

    QList<int>::const_iterator it = copy.begin();
    while( it != copy.end() )
    {
        index                     = *it;
        if( index <= 0 ) // cannot swap top item
        {
            ++it;
            continue;
        }

        // swap items 
        tmp                    = m_vector->at( index );
        (*m_vector)[index]     = KRenameFile( m_vector->at( index - 1 ) );
        (*m_vector)[index - 1] = tmp;

        ++it;
    }

    this->reset();
}

void KRenameModel::moveFilesDown( const QList<int> & files )
{
    int         index;
    KRenameFile tmp;

    QList<int> copy( files );
    // sort the list in reverse order
    qSort( copy.begin(), copy.end(), qGreater<int>() );

    QList<int>::const_iterator it = copy.begin();
    while( it != copy.end() )
    {
        index                     = *it;
        if( index + 1 >= m_vector->size() ) // cannot swap bottom item
        {
            ++it;
            continue;
        }

        // swap items 
        tmp                    = m_vector->at( index );
        (*m_vector)[index]     = KRenameFile( m_vector->at( index + 1 ) );
        (*m_vector)[index + 1] = tmp;

        ++it;
    }

    this->reset();
}

//////////////////////////////////////////////////////////////
// Preview model starts below
//////////////////////////////////////////////////////////////
KRenamePreviewModel::KRenamePreviewModel( KRenameFile::List* vector )
    : m_vector( vector )
{

}

KRenamePreviewModel::~KRenamePreviewModel()
{

}

int KRenamePreviewModel::rowCount ( const QModelIndex & parent ) const
{
    if( !parent.isValid() )
       return m_vector->size();

    return 0;
}

int KRenamePreviewModel::columnCount ( const QModelIndex & ) const
{
    return 2;
}

QVariant KRenamePreviewModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if (orientation != Qt::Horizontal || section >= 2 || role != Qt::DisplayRole )
        return QVariant();

    return section ? i18n("Origin") : i18n("Renamed");
} 

QVariant KRenamePreviewModel::data ( const QModelIndex & index, int role ) const
{
    if (!index.isValid())
        return QVariant();
    
    if (index.row() >= m_vector->size())
        return QVariant();
    
    if (index.column() >= 2 )
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        const KRenameFile& file = m_vector->at(index.row());
        QString filename;
        QString extension;
        QString manual;

        if( index.column() )
        {
            manual    = file.manualChanges();
            if( manual.isNull() )
            {
                filename  = file.dstFilename();
                extension = file.dstExtension();
            }
            else
                filename = manual;
        }
        else
        {
            filename  = file.srcFilename();
            extension = file.srcExtension();
        }

        if( !extension.isEmpty() )
        {
            filename += ".";
            filename += extension;
        }

        if( file.isDirectory() )
        {
            filename = index.column() ? file.dstDirectory() : file.srcDirectory();
        }

        return filename;
    }
    else if( role == Qt::ForegroundRole )
    {
        const KRenameFile& file = m_vector->at(index.row());
        if( !file.manualChanges().isNull() )
            return QVariant( Qt::blue );
    }

    return QVariant();
    
}

QModelIndex KRenamePreviewModel::parent ( const QModelIndex & ) const
{
    return QModelIndex();
}

QModelIndex KRenamePreviewModel::sibling ( int, int, const QModelIndex & ) const
{
    return QModelIndex();
}

void KRenamePreviewModel::refresh() 
{
    emit reset();
}

#include "krenamemodel.moc"
