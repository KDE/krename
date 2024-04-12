// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#include "systemplugin.h"

#include "batchrenamer.h"

#include <QDate>
#include <QTime>

#include <kfileitem.h>
#include <KLocalizedString>
#include <KIO/StatJob>

SystemPlugin::SystemPlugin(PluginLoader *loader)
    : FilePlugin(loader)
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

    m_help.append(Plugin::createHelpEntry("date", i18n("Insert the current date")));
    m_help.append(Plugin::createHelpEntry("date;yyyy-MM-dd", i18n("Insert the current date using the formatting string yyyy-MM-dd")));
    m_help.append(Plugin::createHelpEntry("year", i18n("Insert the current year")));
    m_help.append(Plugin::createHelpEntry("month", i18n("Insert the current month as number")));
    m_help.append(Plugin::createHelpEntry("day", i18n("Insert the current day as number")));
    m_help.append(Plugin::createHelpEntry("time", i18n("Insert the current time")));
    m_help.append(Plugin::createHelpEntry("hour", i18n("Insert the current hour as number")));
    m_help.append(Plugin::createHelpEntry("minute", i18n("Insert the current minute as number")));
    m_help.append(Plugin::createHelpEntry("second", i18n("Insert the current second as number")));
    m_help.append(Plugin::createHelpEntry("user", i18n("Owner of the file")));
    m_help.append(Plugin::createHelpEntry("group", i18n("Owning group of the file")));
    m_help.append(Plugin::createHelpEntry("creationdate", i18n("Insert the files creation date")));
    m_help.append(Plugin::createHelpEntry("creationdate;yyyy-MM-dd", i18n("Insert the formatted file creation date")));
    m_help.append(Plugin::createHelpEntry("modificationdate", i18n("Insert the files modification date")));
    m_help.append(Plugin::createHelpEntry("modificationdate;yyyy-MM-dd", i18n("Insert the formatted modification date")));
    m_help.append(Plugin::createHelpEntry("accessdate", i18n("Insert the date of the last file access")));
    m_help.append(Plugin::createHelpEntry("accessdate;yyyy-MM-dd", i18n("Insert the formatted date of the last file access")));
    m_help.append(Plugin::createHelpEntry("filesize", i18n("Insert the file size in bytes")));

    m_name = i18n("Date and system functions");
    m_icon = "system-run";
    m_comment = i18n("<qt>This plugin contains tokens to get "
                     "the creation, modification and last access "
                     "time of files and the current system time and date.</qt>");
}

SystemPlugin::~SystemPlugin()
{

}

QString SystemPlugin::processFile(BatchRenamer *b, int index, const QString &filenameOrToken, EPluginType)
{
    /*
    if( token.lower().startsWith( getPattern() ) )
        token = token.mid( getPattern().length(), token.length() - getPattern().length() );
    */

    QString token(filenameOrToken);
    QDate d = QDate::currentDate();
    QTime t = QTime::currentTime();
    QString format = "dd-MM-yyyy";
    if (token.contains(";")) {
        format = token.section(';', 1, 1);
        token = token.section(';', 0, 0).toLower();
    } else {
        token = token.toLower();
    }

    if (token == "date") {
        return QDateTime::currentDateTime().toString(format);
    } else if (token == "year") {
        return QString("%1").arg(d.year());
    } else if (token == "month") {
        return QString::asprintf("%0*i", 2, d.month());
    } else if (token == "day") {
        return QString::asprintf("%0*i", 2, d.day());
    } else if (token == "time") {
        QString hour   = QString::asprintf("%0*i", 2, t.hour());
        QString minute = QString::asprintf("%0*i", 2, t.minute());
        QString second = QString::asprintf("%0*i", 2, t.second());
        return QString("%1-%2-%3").arg(hour, minute, second);
    } else if (token == "hour") {
        return QString::asprintf("%0*i", 2, t.hour());
    } else if (token == "minute") {
        return QString::asprintf("%0*i", 2, t.minute());
    } else if (token == "second") {
        return QString::asprintf("%0*i", 2, t.second());
    } else {
        const QUrl &url = b->files()->at(index).srcUrl();
        KIO::StatJob *statJob = KIO::statDetails(url, KIO::StatJob::DestinationSide, KIO::StatDefaultDetails);
        statJob->exec();
        if (statJob->error()) {
            return QString();
        }
        KFileItem item(statJob->statResult(), url);
        if (token == "user") {
            return item.user();
        } else if (token == "group") {
            return item.group();
        } else if (token == "creationdate")
            // TODO: Use toDateTime()
        {
            return time(item.time(KFileItem::ModificationTime).toSecsSinceEpoch(), format);
        } else if (token == "modificationdate") {
            return time(item.time(KFileItem::ModificationTime).toSecsSinceEpoch(), format);
        } else if (token == "accessdate") {
            return time(item.time(KFileItem::AccessTime).toSecsSinceEpoch(), format);
        } else if (token == "filesize") {
            return QString::number(item.size());
        }
    }

    return QString();
}

const QString SystemPlugin::time(time_t time, const QString &format)
{
    QDateTime dt;
    dt.setSecsSinceEpoch(time);
    return dt.toString(format);
}
