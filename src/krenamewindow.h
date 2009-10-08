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

#include <QHash>

class KRenameModel;
class KRenamePreviewModel;

class KPushButton;
class QAbstractItemView;
class QComboBox;
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QModelIndex;
class QStackedWidget;
class QTabBar;
class QTreeWidgetItem;

class Plugin;

namespace Ui {
    class KRenameFiles;
    class KRenameDestination;
    class KRenameSimple;
    class KRenamePlugins;
    class KRenameFilename;
};

/** KRenameWindow is the main window of KRename.
 *
 *  According to the window mode setting, a row of buttons 
 *  is displayed at the bottom of the window.
 */
class KRenameWindow : public KMainWindow {
 Q_OBJECT

     friend class KRenameImpl;

 public:
    KRenameWindow( QWidget* parent = NULL );

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


    /**
     * Load UI configuration where necessary.
     */
    void loadConfig();

    /**
     * Save UI configuration where necessary.
     */
    void saveConfig();

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

    /** Set the template for the filename extension.
     *
     *  @param templ the new template
     *  @param insert if true the template string will be inserted into the current
     *         template otherwise it will be replaced
     */
    void setExtensionTemplate( const QString & templ, bool insert = false );

    /** Change the renmae mode 
     *
     *  @param mode the renaming mode to use
     */
    void setRenameMode( ERenameMode mode );

    /** Set the destination url
     * 
     * @param url destination url for eRenameMode_Copy, 
     * eRenameMode_Move and eRenameMode_Link
     */
    void setDestinationUrl( const KUrl & url );

    /** Reset the display of the internal file list
     */
    void resetFileList();

    /** Set the KRename model
     *
     *  @param model a krename model
     */
    void setModel( KRenameModel* model );

    /** Set the preview model
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

    /**
     * \returns a list of all file indexes that are selected
     *          in the preview
     */
    QList<int> selectedFileItemsPreview() const;

    /** 
     * \returns if file preview is enabled in the file tab
     */
    bool isPreviewEnabled() const;

    /**
     * \param bPreview enable/disable file preview in the file tab
     */
    void setPreviewEnabled( bool bPreview );

    /** 
     * \returns if name display is enabled in the file tab
     */
    bool isPreviewNamesEnabled() const;

    /**
     * \param bPreview enable/disable file name display in the file tab
     */
    void setPreviewNamesEnabled( bool bPreview );

    /**
     * \returns the start index for numbers
     */
    int numberStartIndex() const;

    /**
     * \param index start index for numbers
     */
    void setNumberStartIndex( int index );

    /**
     * \returns the current sort mode in the file tab
     */
    int sortMode() const;

    /** 
     * \param sortMode sets the current sort mode in the file tab
     */
    void setSortMode( int sortMode );

    bool isAdvancedMode() const;
    void setAdvancedMode( bool bAdvanced );

    /**
     * Shows the filename tab
     * This might be useful, if the user
     * passed filenames via the commandline
     * and wants to immediately work with the 
     * filename template.
     */
    void showFilenameTab();

 public slots:
    /** Resets the enabled/disabled state of all GUI elements correctly
     */
    void slotEnableControls();

 signals:
    /** Called when the krenamewindow 
     *  needs an update of the file cound
     *
     *  \see setCount 
     */
    void updateCount();

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

    /** Called whenever the user change the splitmode between filename and extension
     *
     *  @param splitMode the mode which is used to split filename and extension
     *  @param dot the n-th dot to use for splitting if splitMode = eSplitMode_CustomDot
     */
    void extensionSplitModeChanged( ESplitMode splitMode, int dot );

    /** Called whenever the user changes the start index
     *
     *  @param index the new start index for numberings
     */
    void startIndexChanged( int index );

    /** Called whenever the user changes the file preview state
     *
     *  @param enable if true display previews 
     *  @param filenames if true display filenames next to preview (if enable is false
     *                   this parameter has to be ignored.
     */
    void filePreviewChanged( bool enable, bool filenames );

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

    /** Called when one of the checkboxes for 
     *  rename, copy, more or link was clicked
     *  to change the rename mode.
     */
    void slotRenameModeChanged();

    /** Called whenever one of the template fields has changed
     */
    void slotTemplateChanged();

    /** Called whenever one of the template fields in wizard mode has changed
     */
    void slotSimpleTemplateChanged();

    /** Emits the showTokenHelpDialog signal with the appropriate
     *  KLineEdit
     */
    void slotTokenHelpRequested();

    /** Emits the showTokenHelpDialog signal with the appropriate
     *  KLineEdit
     */
    void slotTokenHelpRequestedWizard1();

    /** Emits the showTokenHelpDialog signal with the appropriate
     *  KLineEdit
     */
    void slotTokenHelpRequestedWizard2();

