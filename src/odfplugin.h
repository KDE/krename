/***************************************************************************
                      odfplugin.h  -  description
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

#ifndef ODF_PLUGIN_H
#define ODF_PLUGIN_H

#include "fileplugin.h"

class OdfPlugin : public FilePlugin
{
public:
    explicit OdfPlugin(PluginLoader *loader);

public: // FilePlugin API
    QString processFile(BatchRenamer *b, int index, const QString &filenameOrToken, EPluginType eCurrentType) override;

public: // Plugin API
    const QStringList &help() const override;

private:
    QStringList m_help;

    // tokens are first set to real string, then to lowercase variant for comparison usage in processFile()
    // see OdfPlugin constructor
    QString m_creatorToken;
    QString m_keywordsToken;
    QString m_subjectToken;
    QString m_titleToken;
    QString m_generatorToken;
    QString m_languageToken;
    QString m_pageCountToken;
    QString m_wordCountToken;
};

#endif // ODF_PLUGIN_H
