// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2005 Dominik Seichter <domseichter@web.de>

#ifndef THREADEDLISTER_H
#define THREADEDLISTER_H

#include <QThread>
#include <QMutex>

#include <kfileitem.h>

#include "krenamefile.h"

class KRenameModel;
class QMutex;
class QWidget;

namespace KIO
{
class Job;
};

class ThreadedLister : public QObject
{
    Q_OBJECT
public:
    ThreadedLister(const QUrl &dirname, QWidget *cache, KRenameModel *model);
    ~ThreadedLister() override;

    inline void start()
    {
        this->run();
    }

    /** Sets if dirnames should listed along with the filenames.
     *  This is disabled by default.
     *
     *  \param names if true dirnames are listed along with the filenames.
     */
    inline void setListDirnames(bool names);

    /** Sets if only dirnames should listed.
     *  This is disabled by default.
     *
     *  \param names if true only dirnames are listed.
     */
    inline void setListDirnamesOnly(bool names);

    inline void setFilter(const QString &filter);
    inline void setListHidden(bool h);
    inline void setListRecursively(bool r);

Q_SIGNALS:
    void listerDone(ThreadedLister *);

protected:
    void run();

private Q_SLOTS:
    void completed();

    void foundItem(KIO::Job *, const KIO::UDSEntryList &list);

private:
    static QMutex     s_mutex; ///< Mutex assuring that only one thread at a time will work on m_model

    QUrl              m_dirname;
    QString           m_filter;
    bool              m_listHiddenFiles;
    bool              m_listRecursive;
    bool              m_listDirnamesOnly;
    bool              m_listDirnames;

    QWidget          *m_cache;
    KRenameModel     *m_model;
    KRenameFile::List m_files;

    ESplitMode        m_eSplitMode;
    unsigned int      m_dot;
};

void ThreadedLister::setListDirnames(bool names)
{
    m_listDirnames = names;
}

void ThreadedLister::setListDirnamesOnly(bool names)
{
    m_listDirnamesOnly = names;
}

void ThreadedLister::setFilter(const QString &filter)
{
    m_filter = filter;
}

void ThreadedLister::setListHidden(bool h)
{
    m_listHiddenFiles = h;
}

void ThreadedLister::setListRecursively(bool r)
{
    m_listRecursive = r;
}

#endif
