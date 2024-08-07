// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#include "progressdialog.h"

#include "batchrenamer.h"
#include "krenameimpl.h"

#include <QMenu>

#include <KIO/OpenUrlJob>
#include <KIO/JobUiDelegateFactory>

ProgressDialog::ProgressDialog(ESplitMode eSplitMode, unsigned int dot, QWidget *parent)
    : QDialog(parent), m_canceled(false), m_renamer(nullptr), m_eSplitMode(eSplitMode), m_dot(dot)
{
    m_widget.setupUi(this);

    m_buttonMore  = m_widget.buttonBox->addButton(i18n("&Rename more..."), QDialogButtonBox::ActionRole);
    m_buttonUndo  = m_widget.buttonBox->addButton(i18n("&Undo"), QDialogButtonBox::ActionRole);
    m_buttonDest  = m_widget.buttonBox->addButton(i18n("&Open Destination"), QDialogButtonBox::ActionRole);
    m_buttonClose = m_widget.buttonBox->button(QDialogButtonBox::Close);

    m_buttonUndo->setEnabled(false);
    m_buttonMore->setEnabled(false);
    m_buttonClose->setEnabled(false);

    connect(m_widget.buttonCancel, &QPushButton::clicked,
            this, &ProgressDialog::slotCancelled);
    connect(m_buttonDest, &QPushButton::clicked,
            this, &ProgressDialog::slotOpenDestination);
    connect(m_buttonUndo, &QPushButton::clicked,
            this, &ProgressDialog::slotUndo);
    connect(m_buttonClose, &QPushButton::clicked,
            qApp, &QApplication::quit, Qt::QueuedConnection);

    QMenu *menu = new QMenu(this);
    menu->addAction(i18n("Restart &KRename..."), this, &ProgressDialog::slotRestartKRename);
    menu->addSeparator();
    m_actProcessed = menu->addAction(i18n("Rename Processed Files &Again..."), this, &ProgressDialog::slotRenameProcessedAgain);
    m_actUnprocessed = menu->addAction(i18n("Rename &Unprocessed Files Again..."), this, &ProgressDialog::slotRenameUnprocessedAgain);
    menu->addAction(i18n("&Rename All Files Again..."), this, &ProgressDialog::slotRenameAllAgain);

    m_buttonMore->setMenu(menu);
}

void ProgressDialog::slotCancelled()
{
    m_canceled = true;
}

void ProgressDialog::slotOpenDestination()
{
    auto *job = new KIO::OpenUrlJob(m_dest);
    job->setUiDelegate(KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoErrorHandlingEnabled, this));
    job->start();
}

void ProgressDialog::slotRestartKRename()
{
    (void *)KRenameImpl::launch(QRect(0, 0, 0, 0), KRenameFile::List());
    QDialog::done(0);
}

void ProgressDialog::slotRenameProcessedAgain()
{
    KRenameFile::List list;
    KRenameFile::List::const_iterator it = m_renamer->files()->begin();

    list.reserve(m_renamer->files()->count());
    while (it != m_renamer->files()->end()) {
        if (!(*it).hasError()) {
            KRenameFile file(m_renamer->buildDestinationUrl(*it), (*it).isDirectory(), m_eSplitMode, m_dot);
            file.setIcon(file.icon());
            list.append(file);
        }

        ++it;
    }

    (void *)KRenameImpl::launch(QRect(0, 0, 0, 0), list);
    QDialog::done(0);
}

void ProgressDialog::slotRenameUnprocessedAgain()
{
    KRenameFile::List list;
    KRenameFile::List::const_iterator it = m_renamer->files()->begin();

    list.reserve(m_renamer->files()->count());
    while (it != m_renamer->files()->end()) {
        if ((*it).hasError()) {
            KRenameFile file(*it);
            file.setManualChanges(QString(), eManualChangeMode_None);   // reset manual changes!!
            list.append(file);
        }

        ++it;
    }

    (void *)KRenameImpl::launch(QRect(0, 0, 0, 0), list);
    QDialog::done(0);
}

void ProgressDialog::slotRenameAllAgain()
{
    KRenameFile::List list;
    KRenameFile::List::const_iterator it = m_renamer->files()->begin();

    list.reserve(m_renamer->files()->count());
    while (it != m_renamer->files()->end()) {
        KRenameFile file(m_renamer->buildDestinationUrl(*it), (*it).isDirectory(), m_eSplitMode, m_dot);
        file.setIcon(file.icon());
        list.append(file);
        ++it;
    }

    (void *)KRenameImpl::launch(QRect(0, 0, 0, 0), list);
    QDialog::done(0);
}

void ProgressDialog::slotUndo()
{
    if (m_renamer) {
        m_widget.buttonCancel->setEnabled(true);

        m_buttonUndo->setEnabled(false);
        m_buttonMore->setEnabled(false);

        m_renamer->undoFiles(this);
    }
}

void ProgressDialog::renamingDone(bool enableMore, bool enableUndo, BatchRenamer *renamer, int errors)
{
    m_widget.buttonCancel->setEnabled(false);

    m_buttonUndo->setEnabled(enableUndo);
    m_buttonMore->setEnabled(enableMore);   // Do allow renaming processed files after undo
    m_buttonClose->setEnabled(true);
    m_buttonClose->setShortcut(QKeySequence(QKeySequence::Quit));

    m_actProcessed->setEnabled(renamer->files()->count() != errors);
    m_actUnprocessed->setEnabled(0 != errors);

    m_renamer = renamer;
}

void ProgressDialog::closeEvent(QCloseEvent *event)
{
    // KRenameWindow is still around but hidden. When we close this dialog we
    // want to quit the app completely if there isn't some operation still
    // running.
    if (m_buttonClose->isEnabled()) {
        qApp->quit();
        QWidget::closeEvent(event);
    }
}

void ProgressDialog::print(const QString &text, const QString &pixmap)
{
    QIcon icon = QIcon::fromTheme(pixmap);

    new QListWidgetItem(icon, text, m_widget.display);
}

void ProgressDialog::error(const QString &text)
{
    this->print(text, "cancel");
}

void ProgressDialog::warning(const QString &text)
{
    this->print(text, "help-hint");
}

#include "moc_progressdialog.cpp"
