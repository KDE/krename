/***************************************************************************
                       previewlist.cpp  -  description
                             -------------------
    begin                : Sat Oct 06 2007
    copyright            : (C) 2007 by Dominik Seichter
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

    emit updateCount();
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
