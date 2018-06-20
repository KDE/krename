/***************************************************************************
                      odfplugin.cpp  -  description
                             -------------------
    begin                : Wed June 20th 2018
    copyright            : (C) 2018 by Friedrich W. H. Kossebau
    email                : kossebau@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "odfplugin.h"

#include "batchrenamer.h"
#include "tokenhelpdialog.h"
// KF
#include <KZip>
// Qt
#include <QDomDocument>
#include <QDebug>

namespace {
inline QString dcNS()     { return QStringLiteral("http://purl.org/dc/elements/1.1/"); }
inline QString metaNS()   { return QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:meta:1.0"); }
inline QString officeNS() { return QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:office:1.0"); }

QString createHelpString(const QString &tokenId, const QString &description)
{
    return QLatin1Char('[') + tokenId + QLatin1Char(']') + TokenHelpDialog::getTokenSeparator() + description;
}

QDomElement namedItemNS(const QDomNode &node, const QString &nsURI, const QString &localName)
{
    for (auto n = node.firstChildElement(); !n.isNull(); n = n.nextSiblingElement()) {
        if (n.localName() == localName && n.namespaceURI() == nsURI) {
            return n;
        }
    }

    return QDomElement();
}
}


OdfPlugin::OdfPlugin(PluginLoader *loader)
    : FilePlugin(loader)
    , m_creatorToken("odfCreator")
    , m_keywordsToken("odfKeywords")
    , m_subjectToken("odfSubject")
    , m_titleToken("odfTitle")
    , m_generatorToken("odfGenerator")
    , m_languageToken("odfLanguage")
    , m_pageCountToken("odfPageCount")
    , m_wordCountToken("odfWordCount")
{
    m_name = i18n("OpenDocument Format (ODT, ODS, ODP) Plugin");
    m_comment = i18n("<qt>This plugin supports reading metadata from "
                     "files in an OpenDocument format.</qt>");

    // there is no generic ODF icon, so use the text one for now
    m_icon = "application-vnd.oasis.opendocument.text";

    addSupportedToken(m_creatorToken);
    addSupportedToken(m_keywordsToken);
    addSupportedToken(m_subjectToken);
    addSupportedToken(m_titleToken);
    addSupportedToken(m_generatorToken);
    addSupportedToken(m_languageToken);
    addSupportedToken(m_pageCountToken);
    addSupportedToken(m_wordCountToken);

    m_help = QStringList {
        createHelpString(m_creatorToken, i18n("Creator of the ODF file")),
        createHelpString(m_keywordsToken, i18n("Keywords of the ODF file")),
        createHelpString(m_subjectToken, i18n("Subject of the ODF file")),
        createHelpString(m_titleToken, i18n("Title of the ODF file")),
        createHelpString(m_generatorToken, i18n("Generator of the ODF file")),
        createHelpString(m_languageToken, i18n("Language of the ODF file")),
        createHelpString(m_pageCountToken, i18n("Number of pages in the ODF file")),
        createHelpString(m_wordCountToken, i18n("Number of words in the ODF file")),
    };

    // prepare for case-insensitive comparison
    m_creatorToken = m_creatorToken.toLower();
    m_keywordsToken = m_keywordsToken.toLower();
    m_subjectToken = m_subjectToken.toLower();
    m_titleToken = m_titleToken.toLower();
    m_generatorToken = m_generatorToken.toLower();
    m_languageToken = m_languageToken.toLower();
    m_pageCountToken = m_pageCountToken.toLower();
    m_wordCountToken = m_wordCountToken.toLower();
}


QString OdfPlugin::processFile(BatchRenamer *b, int index, const QString &filenameOrToken, EPluginType)
{
    const QString token = filenameOrToken.toLower();

    if (!supports(token)) {
        return QString("");
    }

    const QString filename = (*b->files())[index].srcUrl().path();
    KZip zip(filename);
    if (!zip.open(QIODevice::ReadOnly)) {
        return QString("");
    }

    const KArchiveDirectory* directory = zip.directory();
    if (!directory) {
        return QString("");
    }

    // we need a meta xml file in the archive!
    const auto metaXml = directory->entry(QStringLiteral("meta.xml"));
    if (!metaXml || !metaXml->isFile()) {
        return QString("");
    }

    QDomDocument metaDataDocument(QStringLiteral("metaData"));
    bool success = metaDataDocument.setContent(static_cast<const KArchiveFile*>(metaXml)->data(), true);
    if (!success)  {
        return QString("");
    }

    // parse metadata ..
    QDomElement meta = namedItemNS(namedItemNS(metaDataDocument,
                                               officeNS(), QStringLiteral("document-meta")),
                                               officeNS(), QStringLiteral("meta"));

    // Dublin Core
    if (token == m_subjectToken) {
        return namedItemNS(meta, dcNS(), QStringLiteral("subject")).text();
    }
    if (token == m_titleToken) {
        return namedItemNS(meta, dcNS(), QStringLiteral("title")).text();
    }
    if (token == m_creatorToken) {
        return namedItemNS(meta, dcNS(), QStringLiteral("creator")).text();
    }
    if (token == m_languageToken) {
        return namedItemNS(meta, dcNS(), QStringLiteral("language")).text();
    }

    // Meta Properties
    if (token == m_keywordsToken) {
        return namedItemNS(meta, metaNS(), QStringLiteral("keyword")).text();
    }
    if (token == m_generatorToken) {
        return namedItemNS(meta, metaNS(), QStringLiteral("generator")).text();
    }

    const QString attributeName =
        (token == m_pageCountToken) ? QStringLiteral("page-count") :
        (token == m_wordCountToken) ? QStringLiteral("word-count") :
        /* else */                    QString();

    if (!attributeName.isEmpty()) {
        return namedItemNS(meta, metaNS(), QStringLiteral("document-statistic")).attributeNS(metaNS(), attributeName);
    }

    return QString("");
}

const QStringList & OdfPlugin::help() const
{
    return m_help;
}
