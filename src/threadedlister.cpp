// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2005 Dominik Seichter <domseichter@web.de>

#include "threadedlister.h"

#include "krenamemodel.h"

#include <KIO/ListJob>

#include <QMutex>
#include <QRegExp>

QMutex ThreadedLister::s_mutex;

ThreadedLister::ThreadedLister(const QUrl &dirname, QWidget *cache, KRenameModel *model)
    : QObject(nullptr), m_dirname(dirname), m_cache(cache), m_model(model)
{
    m_listHiddenFiles  = false;
    m_listRecursive    = false;
    m_listDirnamesOnly = false;
    m_listDirnames     = false;
    m_eSplitMode       = m_model->splitMode();
    m_dot              = m_model->splitDot();

    qRegisterMetaType<KFileItemList>("KFileItemList");
}

ThreadedLister::~ThreadedLister()
{
}

void ThreadedLister::run()
{
    s_mutex.lock();
    if (m_listDirnames || m_listDirnamesOnly) {
        QString name = m_dirname.fileName();
        if (!m_listHiddenFiles && !name.startsWith(QLatin1Char('.'))) {
            KRenameFile::List list;
            list.append(KRenameFile(m_dirname, true, m_eSplitMode, m_dot));

            m_model->addFiles(list);
        }
    }
    s_mutex.unlock();

    KIO::ListJob *job   = nullptr; // Will delete itself automatically
    KIO::JobFlags flags = KIO::HideProgressInfo;
#if QT_VERSION_MAJOR == 6
    KIO::ListJob::ListFlags listFlags = m_listHiddenFiles ? KIO::ListJob::ListFlag::IncludeHidden : KIO::ListJob::ListFlags();
#else
    bool listFlags = m_listHiddenFiles;
#endif
    if (m_listRecursive) {
        job = KIO::listRecursive(m_dirname, flags, listFlags);
    } else {
        job = KIO::listDir(m_dirname, flags, listFlags);
    }

    connect(job, &KIO::ListJob::entries, this, &ThreadedLister::foundItem);
    connect(job, &KIO::ListJob::result, this, &ThreadedLister::completed);

    job->start();
}

void ThreadedLister::foundItem(KIO::Job *, const KIO::UDSEntryList &list)
{
    QString displayName;
    QRegExp filter(m_filter);
    filter.setPatternSyntax(QRegExp::Wildcard);

    m_files.reserve(m_files.count() + list.count());

    KIO::UDSEntryList::const_iterator it = list.begin();
    while (it != list.end()) {
        displayName = (*it).stringValue(KIO::UDSEntry::UDS_NAME);
        if (!filter.isEmpty() && !filter.exactMatch(displayName)) {
            // does not match filter
            // skip it
            ++it;
        } else {
            QUrl url = m_dirname;
            url = url.adjusted(QUrl::StripTrailingSlash);
            url.setPath(url.path() + '/' + (displayName));

            if ((m_listDirnames || m_listDirnamesOnly) && (*it).isDir()) {
                // Filter out parent and current directory
                if (displayName != "." && displayName != "..") {
                    m_files.append(KRenameFile(KFileItem(*it, url), m_eSplitMode, m_dot));
                }
            } else if (!m_listDirnamesOnly && !(*it).isDir()) {
                m_files.append(KRenameFile(KFileItem(*it, url), m_eSplitMode, m_dot));
            }

            ++it;
        }
    }
}

void ThreadedLister::completed()
{
    if (m_files.count() > 0) {
        // We add the files in the completed slot
        // and not directly in the foundItem slot,
        // as the latter can produce deadlocks if
        // we get a signal while we keep the mutex!
        //s_mutex.lock();
        m_model->addFiles(m_files);
        //s_mutex.unlock();
    }

    Q_EMIT listerDone(this);
}

#include "moc_threadedlister.cpp"
