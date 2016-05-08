/***************************************************************************
                          dirsortplugin.cpp  -  description
                             -------------------
    begin                : Sun Dec 27 2009
    copyright            : (C) 2009 by Dominik Seichter
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

// Own includes
#include "dirsortplugin.h"

#include "batchrenamer.h"

// KDE includes
#include <kiconloader.h>
#include <kio/job.h>
#include <kio/netaccess.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <KJobWidgets>

DirSortPlugin::DirSortPlugin( PluginLoader* loader ) 
    : Plugin( loader )
{
    m_widget = new Ui::DirSortPluginWidget();    
}

DirSortPlugin::~DirSortPlugin() 
{
    delete m_widget;
}

const QString DirSortPlugin::name() const
{
    return i18n("Subdirectory-Sort Plugin");
}

int DirSortPlugin::type() const
{
    return ePluginType_File;
}

bool DirSortPlugin::alwaysEnabled() const
{
    return false;
}

const QPixmap DirSortPlugin::icon() const
{
    return KIconLoader::global()->loadIcon( "folder", KIconLoader::NoGroup, KIconLoader::SizeSmall );
}

void DirSortPlugin::createUI( QWidget* parent ) const
{
    m_widget->setupUi( parent );

    m_widget->outputUrl->setMode( KFile::Directory | KFile::ExistingOnly );
}

/*
void DirSortPlugin::fillStructure()
{
    fpd = spinFiles->value();
    fpd--;
    dir = outputdir->text();

    filecounter = 0;
    dircounter = spinStart->value();
    curdir = dir +  QString("/%1/").arg( dircounter );
    d = new QDir( dir );
    d->mkdir( curdir );
}
*/

QString DirSortPlugin::processFile( BatchRenamer* b, int index, const QString &, EPluginType )
{
    QString errorMsg = QString::null;

    if( index == 0 )
    {
        // Initialize plugin
        m_dirCounter = m_widget->spinStart->value();
        m_fileCounter = 0;
        m_filesPerDir = m_widget->spinFiles->value();
        m_digits = m_widget->spinDigits->value();
        m_baseDirectory = m_widget->outputUrl->url();

        if( !KIO::NetAccess::exists( m_baseDirectory, KIO::NetAccess::DestinationSide, m_widget->spinStart ) )
        {
            m_valid = false;
            return this->name() + 
                i18n(": The output directory %1 does not exist.", 
                     m_baseDirectory.toDisplayString(QUrl::PreferLocalFile) );
        }
        else 
        {
            m_valid = true;

            m_currentDirectory = createNewSubdirectory();
        }
    }
    
    if( !m_valid ) 
        return errorMsg;

    if( m_fileCounter == m_filesPerDir ) 
    {
        m_fileCounter = 0;
        m_dirCounter++;

        m_currentDirectory = createNewSubdirectory();
    }

    QUrl srcUrl = b->buildDestinationUrl( (*b->files())[index] );
    QUrl dstUrl = m_currentDirectory;
    dstUrl = dstUrl.adjusted(QUrl::StripTrailingSlash);
    dstUrl.setPath(dstUrl.path() + '/' + ( srcUrl.fileName() ));
    KIO::JobFlags flags = KIO::DefaultFlags | KIO::HideProgressInfo;
    KIO::Job* job = KIO::file_move( srcUrl, dstUrl, -1, flags );
    m_fileCounter++;
    if( m_valid && job && !KIO::NetAccess::synchronousRun( job, m_widget->spinStart ) ) 
    {
        errorMsg = i18n("Error renaming %2 (to %1)", 
                        dstUrl.toDisplayString(QUrl::PreferLocalFile),
                        srcUrl.toDisplayString(QUrl::PreferLocalFile));
    } 

    return errorMsg;
}

const QStringList & DirSortPlugin::supportedTokens() const
{
    return m_emptyList;
}

const QStringList & DirSortPlugin::help() const
{
    return m_emptyList;
}

QUrl DirSortPlugin::createNewSubdirectory() const
{
    QUrl url = m_baseDirectory;

    QString dir;
    dir.sprintf("%0*i", m_digits, m_dirCounter );
    url = url.adjusted(QUrl::StripTrailingSlash);
    url.setPath(url.path() + '/' + ( dir ));

    KIO::MkdirJob *job = KIO::mkdir(url);
    KJobWidgets::setWindow(job, m_widget->groupBox); // we just need a random widget, FIXME use the proper parent

    if( !job->exec() ) {
        KMessageBox::error( m_widget->groupBox,
                            i18n("Cannot create directory %1", url.toDisplayString(QUrl::PreferLocalFile)) );
    }
    
    return url;    
}
