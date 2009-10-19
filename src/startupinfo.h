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

#ifndef START_UP_INFO_H_
#define START_UP_INFO_H_

#include <QFrame>

namespace Ui {
   class StartUpInfoWidget;
};

/**
 * A info widget which tells the user about the next steps
 * when using KRename.
 *
 */
class StartUpInfo : public QFrame {
    Q_OBJECT

public:
    StartUpInfo(QWidget* parent = NULL);
    virtual ~StartUpInfo();

signals:
    /**
     * Emitted when the user clicks the link to add more files.
     */
    void addFiles();

    /**
     * Emitted when the user clicks the link to specify the template.
     */
    void enterTemplate();

private:
    Ui::StartUpInfoWidget* m_widget;
};

#endif // START_UP_INFO_H_

