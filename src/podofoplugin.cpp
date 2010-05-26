/***************************************************************************
                      podofoplugin.cpp  -  description
                             -------------------
    begin                : Wed May 26th 2010
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

#ifdef HAVE_PODOFO
#include "podofoplugin.h"

#include <klocale.h>

#include <podofo/podofo.h>

#include "batchrenamer.h"

using namespace PoDoFo;

PodofoPlugin::PodofoPlugin( PluginLoader* loader )
    : FilePlugin( loader )
{
    this->addSupportedToken("pdfAuthor");
    this->addSupportedToken("pdfCreator");
    this->addSupportedToken("pdfKeywords");
    this->addSupportedToken("pdfSubject");
    this->addSupportedToken("pdfTitle");
    this->addSupportedToken("pdfProducer");
    this->addSupportedToken("pdfPages");
    m_help.append( "[pdfAuthor];;" + i18n("Author of the pdf file") );
    m_help.append( "[pdfCreator];;" + i18n("Creator of the pdf file") );
    m_help.append( "[pdfKeywords];;" + i18n("Keywords of the pdf file") );
    m_help.append( "[pdfSubject];;" + i18n("Subject of the pdf file") );
    m_help.append( "[pdfTitle];;" + i18n("Title of the pdf file") );
    m_help.append( "[pdfProducer];;" + i18n("Producer of the pdf file") );
    m_help.append( "[pdfPages];;" + i18n("Number of pages in the pdf file") );

    m_name = i18n("PoDoFo (PDF) Plugin");
    m_comment = i18n("<qt>This plugin supports reading tags from "
                     "PDF files.</qt>");

    m_icon = "application-pdf";
}


QString PodofoPlugin::processFile( BatchRenamer* b, int index, const QString & filenameOrToken, EPluginType )
{
    QString token( filenameOrToken.toLower() );
    QString filename = (*b->files())[index].srcUrl().path();

    if( !this->supports( token ) )
        return QString("");

    try {
        PdfMemDocument doc;
        doc.Load(filename.toUtf8().data());
        PdfInfo* info = doc.GetInfo();

        if( token == "pdfauthor" ) 
            return QString::fromUtf8(info->GetAuthor().GetStringUtf8().c_str());
        else if( token == "pdfcreator" )
            return QString::fromUtf8(info->GetCreator().GetStringUtf8().c_str());
        else if( token == "pdfkeywords" )
            return QString::fromUtf8(info->GetKeywords().GetStringUtf8().c_str());
        else if( token == "pdfsubject" )
            return QString::fromUtf8(info->GetSubject().GetStringUtf8().c_str());
        else if( token == "pdftitle" )
            return QString::fromUtf8(info->GetTitle().GetStringUtf8().c_str());
        else if( token == "pdfproducer" )
            return QString::fromUtf8(info->GetProducer().GetStringUtf8().c_str());
        else if( token == "pdfpages" )
            return QString::number(doc.GetPageCount());
    }
    catch( PdfError & error ) 
    {
        return QString::fromUtf8( error.what() );
    }

    return QString("");
}

#endif // HAVE_PODOFO
