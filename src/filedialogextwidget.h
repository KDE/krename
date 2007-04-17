/***************************************************************************
                    filedialogextwidget.h -  description
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

#ifndef _FILE_DIALOG_EXT_WIDGET_H_
#define _FILE_DIALOG_EXT_WIDGET_H_

#include <QCheckBox>
#include <QWidget>

class FileDialogExtWidget : public QWidget {
 Q_OBJECT
 public:
    FileDialogExtWidget();

    /** 
     * \returns true if directories should be added recursively
     */
    inline bool addRecursively() const;

    /** 
     * \returns true if hidden directories should be added too
     *               when adding directories recusively
     */
    inline bool addHidden() const;

    /**
     * \returns true if directory names should be added along with 
     *               their contents
     */
    inline bool addDirsWithFiles() const;

    /**
     * \returns true if only directories should be added and no files
     */
    inline bool addDirsOnly() const;

    /** 
     * \param b if true adding recursively will be enabled by default
     */
    inline void setAddRecursively( bool b );

 private slots:
    void enableControls();

 private:
    QCheckBox* checkRecursive;
    QCheckBox* checkHidden;
    QCheckBox* checkDir;
    QCheckBox* checkOnlyDir;
};

bool FileDialogExtWidget::addRecursively() const
{
    return checkRecursive->isChecked();
}

bool FileDialogExtWidget::addHidden() const
{
    return checkHidden->isChecked();
}

bool FileDialogExtWidget::addDirsWithFiles() const
{
    return checkDir->isChecked();
}

bool FileDialogExtWidget::addDirsOnly() const
{
    return checkOnlyDir->isChecked();
}

void FileDialogExtWidget::setAddRecursively( bool b )
{
    checkRecursive->setChecked( b );
}

#endif // _FILE_DIALOG_EXT_WIDGET_H_
