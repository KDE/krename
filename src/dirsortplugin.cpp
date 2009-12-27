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

        if( !KIO::NetAccess::exists( m_baseDirectory, true, m_widget->spinStart ) ) 
        {
            m_valid = false;
            return this->name() + 
                i18n(": The output directory %1 does not exist.", 
                     m_baseDirectory.prettyUrl() ); 
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

    KUrl srcUrl = b->buildDestinationUrl( (*b->files())[index] );
    KUrl dstUrl = m_currentDirectory;
    dstUrl.addPath( srcUrl.fileName() );
    KIO::JobFlags flags = KIO::DefaultFlags | KIO::HideProgressInfo;
    KIO::Job* job = KIO::file_move( srcUrl, dstUrl, -1, flags );
    m_fileCounter++;
    if( m_valid && job && !KIO::NetAccess::synchronousRun( job, m_widget->spinStart ) ) 
    {
        errorMsg = i18n("Error renaming %2 (to %1)", 
                        dstUrl.prettyUrl(), 
                        srcUrl.prettyUrl());
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

KUrl DirSortPlugin::createNewSubdirectory() const
{
    KUrl url = m_baseDirectory;

    QString dir;
    dir.sprintf("%0*i", m_digits, m_dirCounter );
    url.addPath( dir );

    if( !KIO::NetAccess::mkdir( url, m_widget->spinStart ) ) {
        KMessageBox::error( m_widget->spinStart, 
                            i18n("Cannot create directory %1", url.prettyUrl()) );
    }
    
    return url;    
}
