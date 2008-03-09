/***************************************************************************
                      strigiplugin.cpp  -  description
                             -------------------
    begin                : Sun Feb 24 2008
    copyright            : (C) 2008 by Dominik Seichter
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

#include "strigiplugin.h"

#include "batchrenamer.h"

#include <strigi/fieldproperties.h>
#include <strigi/fieldpropertiesdb.h>

#include <kiconloader.h>
#include <klocale.h>

using namespace Strigi;

StrigiPlugin::StrigiPlugin() 
    : FilePlugin(), m_analyzer( m_config )
{
    m_name = i18n("Strigi Plugin");
    m_icon = "strigi";

    FieldPropertiesDb db = FieldPropertiesDb::db();

    const std::map<std::string, FieldProperties> & allProperties =  db.allProperties();
    std::map<std::string, FieldProperties>::const_iterator it = allProperties.begin();

    while( it != allProperties.end() ) {
        QString name = (*it).second.name().c_str();
        QString desc = (*it).second.description().c_str();
        QString help = QString("[%1];;%2").arg( name.toLower() ).arg( desc );

        if( !name.isEmpty() && (*it).second.valid() ) 
        {
            this->addSupportedToken( name.toLower() );
            m_help.append( help );
        }

        ++it;
    }
}

QString StrigiPlugin::processFile( BatchRenamer* b, int index, const QString & filenameOrToken, EPluginType eCurrentType )
{
    QString token( filenameOrToken.toLower() );
    QString filename = (*b->files())[index].dstUrl().path();

    if( filename.isEmpty() )
        return QString::null;

    char result = m_analyzer.indexFile( filename.toUtf8().data() );
    qDebug("index result=%x", result );

    return QString::null;
}
