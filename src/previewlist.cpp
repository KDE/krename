// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#include "previewlist.h"

#include "customdialog.h"
#include "krenamemodel.h"

#include <QContextMenuEvent>
#include <QMenu>
#include <QPointer>

PreviewList::PreviewList(QWidget *parent)
    : QTreeView(parent), m_model(nullptr)
{
    m_menu = new QMenu("KRename", this);   // we need any text here so that we have a title
    m_menu->addAction(i18n("&Change filename manually..."), this, &PreviewList::slotManually, QKeySequence("F2"));
    m_menu->addSeparator();
    m_menu->addAction(i18n("&Open"), this, &PreviewList::slotOpen);
    m_menu->addSeparator();
    m_menu->addAction(i18n("&Add..."), this, &PreviewList::addFiles);
    m_menu->addAction(i18n("&Remove"), this, &PreviewList::slotRemove);

    connect(this, &PreviewList::activated, this, &PreviewList::slotManually);
}

PreviewList::~PreviewList()
{

}

void PreviewList::contextMenuEvent(QContextMenuEvent *e)
{
    // only show a context menu if we have model and contents
    if (m_model && m_model->rowCount() && currentIndex().isValid()) {
        const KRenameFile &file  = m_model->file(this->currentIndex().row());

        m_menu->setTitle(file.srcUrl().toDisplayString(QUrl::PreferLocalFile));
        m_menu->popup(e->globalPos());
    }
}

void PreviewList::slotOpen()
{
    m_model->run(this->currentIndex(), this);
}

void PreviewList::slotRemove()
{
    QList<int> list;

    list.append(this->currentIndex().row());

    m_model->removeFiles(list);

    Q_EMIT updateCount();
}

void PreviewList::slotManually()
{
    QPointer<CustomDialog> dialog = new CustomDialog(m_model->file(this->currentIndex().row()), this);
    if (dialog->exec() == QDialog::Accepted) {
        QString manual;
        EManualChangeMode mode = eManualChangeMode_None;
        if (dialog->hasManualChanges()) {
            manual = dialog->manualChanges();
            mode = dialog->manualChangeMode();
        }

        m_model->file(this->currentIndex().row()).setManualChanges(manual, mode);
    }
    delete dialog;
}

#include "moc_previewlist.cpp"
