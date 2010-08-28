/***************************************************************************
                       krenametokensorter.h  -  description
                             -------------------
    begin                : Sat Aug 28 2010
    copyright            : (C) 2010 by Dominik Seichter
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

#ifndef KRENAMETOKENSORTER_H
#define KRENAMETOKENSORTER_H

#include "krenamefile.h"

#include <QMap>
#include <QString>

#include <kurl.h>

class BatchRenamer;
class Plugin;

bool ascendingKRenameFileLessThan( const KRenameFile & file1, const KRenameFile & file2 );
bool descendingKRenameFileLessThan( const KRenameFile & file1, const KRenameFile & file2 ); 
bool numericKRenameFileLessThan( const KRenameFile & file1, const KRenameFile & file2 );
bool randomKRenameFileLessThan( const KRenameFile &, const KRenameFile & );

class KRenameTokenSorter {
public:
    
    enum ESimpleSortMode {
        eSimpleSortMode_Ascending,
        eSimpleSortMode_Descending,
        eSimpleSortMode_Numeric
    };

    KRenameTokenSorter(BatchRenamer* renamer, const QString & token, const KRenameFile::List & list, ESimpleSortMode eSortMode);

    bool operator()( const KRenameFile & file1, const KRenameFile & file2 );

private:
    QString processString( int index ) const;

private:
    BatchRenamer* m_renamer;
    const QString & m_token;
    const KRenameFile::List & m_list;
    ESimpleSortMode m_eSortMode; 
    Plugin* m_plugin;
    QMap<KUrl,QString> m_values;
};

#endif // KRENAMETOKENSORTER_H
