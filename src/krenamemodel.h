// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#ifndef KRENAMEMODEL_H
#define KRENAMEMODEL_H

#include "krenamefile.h"
#include "krenametokensorter.h"

#include <QAbstractListModel>

class BatchRenamer;
class ThreadedLister;
class KFileItem;
class QPixmap;

/** This enum is used to specify a sorting mode
 */
enum ESortMode {
    eSortMode_Unsorted,
    eSortMode_Ascending,
    eSortMode_Descending,
    eSortMode_Numeric,
    eSortMode_Random,
    eSortMode_AscendingDate,
    eSortMode_DescendingDate,
    eSortMode_Token
};

class KRenameModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit KRenameModel(KRenameFile::List *vector);
    ~KRenameModel() override;

    /**
     * Set the batchrenamer instance.
     */
    inline void setRenamer(BatchRenamer *renamer);

    /** Add a KRenameFile to the model
     *
     *  @param files a KRenameFile::List which is added to the internal list
     */
    void addFiles(const KRenameFile::List &files);

    /** Remove items in the model
     *
     *  @param remove list of indexes to remove
     */
    void removeFiles(const QList<int> &remove);

    /** Sort the data in the model
     *  using the selected sort mode.
     *
     *  @param mode the sort mode to use
     *  @param customSortToken customSortToken if mode is eSortMode_Token
     *  @param customSortMode mode for sorting if mode is eSortMode_Token
     */
    void sortFiles(ESortMode mode, const QString &customSortToken,
                   KRenameTokenSorter::ESimpleSortMode customSortMode);

    /** Get the current sort mode.
     *  @returns the current sort mode
     */
    inline ESortMode getSortMode() const;

    inline QString getSortModeCustomToken() const;
    inline KRenameTokenSorter::ESimpleSortMode getSortModeCustomMode() const;

    /** Move each file in a list of indices upwards
     *  @param files list of file indices. Each file is moved up one position
     */
    void moveFilesUp(const QList<int> &files);

    /** Move each file in a list of indices downwards
     *  @param files list of file indices. Each file is moved down one position
     */
    void moveFilesDown(const QList<int> &files);

    /** Creates a new model index
     *
     *  @param row the index of the requested file
     *  @returns the model index for a certain row
     */
    const QModelIndex createIndex(int row) const;

    /** Get the file at position index.
     *
     *  @param a valid index in the internal vector
     *
     *  @returns a KRenameFile object
     */
    inline const KRenameFile &file(int index) const;

    /** Get the file at position index.
     *
     *  @param a valid index in the internal vector
     *
     *  @returns a KRenameFile object
     */
    inline KRenameFile &file(int index);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Qt::DropActions supportedDropActions() const override;
    QStringList mimeTypes() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent) override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    /** Enable the preview of KRenameFile objects.
     *
     *  @param preview enable/disable preview
     *  @param filenames if preview is true this items decides
     *                   if the filename text is displayed next to the preview
     */
    inline void setEnablePreview(bool preview, bool filenames);
    /** Test if the preview of KRenameFile objects is enabled.
     *
     *  @return preview enabled?
     */
    inline bool isPreviewEnabled() const;

    /** Run/open the file which is pointed to by the passed modelinex
     *
     *  @param index a modelindex specifying a file to open
     *  @param window a window used to cache authentication information
     */
    void run(const QModelIndex &index, QWidget *window) const;

    /**
     * Specify the extension split mode.
     * \param splitMode split mode
     * \param dot dot t use for eSplitMode_CustomDot
     */
    inline void setExtensionSplitMode(ESplitMode splitMode, unsigned int dot);

    inline ESplitMode splitMode();
    inline unsigned int splitDot();

Q_SIGNALS:
    /** This signal is emitted when the maximum number of
     *  dots in a filename that can be used to separate
     *  filename and extension has changed (by adding a new file).
     *
     *  @param dots the maximum number of dots in a filename
     */
    void maxDotsChanged(int dots);

    /** Emitted when files have been added using drag and drop
     */
    void filesDropped();

private Q_SLOTS:
    void slotListerDone(ThreadedLister *lister);
    void gotPreview(const KFileItem &item, const QPixmap &preview);

private:
    BatchRenamer *m_renamer;
    KRenameFile::List *m_vector;

    bool               m_preview;
    bool               m_text;

    int                m_maxDots;  ///< The maximum number of dots in a filename which can be used to separate filename and extension
    const char        *m_mimeType; ///< MIME type for drag and drop operations

    ESortMode          m_eSortMode; ///< Last used sort mode
    QString            m_customSortToken; ///< if m_eSortMode = eSortMode_Token
    KRenameTokenSorter::ESimpleSortMode m_eCustomSortMode;  ///< if m_eSortMode = eSortMode_Token

    ESplitMode        m_eSplitMode;
    unsigned int      m_dot;
};

void KRenameModel::setRenamer(BatchRenamer *renamer)
{
    m_renamer = renamer;
}

ESortMode KRenameModel::getSortMode() const
{
    return m_eSortMode;
}

QString KRenameModel::getSortModeCustomToken() const
{
    return m_customSortToken;
}

KRenameTokenSorter::ESimpleSortMode KRenameModel::getSortModeCustomMode() const
{
    return m_eCustomSortMode;
}

const KRenameFile &KRenameModel::file(int index) const
{
    return m_vector->at(index);
}

KRenameFile &KRenameModel::file(int index)
{
    return (*m_vector)[index];
}

bool KRenameModel::isPreviewEnabled() const
{
    return m_preview;
}

void KRenameModel::setEnablePreview(bool preview, bool filenames)
{
    bool update = false;
    if (m_preview != preview || filenames != m_text) {
        update = true;
    }

    m_preview = preview;
    m_text    = filenames;

    if (update) {
        // TODO: update the model
        ;
    }
}

void KRenameModel::setExtensionSplitMode(ESplitMode splitMode, unsigned int dot)
{
    m_eSplitMode = splitMode;
    m_dot = dot;
}

ESplitMode KRenameModel::splitMode()
{
    return m_eSplitMode;
}

unsigned int KRenameModel::splitDot()
{
    return m_dot;
}

class KRenamePreviewModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit KRenamePreviewModel(KRenameFile::List *vector);
    ~KRenamePreviewModel() override;

    void refresh();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex parent(const QModelIndex &index) const override;
    QModelIndex sibling(int row, int column, const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
private:
    KRenameFile::List *m_vector;
};

#endif // KRENAMEMODEL_H

