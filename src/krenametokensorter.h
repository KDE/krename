// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2010 Dominik Seichter <domseichter@web.de>

#ifndef KRENAMETOKENSORTER_H
#define KRENAMETOKENSORTER_H

#include "krenamefile.h"

#include <QMap>
#include <QString>

#include <QUrl>

class BatchRenamer;
class Plugin;

bool ascendingKRenameFileLessThan(const KRenameFile &file1, const KRenameFile &file2);
bool descendingKRenameFileLessThan(const KRenameFile &file1, const KRenameFile &file2);
bool numericKRenameFileLessThan(const KRenameFile &file1, const KRenameFile &file2);

class KRenameTokenSorter
{
public:

    enum ESimpleSortMode {
        eSimpleSortMode_Ascending,
        eSimpleSortMode_Descending,
        eSimpleSortMode_Numeric
    };

    KRenameTokenSorter(BatchRenamer *renamer, const QString &token, const KRenameFile::List &list, ESimpleSortMode eSortMode);

    bool operator()(const KRenameFile &file1, const KRenameFile &file2);

private:
    QString processString(int index) const;

private:
    BatchRenamer *m_renamer;
    const QString &m_token;
    const KRenameFile::List &m_list;
    ESimpleSortMode m_eSortMode;
    Plugin *m_plugin;
    QMap<QUrl, QString> m_values;
};

#endif // KRENAMETOKENSORTER_H
