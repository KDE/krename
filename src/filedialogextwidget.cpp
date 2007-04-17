/***************************************************************************
                    filedialogextwidget.cpp -  description
                             -------------------
    begin                : Tue Apr 17 2007
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

#include "filedialogextwidget.h"

// Qt includes
#include <QHBoxLayout>
#include <QToolTip>
#include <QVBoxLayout>

#include <klocale.h>

FileDialogExtWidget::FileDialogExtWidget()
    : QWidget()
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    QHBoxLayout* hbox   = new QHBoxLayout();
    
    checkDir       = new QCheckBox( i18n("Add directory names &with filenames"), this );
    checkRecursive = new QCheckBox( i18n("Add subdirectories &recursively"), this );
    checkHidden    = new QCheckBox( i18n("Add &hidden directories"), this );
    checkOnlyDir   = new QCheckBox( i18n("Add directory names only"), this );    
    
    hbox->addSpacing ( 20 );
    hbox->addWidget  ( checkHidden );
    hbox->setStretchFactor( checkHidden, 4 );
    
    layout->addWidget( checkDir );
    layout->addWidget( checkRecursive );
    layout->addLayout( hbox );
    layout->addWidget( checkOnlyDir );
    
    connect( checkRecursive, SIGNAL( clicked() ), SLOT( enableControls() ));

    checkRecursive->setToolTip( i18n("Walk recursively through the directory tree and add also the content "
                                     "of all subdirectories to the list of files to rename.") );
    checkHidden->setToolTip( i18n("If not checked, KRename will ignore directories starting "
                                  "with a dot during recursive adding.") );
    checkOnlyDir->setToolTip( i18n("Add only the directory names and not the names "
                                   "of the files in the directory to KRename.") );
    checkDir->setToolTip( i18n("This option causes KRename to add also the name of the base "
                               "directory of the selected files to its list.") );
    
    enableControls();
}

void FileDialogExtWidget::enableControls()
{
    checkHidden->setEnabled( checkRecursive->isChecked() );
}

#include "filedialogextwidget.moc"
