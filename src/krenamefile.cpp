/***************************************************************************
                          krenamefile.cpp  -  description
                             -------------------
    begin                : Wed Apr 18 2007
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

#include "krenamefile.h"

KRenameFile::KRenameFile( KUrl src )
{
    initFileDescription( m_src, src );
}

KRenameFile::KRenameFile( const KRenameFile & rhs )
{
    this->operator=( rhs );
}

const KRenameFile & KRenameFile::operator=( const KRenameFile & rhs ) 
{
    m_bDirectory = rhs.m_bDirectory;
    m_src        = rhs.m_src;
    m_dst        = rhs.m_dst;
}

void KRenameFile::initFileDescription( TFileDescription & rDescription, const KUrl & url ) const
{
    rDescription.url = url;
}


