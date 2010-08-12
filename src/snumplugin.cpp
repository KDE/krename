/***************************************************************************
                         snumplugin.cpp  -  description
                             -------------------
    begin                : Tue Aug 05 2010
    copyright            : (C) 2010 by Matteo Azzali
    email                : matte.azzali@NOSPAMalice.it
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "snumplugin.h"

#include "batchrenamer.h"

#include <kiconloader.h>
#include <klistwidget.h>
#include <klocale.h>

#include <QLabel>
#include <QHBoxLayout>
#include <QRegExp>



SnumPlugin::SnumPlugin( PluginLoader* loader )
    : FilePlugin( loader )
{
    this->addSupportedToken("snum");
    this->addSupportedToken("season");
    this->addSupportedToken("episode");
    //this->addSupportedToken("season;.*");
    //this->addSupportedToken("episode;.*");

    m_help.append( "[snum];;" + i18n("Inserts the series number of original filename") );
    m_help.append( "[season];;" + i18n("Inserts the season number in two digits") );
    m_help.append( "[episode];;" + i18n("Inserts the episode number in two or three digits") );
    //m_help.append( "[season;pad];;" + i18n("Inserts the season number in at least pad digits") );
    //m_help.append( "[episode;pad];;" + i18n("Inserts the episode number in at least pad digits") );


    m_name = i18n("SeriesNumber");
    m_icon = "video-television";
    m_comment = i18n("<qt>This plugin can extract a filename "
		     "series number.</qt>");


}

SnumPlugin::~SnumPlugin()
{

}


QString SnumPlugin::processFile( BatchRenamer* b, int index, const QString & filenameOrToken, EPluginType eCurrentType )
{
    QString src;
    QString token;

    // This plugin supports to types
    if( eCurrentType == ePluginType_Token ) 
    {

        token = filenameOrToken.toLower();

        if( token == "snum" )
        {
            src = b->files()->at( index ).srcFilename();

            return this->extractnum( src ,0 );
        }
        else if( token == "season" )
        {
            src = b->files()->at( index ).srcFilename();

            return this->extractnum( src ,1 );
        }
        else if( token == "episode" )
        {
            src = b->files()->at( index ).srcFilename();

            return this->extractnum( src ,2 );
        }
    }

    return QString::null;
}


QString SnumPlugin::extractnum(const QString & unicoded, int a)
{
    int pos;
    QString tmp = "";
    QString seriesnum = "";
    QString season = "";
    QString episode = "";
    
    QRegExp rx( "(\\d{1,2})[^\\d]{1}(\\d{1,3})" );
    pos = 0;
    pos = rx.indexIn( unicoded, pos );
    if ( pos > -1 ) {
        season += rx.cap( 1 );
        episode += rx.cap( 2 );
        pos  += rx.matchedLength();
    }
    else
    {
        QRegExp px( "(\\d{1,2})[^\\d]{2}(\\d{1,3})" );
        pos = 0;
        pos = px.indexIn( unicoded, pos );
        if ( pos > -1 ) {
            season += px.cap( 1 );
            episode += px.cap( 2 );
            pos  += px.matchedLength();
        }
        else
        {
            QRegExp gx( "(\\d{1,2})[^\\d]{0}(\\d{2})" );
            pos = 0;
            pos = gx.indexIn( unicoded, pos );
            if ( pos > -1 ) {
                season += gx.cap( 1 );
                episode += gx.cap( 2 );
                pos  += gx.matchedLength();
            }
        }
    }
    
    
    if (season.length() == 1)
      tmp = '0' + season;
    else
        tmp = season;
    season=tmp;
	
    if (episode.length() == 1)
        tmp = '0' + episode;
    else
        tmp = episode;
    episode=tmp;
    
    seriesnum += season + 'e' + episode;
    
    if (a==0)
        return seriesnum;
    else if (a==1)
    {
        return season;
    }
    else
        return episode;
}

