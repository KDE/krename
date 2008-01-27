/***************************************************************************
                          fileplugin.cpp  -  description
                             -------------------
    begin                : Mon Jul 1 2002
    copyright            : (C) 2002 by Dominik Seichter
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

#include "fileplugin.h"

#include <kapplication.h>
#include <kiconloader.h>
#include <klistwidget.h>
#include <klocale.h>
#include <kservice.h>

#include <QLabel>
#include <QHBoxLayout>

FilePlugin::FilePlugin( KService* service )
{
    m_name = service->name();
    m_icon = service->icon();

    qDebug("!!LOADED service: %s", m_name.toLatin1().data() );
}

FilePlugin::FilePlugin()
{
    m_name = "FilePlugin";
}

FilePlugin::~FilePlugin()
{

}

QString FilePlugin::processFile( BatchRenamer* b, int index, const QString & filenameOrToken, EPluginType eCurrentType )
{

    return QString::null;
}

bool FilePlugin::supports( const QString & token )
{
    QString lower = token.toLower();

    for( int i = 0; i < m_keys.count(); i++ )
        // TODO: Maybe we can optimize by putting all tokens
        //       already converted to lowercase into m_keys
        if( QRegExp( m_keys[i].toLower() ).exactMatch( lower ) )
            return true;
            
    return false;
}

const QPixmap FilePlugin::icon() const
{
    return KIconLoader::global()->loadIcon( m_icon, K3Icon::Small );
}

void FilePlugin::createUI( QWidget* parent ) const
{
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding );
    
    QVBoxLayout* l    = new QVBoxLayout( parent );
    QHBoxLayout* hbox = new QHBoxLayout( parent );
    
    QLabel* pix = new QLabel( parent );
    pix->setPixmap( KIconLoader::global()->loadIcon( m_icon, K3Icon::Desktop ) );
    
    hbox->addWidget( pix );
    hbox->addWidget( new QLabel( "<qt><b>"+name()+"</b></qt>", parent  ) );
    hbox->addItem( spacer );

    l->addLayout( hbox );    
    l->addWidget( new QLabel( "KOMMENTAR", parent  ) );
    l->addWidget( new QLabel( i18n("Supported tokens:"), parent  ) );

    KListWidget* list = new KListWidget( parent  );
    //list->setColumnMode( KListBox::FitToWidth );
    
    const QStringList & keys = supportedTokens();

    for( unsigned int i = 0; i < keys.count(); i++ )
        list->insertItem( 0, "[" + keys[i] + "]" );
    
    l->addWidget( list );
    l->setStretchFactor( list, 2 );
}

