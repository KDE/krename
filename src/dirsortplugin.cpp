// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2009 Dominik Seichter <domseichter@web.de>

// Own includes
#include "dirsortplugin.h"

#include "batchrenamer.h"

// KDE includes
#include <KIO/StatJob>
#include <KIO/FileCopyJob>
#include <KIO/MkdirJob>
#include <kmessagebox.h>
#include <KJobWidgets>

DirSortPlugin::DirSortPlugin(PluginLoader *loader)
    : Plugin(loader)
{
    m_widget = new Ui::DirSortPluginWidget();
}

DirSortPlugin::~DirSortPlugin()
{
    delete m_widget;
}

const QString DirSortPlugin::name() const
{
    return i18n("Subfolder-Sort Plugin");
}

int DirSortPlugin::type() const
{
    return ePluginType_File;
}

bool DirSortPlugin::enabledByDefault() const
{
    return false;
}

const QIcon DirSortPlugin::icon() const
{
    return QIcon::fromTheme("folder");
}

void DirSortPlugin::createUI(QWidget *parent) const
{
    m_widget->setupUi(parent);

    m_widget->outputUrl->setMode(KFile::Directory | KFile::ExistingOnly);
}

/*
void DirSortPlugin::fillStructure()
{
    fpd = spinFiles->value();
    fpd--;
    dir = outputdir->text();

    filecounter = 0;
    dircounter = spinStart->value();
    curdir = dir +  QString("/%1/").arg( dircounter );
    d = new QDir( dir );
    d->mkdir( curdir );
}
*/

QString DirSortPlugin::processFile(BatchRenamer *b, int index, const QString &, EPluginType)
{
    QString errorMsg;

    if (index == 0) {
        // Initialize plugin
        m_dirCounter = m_widget->spinStart->value();
        m_fileCounter = 0;
        m_filesPerDir = m_widget->spinFiles->value();
        m_digits = m_widget->spinDigits->value();
        m_baseDirectory = m_widget->outputUrl->url();

#if QT_VERSION_MAJOR == 6
        KIO::StatJob *statJob = KIO::stat(m_baseDirectory, KIO::StatJob::DestinationSide, KIO::StatNoDetails);
#else
        KIO::StatJob *statJob = KIO::statDetails(m_baseDirectory, KIO::StatJob::DestinationSide, KIO::StatNoDetails);
#endif
        KJobWidgets::setWindow(statJob, m_widget->spinStart);
        statJob->exec();
        if (statJob->error()) {
            m_valid = false;
            return
                i18n("%1: The output folder %2 does not exist.",
                     this->name(),
                     m_baseDirectory.toDisplayString(QUrl::PreferLocalFile));
        } else {
            m_valid = true;

            m_currentDirectory = createNewSubdirectory();
        }
    }

    if (!m_valid) {
        return errorMsg;
    }

    if (m_fileCounter == m_filesPerDir) {
        m_fileCounter = 0;
        m_dirCounter++;

        m_currentDirectory = createNewSubdirectory();
    }

    QUrl srcUrl = b->buildDestinationUrl((*b->files())[index]);
    QUrl dstUrl = m_currentDirectory;
    dstUrl = dstUrl.adjusted(QUrl::StripTrailingSlash);
    dstUrl.setPath(dstUrl.path() + '/' + (srcUrl.fileName()));
    KIO::JobFlags flags = KIO::DefaultFlags | KIO::HideProgressInfo;
    KIO::Job *job = KIO::file_move(srcUrl, dstUrl, -1, flags);
    m_fileCounter++;
    KJobWidgets::setWindow(job, m_widget->spinStart);
    job->exec();
    if (!job->exec()) {
        errorMsg = i18n("Error renaming %2 (to %1)",
                        dstUrl.toDisplayString(QUrl::PreferLocalFile),
                        srcUrl.toDisplayString(QUrl::PreferLocalFile));
    }

    return errorMsg;
}

const QStringList &DirSortPlugin::supportedTokens() const
{
    return m_emptyList;
}

const QStringList &DirSortPlugin::help() const
{
    return m_emptyList;
}

QUrl DirSortPlugin::createNewSubdirectory() const
{
    QUrl url = m_baseDirectory;

    QString dir = QString::asprintf("%0*i", m_digits, m_dirCounter);
    url = url.adjusted(QUrl::StripTrailingSlash);
    url.setPath(url.path() + '/' + (dir));

    KIO::MkdirJob *job = KIO::mkdir(url);
    KJobWidgets::setWindow(job, m_widget->groupBox); // we just need a random widget, FIXME use the proper parent

    if (!job->exec()) {
        KMessageBox::error(m_widget->groupBox,
                           i18n("Cannot create folder %1", url.toDisplayString(QUrl::PreferLocalFile)));
    }

    return url;
}
