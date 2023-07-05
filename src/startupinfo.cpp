// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2009 Dominik Seichter <domseichter@web.de>

#include "startupinfo.h"

#include "ui_startupinfowidget.h"

#include <QIcon>

#include <kiconloader.h>

StartUpInfo::StartUpInfo(QWidget *parent)
    : QFrame(parent)
{
    m_widget = new Ui::StartUpInfoWidget();
    m_widget->setupUi(this);

    m_widget->krenameIcon->setPixmap(QIcon::fromTheme("krename").pixmap(KIconLoader::SizeMedium));
    m_widget->addIcon->setPixmap(QIcon::fromTheme("document-open-folder").pixmap(KIconLoader::SizeSmallMedium));
    m_widget->templateIcon->setPixmap(QIcon::fromTheme("edit-rename").pixmap(KIconLoader::SizeSmallMedium));

    connect(m_widget->labelAdd, static_cast<void (KUrlLabel::*)()>(&KUrlLabel::leftClickedUrl),
            this, &StartUpInfo::addFiles);
    connect(m_widget->labelTemplate, static_cast<void (KUrlLabel::*)()>(&KUrlLabel::leftClickedUrl),
            this, &StartUpInfo::enterTemplate);
}

StartUpInfo::~StartUpInfo()
{
    delete m_widget;
}

#include "moc_startupinfo.cpp"
