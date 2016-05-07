/***************************************************************************
                       systemplugin.cpp  -  description
                             -------------------
    begin                : Thu Oct 11 2007
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

#include "systemplugin.h"

#include "batchrenamer.h"

#include <QDate>
#include <QTime>

#include <kfileitem.h>
#include <kio/netaccess.h>
#include <klocale.h>

SystemPlugin::SystemPlugin(  PluginLoader* loader )
    : FilePlugin( loader )
{
    this->addSupportedToken("date");
    this->addSupportedToken("date;.*");
    this->addSupportedToken("year");
    this->addSupportedToken("month");
    this->addSupportedToken("day");
    this->addSupportedToken("time");
    this->addSupportedToken("hour");
    this->addSupportedToken("minute");
    this->addSupportedToken("second");
    this->addSupportedToken("user");
    this->addSupportedToken("group");
    this->addSupportedToken("creationdate");
    this->addSupportedToken("creationdate;.*");
    this->addSupportedToken("modificationdate");
    this->addSupportedToken("modificationdate;.*");
    this->addSupportedToken("accessdate");
    this->addSupportedToken("accessdate;.*");
    this->addSupportedToken("filesize");
 
    m_help.append( Plugin::createHelpEntry("date", i18n("Insert the current date")) );
    m_help.append( Plugin::createHelpEntry("date;yyyy-MM-dd", i18n("Insert the current date using the formatting string yyyy-MM-dd")) );
    m_help.append( Plugin::createHelpEntry("year", i18n("Insert the current year")) );
    m_help.append( Plugin::createHelpEntry("month", i18n("Insert the current month as number")) );
    m_help.append( Plugin::createHelpEntry("day", i18n("Insert the current day as number")) );
    m_help.append( Plugin::createHelpEntry("time", i18n("Insert the current time")) );
    m_help.append( Plugin::createHelpEntry("hour", i18n("Insert the current hour as number")) );
    m_help.append( Plugin::createHelpEntry("minute", i18n("Insert the current minute as number")) );
    m_help.append( Plugin::createHelpEntry("second", i18n("Insert the current second as number")) );
    m_help.append( Plugin::createHelpEntry("user", i18n("Owner of the file")) );
    m_help.append( Plugin::createHelpEntry("group", i18n("Owning group of the file")) );
    m_help.append( Plugin::createHelpEntry("creationdate", i18n("Insert the files creation date")));
    m_help.append( Plugin::createHelpEntry("creationdate;yyyy-MM-dd", i18n("Insert the formatted file creation date")) );
    m_help.append( Plugin::createHelpEntry("modificationdate", i18n("Insert the files modification date")) );
    m_help.append( Plugin::createHelpEntry("modificationdate;yyyy-MM-dd", i18n("Insert the formatted modification date")) );
    m_help.append( Plugin::createHelpEntry("accessdate", i18n("Insert the date of the last file access")) );
    m_help.append( Plugin::createHelpEntry("accessdate;yyyy-MM-dd", i18n("Insert the formatted date of the last file access")) );
    m_help.append( Plugin::createHelpEntry("filesize", i18n("Insert the file size in bytes")) );

    m_name = i18n("Date and system functions");
    m_icon = "system-run";   
    m_comment = i18n("<qt>This plugin contains tokens to get "
		     "the creation, modification and last access "
		     "time of files and the current system time and date.</qt>");
}

SystemPlugin::~SystemPlugin()
{

}

QString SystemPlugin::processFile( BatchRenamer* b, int index, const QString & filenameOrToken, EPluginType )
{
    /*
    if( token.lower().startsWith( getPattern() ) )
        token = token.mid( getPattern().length(), token.length() - getPattern().length() );
    */
    
    QString token( filenameOrToken );
    QDate d = QDate::currentDate();
    QTime t = QTime::currentTime();
    QString tmp, text;
    QString format = "dd-MM-yyyy";
    if( token.contains( ";" ) )
    {
        format = token.section( ';', 1, 1 );
        token = token.section( ';', 0, 0 ).toLower();
    } else 
        token = token.toLower();

    if( token == "date" ) {
        return QDateTime::currentDateTime().toString( format );
    } else if( token == "year" )
        return QString( "%1" ).arg( d.year() );
    else if( token == "month" )
        return tmp.sprintf("%0*i", 2, d.month() );
    else if( token == "day" )
        return tmp.sprintf("%0*i", 2, d.day() );
    else if( token == "time" ) {
	QString hour   = QString().sprintf("%0*i", 2, t.hour() );
	QString minute = QString().sprintf("%0*i", 2, t.minute() );
	QString second = QString().sprintf("%0*i", 2, t.second() );
	return QString( "%1-%2-%3" ).arg( hour ).arg( minute ).arg( second );
    } else if( token == "hour" )
        return tmp.sprintf("%0*i", 2, t.hour() );
    else if( token == "minute" )
        return tmp.sprintf("%0*i", 2, t.minute() );
    else if( token == "second" )
        return tmp.sprintf("%0*i", 2, t.second() );
    else {
        const QUrl &url = b->files()->at( index ).srcUrl();
        KIO::UDSEntry entry;
        KIO::NetAccess::stat( url, entry, NULL );
        KFileItem item( entry, url );
        if( token == "user" )
            return item.user();
        else if( token == "group" )
            return item.group();
        else if( token == "creationdate" )
            // TODO: Use toDateTime()
            return time( item.time( KFileItem::ModificationTime ).toTime_t(), format );
        else if( token == "modificationdate" )
            return time( item.time( KFileItem::ModificationTime ).toTime_t(), format );
        else if( token == "accessdate" )
            return time( item.time( KFileItem::AccessTime ).toTime_t(), format );
        else if( token == "filesize" )
            return QString::number( item.size() );
    }
    
    return QString::null;    
}

const QString SystemPlugin::time( time_t time, const QString & format )
{
    QDateTime dt;
    dt.setTime_t( time );
    return dt.toString( format );
}
