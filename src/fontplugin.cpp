/***************************************************************************
                      fontplugin.cpp  -  description
                             -------------------
    begin                : Sun Sep 26th 2010
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

#include "../config-krename.h"

#ifdef HAVE_FREETYPE
#include "batchrenamer.h"
#include "fontplugin.h"

#include <ft2build.h>
#include FT_FREETYPE_H
        
#include <klocale.h>

FontPlugin::FontPlugin( PluginLoader* loader )
    : FilePlugin( loader )
{
    const QString prefix("font");

    this->addSupportedToken("fontpostscript");
    this->addSupportedToken("fontfamily");
    this->addSupportedToken("fontstyle");
    m_help.append( "[fontPostscript]" + Plugin::S_TOKEN_SEPARATOR + i18n("Insert the Postscript name for Type1 and TrueType fonts.") );
    m_help.append( "[fontFamily]" + Plugin::S_TOKEN_SEPARATOR + i18n("Insert the (usually English) name of the font family.") );
    m_help.append( "[fontStyle]" + Plugin::S_TOKEN_SEPARATOR + i18n("Insert the (usually English) name of the font style.") );

    m_name = i18n("Font (FreeType2) Plugin");
    m_comment = i18n("<qt>This plugin supports reading tags from "
                     "font files.</qt>");

    m_icon = "application-x-font-ttf";

    FT_Error error = FT_Init_FreeType( &m_library );
    if( error ) 
    {
        qDebug("Freetype initialization error %i.", error );
        m_library = NULL;
    }
}

FontPlugin::~FontPlugin()
{
    FT_Done_FreeType( m_library );
    m_library = NULL;

}

QString FontPlugin::processFile( BatchRenamer* b, int index, const QString & filenameOrToken, EPluginType )
{
    QString token( filenameOrToken.toLower() );
    QString filename = (*b->files())[index].srcUrl().path();

    if( !this->supports( token ) )
        return QString("");

    if( !m_library ) 
    {
        return QString("Cannot initialize FreeType");
    }

    FT_Face face;
    FT_Error error = FT_New_Face( m_library,
                                  filename.toUtf8().data(),
                                  0,
                                  &face );
    QString result = QString("");

    if ( error == FT_Err_Unknown_File_Format )
    {
        face = NULL;
        result = QString("Unknown font file format error: %1").arg(error);
    }
    else if ( error )
    {
        face = NULL;
        result = QString("Unknown error: %1.").arg(error);
    }
    else
    {
        if( token == "fontpostscript" )
        {
            result = QString::fromAscii( FT_Get_Postscript_Name( face ) );
        }
        else if( token == "fontfamily" && face->family_name ) 
        {
            result = QString::fromAscii( face->family_name );
        }
        else if( token == "fontstyle" && face->style_name ) 
        {
            result = QString::fromAscii( face->style_name );
        }
    }

    if( face ) 
    {
        FT_Done_Face( face );
    }

    return result;
}

#endif // HAVE_FREETYPE
