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
    class KRenameSimple;
    class KRenamePlugins;
    class KRenameFilename;
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

     friend class KRenameImpl;

 public:
    KRenameWindow( EGuiMode guiMode, QWidget* parent = NULL );

    /** The maximum number of pages in any gui mode.
     *  No gui mode can have more pages than this.
     */
    static const int MAX_PAGES = 4;

    /** This structure is used to describe a gui mode.
     *  It is required to map indexes to the right indexes
     *  of the widget stack and to retrieve the total number
     *  of pages per mode as well as the individual page titles.
     */
    typedef struct {
        const int   numPages;
        const char* pageTitles[KRenameWindow::MAX_PAGES];
        const int   mapIndex[KRenameWindow::MAX_PAGES];
    } TGuiMode;

    /** Set the count displayed on the files tab of KRename
     *  \param count typically the number of files in the list
     */
    void setCount( unsigned int count );

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
    int               m_curPage;   /// The index of the current page in the current gui mode
    const TGuiMode*   m_guiMode;   /// The description structure of the current gui mode
    int               m_fileCount; /// Current number of files; used for enabled disabled state

    QStackedWidget*   m_stackTop;  /// Contains a title label in wizard mode
                                   /// and a tabbar in advanced mode
    QStackedWidget*   m_stack;
    QDialogButtonBox* m_buttons;

    QLabel*           m_lblTitle;  /// The title label in wizard mode
    QLabel*           m_lblStep;   /// The current step in wizard mode
    QTabBar*          m_tabBar;    /// The tabbar to switch pages in advanced mode

    KPushButton*      m_buttonBack;
    KPushButton*      m_buttonNext;
    KPushButton*      m_buttonClose;
    KPushButton*      m_buttonFinish;

    Ui::KRenameFiles*       m_pageFiles;
    Ui::KRenameDestination* m_pageDests;
    Ui::KRenameSimple*      m_pageSimple;
    Ui::KRenamePlugins*     m_pagePlugins;
    Ui::KRenameFilename*    m_pageFilename;
};


#endif // _KRENAMEWINDOW_H_
