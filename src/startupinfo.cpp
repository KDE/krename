/***************************************************************************
                       starttupinfo.cüü  -  description
                             -------------------
    begin                : Sat Oct 18 2009
    copyright            : (C) 2009 by Dominik Seichter
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

#include "startupinfo.h"

#include "ui_startupinfowidget.h"
#include <kiconloader.h>

StartUpInfo::StartUpInfo(QWidget* parent)
    : QFrame(parent)
{
    m_widget = new Ui::StartUpInfoWidget();
    m_widget->setupUi(this);

    m_widget->krenameIcon->setPixmap( DesktopIcon("krename") );
    m_widget->addIcon->setPixmap( BarIcon("document-open-folder") );
    m_widget->templateIcon->setPixmap( BarIcon("edit-rename") );

    connect(m_widget->labelAdd, SIGNAL(leftClickedUrl()), SIGNAL(addFiles()));
    connect(m_widget->labelTemplate, SIGNAL(leftClickedUrl()), SIGNAL(enterTemplate()));
}

StartUpInfo::~StartUpInfo()
{
    delete m_widget;
}


