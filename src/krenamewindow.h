/***************************************************************************
                       krenamewindow.h  -  description
                             -------------------
    begin                : Sat Mar 24 2007
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

#ifndef _KRENAMEWINDOW_H_
#define _KRENAMEWINDOW_H_

#include <kmainwindow.h>

class QDialogButtonBox;
class QStackedWidget;

/** KRenameWindow is the main window of KRename.
 *
 *  It can display several pages either as a wizard
 *  or in a tabbed window.
 *
 *  According to the window mode setting, a row of buttons 
 *  is displayed at the bottom of the window.
 */
class KRenameWindow : public KMainWindow {
 public:
    KRenameWindow( bool wizardMode, QWidget* parent = NULL );

 private:
    QStackedWidget*   m_stack;
    QDialogButtonBox* m_buttons;
};

#endif // _KRENAMEWINDOW_H_
