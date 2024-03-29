// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2010 Dominik Seichter <domseichter@web.de>

#include "podofoplugin.h"

#include <podofo/podofo.h>

#include "batchrenamer.h"
#include "tokenhelpdialog.h"

using namespace PoDoFo;

PodofoPlugin::PodofoPlugin(PluginLoader *loader)
    : FilePlugin(loader)
{
    this->addSupportedToken("pdfAuthor");
    this->addSupportedToken("pdfCreator");
    this->addSupportedToken("pdfKeywords");
    this->addSupportedToken("pdfSubject");
    this->addSupportedToken("pdfTitle");
    this->addSupportedToken("pdfProducer");
    this->addSupportedToken("pdfPages");
    m_help.append("[pdfAuthor]" + TokenHelpDialog::getTokenSeparator() + i18n("Author of the PDF file"));
    m_help.append("[pdfCreator]" + TokenHelpDialog::getTokenSeparator() + i18n("Creator of the PDF file"));
    m_help.append("[pdfKeywords]" + TokenHelpDialog::getTokenSeparator() + i18n("Keywords of the PDF file"));
    m_help.append("[pdfSubject]" + TokenHelpDialog::getTokenSeparator() + i18n("Subject of the PDF file"));
    m_help.append("[pdfTitle]" + TokenHelpDialog::getTokenSeparator() + i18n("Title of the PDF file"));
    m_help.append("[pdfProducer]" + TokenHelpDialog::getTokenSeparator() + i18n("Producer of the PDF file"));
    m_help.append("[pdfPages]" + TokenHelpDialog::getTokenSeparator() + i18n("Number of pages in the PDF file"));

    m_name = i18n("PoDoFo (PDF) Plugin");
    m_comment = i18n("<qt>This plugin supports reading tags from "
                     "PDF files.</qt>");

    m_icon = "application-pdf";
}

QString PodofoPlugin::processFile(BatchRenamer *b, int index, const QString &filenameOrToken, EPluginType)
{
    QString token(filenameOrToken.toLower());
    QString filename = (*b->files())[index].srcUrl().path();

    if (!this->supports(token)) {
        return QString("");
    }

    try {
        PdfMemDocument doc;
        doc.Load(filename.toUtf8().data());
#if (PODOFO_VERSION_MINOR>=10 || PODOFO_VERSION_MAJOR>=1)
        const PdfInfo *info = doc.GetInfo();

        if (token == "pdfauthor") {
            return info->GetAuthor().has_value() ? QString::fromUtf8(info->GetAuthor()->GetString().c_str()) : QString();
        } else if (token == "pdfcreator") {
            return info->GetCreator().has_value() ? QString::fromUtf8(info->GetCreator()->GetString().c_str()) : QString();
        } else if (token == "pdfkeywords") {
            return info->GetKeywords().has_value() ? QString::fromUtf8(info->GetKeywords()->GetString().c_str()) : QString();
        } else if (token == "pdfsubject") {
            return info->GetSubject().has_value() ? QString::fromUtf8(info->GetSubject()->GetString().c_str()) : QString();
        } else if (token == "pdftitle") {
            return info->GetTitle().has_value() ? QString::fromUtf8(info->GetTitle()->GetString().c_str()) : QString();
        } else if (token == "pdfproducer") {
            return info->GetProducer().has_value() ? QString::fromUtf8(info->GetProducer()->GetString().c_str()) : QString();
        } else if (token == "pdfpages") {
            return QString::number(doc.GetPages().GetCount());
        }
#else
        PdfInfo *info = doc.GetInfo();

        if (token == "pdfauthor") {
            return QString::fromUtf8(info->GetAuthor().GetStringUtf8().c_str());
        } else if (token == "pdfcreator") {
            return QString::fromUtf8(info->GetCreator().GetStringUtf8().c_str());
        } else if (token == "pdfkeywords") {
            return QString::fromUtf8(info->GetKeywords().GetStringUtf8().c_str());
        } else if (token == "pdfsubject") {
            return QString::fromUtf8(info->GetSubject().GetStringUtf8().c_str());
        } else if (token == "pdftitle") {
            return QString::fromUtf8(info->GetTitle().GetStringUtf8().c_str());
        } else if (token == "pdfproducer") {
            return QString::fromUtf8(info->GetProducer().GetStringUtf8().c_str());
        } else if (token == "pdfpages") {
            return QString::number(doc.GetPageCount());
        }
#endif
    } catch (PdfError &error) {
        return QString::fromUtf8(error.what());
    }

    return QString("");
}
