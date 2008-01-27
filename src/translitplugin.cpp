/***************************************************************************
                         translitplugin.cpp  -  description
                             -------------------
    begin                : Sat Oct 13 2007
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

#include "translitplugin.h"

#include "batchrenamer.h"

#include <kiconloader.h>
#include <klistwidget.h>
#include <klocale.h>

#include <QLabel>
#include <QHBoxLayout>

const QString TranslitPlugin::s_strUtf8[] = {"а","б","в","г","д","е","ё","ж","з","и",
    "й","к","л","м","н","о","п","р","с","т","у","ф","х","ц","ч","ш","щ","ъ","ы","ь",
    "э","ю","я",
    "А","Б","В","Г","Д","Е","Ё","Ж","З","И","Й","К","Л","М","Н","О","П",
    "Р","С","Т","У","Ф","Х","Ц","Ч","Ш","Щ","Ъ","Ы","Ь","Э","Ю","Я",
    "á","ä","č","ď","é","ě","í","ľ","ĺ","ň","ó","ô","ö","ő","ř","ŕ","š","ť","ú","ů","ü","ű","ý","ž",
    "Á","Ä","Č","Ď","É","Ě","Í","Ľ","Ĺ","Ň","Ó","Ô","Ö","Ő","Ř","Ŕ","Š","Ť","Ú","Ů","Ü","Ű","Ý","Ž",QString::null};
    
const QString TranslitPlugin::s_strEngl[]= {"a","b","v","g","d","e","yo","zh","z","i",
    "j","k","l","m","n","o","p","r","s","t","u","f","h","c","ch","sh","sh","","y","",
    "e","yu","ya",
    "A","B","V","G","D","E","Yo","Zh","Z","I","J","K","L","M","N","O","P",
    "R","S","T","U","F","H","C","Ch","Sh","Sh","","Y","","E","Yu","Ya",
    "a","a","c","d","e","e","i","l","l","n","o","o","o","o","r","r","s","t","u","u","u","u","y","z",
    "A","A","C","D","E","E","I","L","L","N","O","O","O","O","R","R","S","T","U","U","U","U","Y","Z",QString::null};

QHash<QString,QString> TranslitPlugin::s_mapFromUTF8;

TranslitPlugin::TranslitPlugin()
    : FilePlugin()
{
    this->addSupportedToken("transliterated");
    this->addSupportedToken("transliterated;.*");

    m_help.append( "[transliterated];;" + i18n("Inserts the transliterated original filename") );
    m_help.append( "[%transliterated];;" + i18n("Inserts the transliterated original filename converted to lower case") );
    m_help.append( "[&transliterated];;" + i18n("Inserts the transliterated original filename converted to upper case") );
    m_help.append( "[*transliterated];;" + i18n("Inserts the transliterated original filename capitalized") );
    m_help.append( "[transliterated;Some string];;" + i18n("Transliterates the string following the semicolon") );

    m_name = i18n("Transliteration");

    if( TranslitPlugin::s_mapFromUTF8.isEmpty() )
    {
        int i=0;
        while( TranslitPlugin::s_strUtf8[i]!=QString::null ) 
        {
            TranslitPlugin::s_mapFromUTF8[ TranslitPlugin::s_strUtf8[i] ] = TranslitPlugin::s_strEngl[i];

            ++i;
        }

    }
}

TranslitPlugin::~TranslitPlugin()
{

}


QString TranslitPlugin::processFile( BatchRenamer* b, int index, const QString & filenameOrToken, EPluginType eCurrentType )
{
    QString src;
    QString token;

    // This plugin supports to types
    if( eCurrentType == ePluginType_Token ) 
    {
        if( filenameOrToken.contains( ";" ) )
        {
            src   = filenameOrToken.section( ';', 1, 1 );
            token = filenameOrToken.section( ';', 0, 0 ).toLower();
        } else 
            token = filenameOrToken.toLower();

        if( token == "transliterated" )
        {
            if( src.isEmpty() )
            {
                // TODO: If extension .....
                if( true ) 
                    src = b->files()->at( index ).srcFilename();
                else
                    src = b->files()->at( index ).srcExtension();                
            }

            return this->transliterate( src );
        }
    }
    /*
    else if( eCurrentType == ePluginType_Filename ) 
    {
        // TODO: If extension .....
        if( true ) 
            src = b->files()->at( index ).srcFilename();
        else
            src = b->files()->at( index ).srcExtension();

        return this->transliterate( src );
    }
    */
    return QString::null;
}

/*
void TranslitPlugin::createUI( QWidget* parent ) const
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
    l->addWidget( new QLabel( i18n("This plugin will transliterate the following characters."), parent  ) );

    KListWidget* list = new KListWidget( parent );

    int i=0;
    while( TranslitPlugin::s_strUtf8[i]!=QString::null ) 
    {
        QString text = QString::fromUtf8( TranslitPlugin::s_strUtf8[i].toUtf8().data() );
        text += " -> ";
        text += TranslitPlugin::s_strEngl[i];;

        list->insertItem( 0, text );
        
        ++i;
    }
    
    l->addWidget( list );
    l->setStretchFactor( list, 2 );
}
*/

QString TranslitPlugin::transliterate(const QString & unicoded)
{
    int i;
    QString transed = "";
    
    transed.reserve(unicoded.length());

    for(i=0; i<(int)unicoded.length(); i++) 
    {
        QString charIn = unicoded.mid(i, 1);
        qDebug("Found: looking for=%s\n", charIn.toUtf8().data() );
        if( TranslitPlugin::s_mapFromUTF8.contains( charIn.toUtf8() ) )
        {
            qDebug("Found: charIn=%s\n", charIn.toUtf8().data() );
            QString charTrans = TranslitPlugin::s_mapFromUTF8[charIn.toUtf8()];
            transed.append(charTrans);
        }
        else
            transed.append(charIn);
       
    }

    return transed;
}

