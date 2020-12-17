// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2009 Dominik Seichter <domseichter@web.de>

#ifndef START_UP_INFO_H
#define START_UP_INFO_H

#include <QFrame>

namespace Ui
{
class StartUpInfoWidget;
};

/**
 * A info widget which tells the user about the next steps
 * when using KRename.
 *
 */
class StartUpInfo : public QFrame
{
    Q_OBJECT

public:
    explicit StartUpInfo(QWidget *parent = nullptr);
    ~StartUpInfo() override;

Q_SIGNALS:
    /**
     * Emitted when the user clicks the link to add more files.
     */
    void addFiles();

    /**
     * Emitted when the user clicks the link to specify the template.
     */
    void enterTemplate();

private:
    Ui::StartUpInfoWidget *m_widget;
};

#endif // START_UP_INFO_H

