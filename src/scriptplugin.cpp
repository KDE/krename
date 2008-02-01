/***************************************************************************
                   scriptplugin.cpp  -  description
                             -------------------
    begin                : Fri Nov 9 2007
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

#include "scriptplugin.h"

#include <kapplication.h>
#include <kiconloader.h>
#include <klistwidget.h>
#include <klocale.h>

#include <QLabel>
#include <QHBoxLayout>

/*
#include <kjs/completion.h>
#include <kjs/interpreter.h>
#include <kjs/value.h>
*/
ScriptPlugin::ScriptPlugin()
{
    m_name = "ScriptPlugin";
    /*
    m_interpreter = new KJS::Interpreter();

    QString script = "alert(\"Hello\"); 4 + 6;";

    KJS::Completion comp = m_interpreter->evaluate( KJS::UString(), 0, KJS::UString( script.toUtf8().data() ) );
    if( comp.complType() == KJS::ReturnValue )
    {
        qDebug( comp.value()->toString( m_interpreter->globalExec() ).cstring().c_str() ); 
    }
    else
        qDebug("compType=%i\n", comp.complType() ); 
    */
}

ScriptPlugin::~ScriptPlugin()
{

}

QString ScriptPlugin::processFile( BatchRenamer* b, int index, const QString & filenameOrToken, EPluginType eCurrentType )
{

    return QString::null;
}

bool ScriptPlugin::supports( const QString & token )
{
    QString lower = token.toLower();

    for( int i = 0; i < m_keys.count(); i++ )
        // TODO: Maybe we can optimize by putting all tokens
        //       already converted to lowercase into m_keys
        if( QRegExp( m_keys[i].toLower() ).exactMatch( lower ) )
            return true;
            
    return false;
}

const QPixmap ScriptPlugin::icon() const
{
    return KIconLoader::global()->loadIcon( m_icon, KIconLoader::NoGroup, KIconLoader::SizeSmall );
}

void ScriptPlugin::createUI( QWidget* parent ) const
{
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding );
    
    QVBoxLayout* l    = new QVBoxLayout( parent );
    QHBoxLayout* hbox = new QHBoxLayout( parent );
    
    QLabel* pix = new QLabel( parent );
    pix->setPixmap( KIconLoader::global()->loadIcon( m_icon, KIconLoader::Desktop ) );
    
    hbox->addWidget( pix );
    hbox->addWidget( new QLabel( "<qt><b>"+name()+"</b></qt>", parent  ) );
    hbox->addItem( spacer );

    l->addLayout( hbox );    
    l->addWidget( new QLabel( i18n("This plugin can execute custom scripts."), parent  ) );
    l->addWidget( new QLabel( i18n("Supported tokens:"), parent  ) );

    KListWidget* list = new KListWidget( parent  );
    //list->setColumnMode( KListBox::FitToWidth );
    
    const QStringList & keys = supportedTokens();

    for( unsigned int i = 0; i < keys.count(); i++ )
        list->insertItem( 0, "[" + keys[i] + "]" );
    
    l->addWidget( list );
    l->setStretchFactor( list, 2 );
}

