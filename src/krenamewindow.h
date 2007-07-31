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
#include "batchrenamer.h"

class KRenameModel;
class KRenamePreviewModel;

class KPushButton;
class QDialogButtonBox;
class QLabel;
class QLineEdit;
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

    ~KRenameWindow();

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
     *  @param count typically the number of files in the list
     */
    void setCount( unsigned int count );

    /** Set the template for the filename.
     *
     *  @param templ the new template
     *  @param insert if true the template string will be inserted into the current
     *         template otherwise it will be replaced
     */
    void setFilenameTemplate( const QString & templ, bool insert = false );

    /** Reset the display of the internal file list
     */
    void resetFileList();

    /** Set the KRename model
     *
     *  @param model a krename model
     */
    void setModel( KRenameModel* model );

    /** Set the preview mode
     *
     * @param model a preview model
     */
    void setPreviewModel( KRenamePreviewModel* model );

    /** 
     * @returns the destinationUrl selected by the user
     */
    const KUrl destinationUrl() const;

    /**
     * \returns a list of all file indexes that are selected
     *          in the filelist
     */
    QList<int> selectedFileItems() const;

 signals:

    /** Called whenever the user changes the rename mode
     *
     *  @param mode the renaming mode selected by the user
     */
    void renameModeChanged( ERenameMode mode );

    /** Called whenever the user changes the option 
     *  if existing files maybe overwritten
     *
     *  @param bool overwrite if true existing files maybe overwritten during renaming
     */
    void overwriteFilesChanged(bool overwrite);

    /** Called whenever the user has changed the template for the filename
     *
     *  @param filename the new template for the filename
     */
    void filenameTemplateChanged( const QString & filename );

    /** Called whenever the user has changed the template for the extension
     *
     *  @param filename the new template for the extension
     */
    void extensionTemplateChanged( const QString & extension );

    /** Called whenever the GUI wants an updated preview
     */
    void updatePreview();

    /** Called when the user requests the advanced numbering dialog
     */
    void showAdvancedNumberingDialog();

    /** Called when the user requests the insert part filename dialog
     */
    void showInsertPartFilenameDialog();

    /** Called when the user requests the find and repalce dialog
     */
    void showFindReplaceDialog();

    /** Called when the user wants to add more files
     */
    void addFiles();

    /** Called when the user wants to remove all files
     */
    void removeAllFiles();

    /** Called when the user wants to remove all selected files
     *
     *  \see selectedFileItems() to retrieve the selected files
     */
    void removeFiles();
    
    /** Show a token help dialog which inserts a token
     *  into a KLineEdit
     *
     *  \param edit the QLineEdit which is used to insert tokens selected by the user
     */
    void showTokenHelpDialog( QLineEdit* edit );

    /** Called when the user wants to rename with current settings
     */
    void accepted();

 private slots:
    void slotBack();
    void slotNext();
    void slotFinish();

    /** Changes the current page to the page with the given index
     * 
     *  @param index the index of the page to show
     */
    void showPage( int index );

    /** Resets the enabled/disabled state of all GUI elements correctly
     */
    void slotEnableControls();

    /** Called when one of the checkboxes for 
     *  rename, copy, more or link was clicked
     *  to change the rename mode.
     */
    void slotRenameModeChanged();

    /** Called whenever one of the template fields has changed
     */
    void slotTemplateChanged();

    /** Emits the showTokenHelpDialog signal with the appropriate
     *  KLineEdi
     */
    void slotTokenHelpRequested();

 private:
    /** Configures the GUI for the current GUI mode
     */
    void setupGui();

    /** Setup all signals and slots
     */
    void setupSlots();

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
