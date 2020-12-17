// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#ifndef FILE_DIALOG_EXT_WIDGET_H
#define FILE_DIALOG_EXT_WIDGET_H

#include <QCheckBox>
#include <QDialog>
#include <KFileWidget>

class FileDialogExtWidget : public QDialog
{
    Q_OBJECT
public:
    explicit FileDialogExtWidget(QWidget *parent);

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
    inline void setAddRecursively(bool b);

    QList<QUrl> selectedUrls()
    {
        return m_fileWidget->selectedUrls();
    }
    QString currentFilter()
    {
        return m_fileWidget->currentFilter();
    }

private Q_SLOTS:
    void enableControls();

private:
    QCheckBox *checkRecursive;
    QCheckBox *checkHidden;
    QCheckBox *checkDir;
    QCheckBox *checkOnlyDir;
    KFileWidget *m_fileWidget;
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

void FileDialogExtWidget::setAddRecursively(bool b)
{
    checkRecursive->setChecked(b);
}

#endif // FILE_DIALOG_EXT_WIDGET_H
