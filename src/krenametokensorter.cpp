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

#include "krenametokensorter.h"

#include "plugin.h"
#include "pluginloader.h"

#include <krandom.h>

// Helper functions for sorting
static const QString findNumInString( unsigned int pos, const QString & s )
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

static int compareNummeric( const QString & s1, const QString & s2 )
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


KRenameTokenSorter::KRenameTokenSorter(BatchRenamer* renamer, const QString & token, 
                                       const KRenameFile::List & list, ESimpleSortMode eSortMode)
    : m_renamer(renamer), m_token(token), m_list(list), m_eSortMode(eSortMode)
{
    m_plugin = PluginLoader::Instance()->findPlugin( token );
    
    KRenameFile::List::ConstIterator it = list.begin();
    int index = 0;
    while( it != list.end() )
    {
        QString value = processString( index++ );
        m_values.insert( (*it).srcUrl(), value );
        
        ++it;
    }
}

bool KRenameTokenSorter::operator()( const KRenameFile & file1, const KRenameFile & file2 )
{
    QString str1, str2;
    
    str1 = m_values.value( file1.srcUrl() );
    str2 = m_values.value( file2.srcUrl() );
    
    if( m_eSortMode == eSimpleSortMode_Ascending )
    {
        return str1 < str2;
    }
    else if( m_eSortMode == eSimpleSortMode_Descending )
    {
        return !(str1 < str2);
    }
    else if( m_eSortMode == eSimpleSortMode_Numeric ) 
    {
        return compareNummeric( str1, str2 ) < 0;
    }

    // Default, should never be reached
    return (str1 < str2);
}

QString KRenameTokenSorter::processString( int index ) const
{
    QString ret = m_token;
    if( m_plugin != NULL ) 
    {
        ret = m_plugin->processFile( m_renamer, index, ret, ePluginType_Token );
    }
    return ret;
}