    /** Emits the showTokenHelpDialog signal with the appropriate
     *  KLineEdit
     */
    void slotTokenHelpRequestedWizard3();

    /** Emits the showTokenHelpDialog signal with the appropriate
     *  KLineEdit
     */
    void slotTokenHelpRequestedWizard4();

    /** Called when the user selects another extension split mode
     */
    void slotExtensionSplitModeChanged( int index );

    /** Called when one of the preview checkboxes is clicked.
     */
    void slotPreviewChanged();

    /** Called whenever the user changes the current sort mode
     *
     *  @param index currently selected sort index
     */
    void slotSortChanged( int index );

    /** Called whenever the possible maximum value of 
     *  dots in a filename has changed.
     *
     *  @param dots the maximum number of dots in a filename
     *              that can be used to separate fileanem and extension
     */
    void slotMaxDotsChanged( int dots );

    /** Called whenever the user clicks a file
     *  in a listview to open it.
     *
     *  @param index the model index of the requested file in a model
     */
    void slotOpenFile(const QModelIndex& index);

    /** Called when the user wants to move files up
     *  in the file list box.
     */
    void slotMoveUp();

    /** Called when the user wants to move files down
     *  in the file list box.
     */
    void slotMoveDown();

    /** Called when the user wants to move files up
     *  in the preview.
     */
    void slotMoveUpPreview();

    /** Called when the user wants to move files down
     *  in the preview.
     */
    void slotMoveDownPreview();

    /** Called when the user selects a plugin in the plugins tab
     */
    void slotPluginChanged(QTreeWidgetItem* selected);

    /** Called when a plugin is enabled or disabled
     */
    void slotPluginEnabled();

    /** Called when the user changes the start index
     *  in the gui
     */
    void slotSimpleStartIndexChanged();
   
 private:
    /** Configures the GUI for the current GUI mode
     */
    void setupGui();

    /** Setup all signals and slots
     */
    void setupSlots();

    /** Load all plugins
     */
    void setupPlugins();

    /** Set standard KDE icons on UI elements
     *  as this cannot be done from within designer
     */
    void setupIcons();

    /** Get a template for the prefix or suffix in wizard mode
     *  from 2 combo boxes
     *
     *  @param combo the combobox used to select a default prefix/suffix (e.g. date)
     *  @param comboCustom additional user defined text
     *  @returns a template string
     */
    QString getPrefixSuffixSimple( QComboBox* combo, QComboBox* comboCustom );

    /** Get a template for the filename or extension in wizard mode
     *  from 2 combo boxes
     *
     *  @param combo the combobox used to select a default filename (e.g. lowercase)
     *  @param comboCustom additional user defined text
     *  @returns a template string
     */
    QString getFilenameSimple( QComboBox* combo, QComboBox* comboCustom );

    /** Set the GUI elements in the simple filename tab using a template
     *  that has specified in advanced mode
     *
     *  @param filename template of the filename
     *  @param extension template of the extension
     */
    void setSimpleTemplate( const QString & filename, const QString & extension );

    /** Emit signals that the template has been changed in some way by the user
     *
     *  @param filename template of the filename
     *  @param extension template of the extension
     */
    void templatesChanged( const QString & filename, const QString & extension );

    void setPrefixSuffixSimple( QComboBox* combo, QComboBox* comboCustom, const QString & templ );

    void moveUp( const QList<int> & selected, QAbstractItemView* view );
    void moveDown( const QList<int> & selected, QAbstractItemView* view );

    /**
     * \param index (0 or 1) index of the column
     * \returns the column width of column index
     */
    int previewColumnWidth( int index );

    /**
     * Set the width of the specified colum
     * \param index (0 or 1) index of the column
     * \param width width in pixels
     */
    void setPreviewColumnWidth( int index, int width );

    void blockSignalsRecursive( QObject* obj, bool b );

 private:
    int               m_curPage;   /// The index of the current page in the current gui mode
    const TGuiMode*   m_guiMode;   /// The description structure of the current gui mode
    int               m_fileCount; /// Current number of files; used for enabled disabled state

    QStackedWidget*   m_stack;
    QDialogButtonBox* m_buttons;

    QTabBar*          m_tabBar;    /// The tabbar to switch pages in advanced mode

    KPushButton*      m_buttonClose;
    KPushButton*      m_buttonFinish;

    Ui::KRenameFiles*       m_pageFiles;
    Ui::KRenameDestination* m_pageDests;
    //Ui::KRenameSimple*      m_pageSimple;
    Ui::KRenamePlugins*     m_pagePlugins;
    Ui::KRenameFilename*    m_pageFilename;

    QHash<QString,QWidget*> m_pluginsWidgetHash;
    QHash<QString,Plugin*>  m_pluginsHash;
};


#endif // _KRENAMEWINDOW_H_
