// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#ifndef PREVIEW_LIST_H
#define PREVIEW_LIST_H

#include <QTreeView>

class KRenameModel;
class QMenu;

class PreviewList : public QTreeView
{
    Q_OBJECT

public:
    explicit PreviewList(QWidget *parent);
    ~PreviewList() override;

    inline void setKRenameModel(KRenameModel *model)
    {
        m_model = model;
    }

Q_SIGNALS:
    void addFiles();
    void updateCount();

private Q_SLOTS:
    void slotOpen();
    void slotRemove();
    void slotManually();

protected:
    void contextMenuEvent(QContextMenuEvent *e) override;

private:
    KRenameModel *m_model;
    QMenu        *m_menu;
};

#endif // PREVIEW_LIST_H

