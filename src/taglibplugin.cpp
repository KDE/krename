/***************************************************************************
                      taglibplugin.cpp  -  description
                             -------------------
    begin                : Tue Jul 1st 2008
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

#include "taglibplugin.h"

#include <taglib/taglib.h>
#include <taglib/fileref.h>
#include <taglib/tfile.h>
#include <taglib/tstring.h>
#include <taglib/tag.h>

#include <klocale.h>

#include "batchrenamer.h"

TagLibPlugin::TagLibPlugin()
    : FilePlugin()
{
    this->addSupportedToken("tagTitle");
    this->addSupportedToken("tagArtist");
    this->addSupportedToken("tagAlbum");
    this->addSupportedToken("tagComment");
    this->addSupportedToken("tagGenre");
    this->addSupportedToken("tagYear");
    this->addSupportedToken("tagTrack");

    m_help.append( "[tagTitle];;" + i18n("Insert the title of a track") );
    m_help.append( "[tagArtist];;" + i18n("Insert the artist of a track") );
    m_help.append( "[tagAlbum];;" + i18n("Insert the album of a track") );
    m_help.append( "[tagComment];;" + i18n("Insert the comment of a track") );
    m_help.append( "[tagGenre];;" + i18n("Insert the genre of a track") );
    m_help.append( "[tagYear];;" + i18n("Insert the year of a track") );
    m_help.append( "[tagTrack];;" + i18n("Insert the number of a track") );
    m_help.append( "[##tagTrack];;" + i18n("Insert the number of a track formatted with a leading 0") );

    m_name = i18n("TagLib (MP3/Ogg) Plugin");
    m_comment = i18n("<qt>This plugin supports reading tags for "
		     "MP3, Ogg Vorbis, FLAC, MPC, Speex WavPack "
		     "and TrueAudio files.</qt>");
    m_icon = "audio-x-generic";
}

QString TagLibPlugin::processFile( BatchRenamer* b, int index, const QString & filenameOrToken, EPluginType )
{
    QString token( filenameOrToken.toLower() );
    QString filename = (*b->files())[index].srcUrl().path();

    TagLib::FileRef g( TagLib::FileName(filename.toUtf8().data()) );
    TagLib::String result;

    if( g.isNull() )
	return QString("");

    if( token == "tagtitle" )
	result = g.tag()->title();
    else if( token == "tagartist" )
	result = g.tag()->artist();
    else if( token == "tagalbum" )
	result = g.tag()->album();
    else if( token == "tagcomment" )
	result = g.tag()->comment();
    else if( token == "taggenre" )
	result = g.tag()->genre();
    else if( token == "tagyear" ) {
	unsigned int year = g.tag()->year();
	return QString::number( year );
    } else if( token == "tagtrack" ) {
	unsigned int track = g.tag()->track();
	return QString::number( track );
    } 
 
    return TStringToQString( result );
}
