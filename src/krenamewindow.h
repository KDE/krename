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

class KPushButton;
class QDialogButtonBox;
class QLabel;
class QStackedWidget;
class QTabBar;

namespace Ui {
    class KRenameFiles;
    class KRenameDestination;
};

/** This enum specifies the available GUI modes for KRename
 */
enum EGuiMode {
    eGuiMode_Wizard,
    eGuiMode_Advanced
};

/** KRenameWindow is the main window of KRename.
 *
 *  It can display several pages either as a wizard
 *  or in a tabbed window.
 *
 *  According to the window mode setting, a row of buttons 
 *  is displayed at the bottom of the window.
 */
class KRenameWindow : public KMainWindow {
 Q_OBJECT

 public:
    KRenameWindow( EGuiMode guiMode, QWidget* parent = NULL );

 private slots:
    void slotBack();
    void slotNext();

    /** Changes the current page to the page with the given index
     * 
     *  \param index the index of the page to show
     */
    void showPage( int index );

 private:
    /** Configures the GUI for the current GUI mode
     */
    void setupGui();

    /** Resets the enabled/disabled state of all GUI elements correctly
     */
    void enableControls();

 private:
    EGuiMode          m_eGuiMode;  /// The current gui mode
    int               m_curPage;   /// The index of the current page

    QStackedWidget*   m_stackTop;  /// Contains a title label in wizard mode
                                   /// and a tabbar in advanced mode
    QStackedWidget*   m_stack;
    QDialogButtonBox* m_buttons;

    QLabel*           m_lblTitle;  /// The title label in wizard mode
    QTabBar*          m_tabBar;    /// The tabbar to switch pages in advanced mode

    KPushButton*      m_buttonBack;
    KPushButton*      m_buttonNext;
    KPushButton*      m_buttonClose;

    Ui::KRenameFiles*       m_pageFiles;
    Ui::KRenameDestination* m_pageDests;

};

#endif // _KRENAMEWINDOW_H_
