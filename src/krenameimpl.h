/***************************************************************************
                          krenameimpl.h  -  description
                             -------------------
    begin                : Die Mai 20 2003
    copyright            : (C) 2003 by Dominik Seichter
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

#ifndef KRENAMEIMPL_H
#define KRENAMEIMPL_H

#include <QObject>
#include <QMutex>

#include <kurl.h>

#include "batchrenamer.h"
#include "krenamefile.h"

class KRenameModel;
class KRenamePreviewModel;
class KRenameWindow;
class ThreadedLister;
class PluginLoader;
class QLineEdit;

class KRenameImpl : public QObject {
    Q_OBJECT

 public: 
    ~KRenameImpl();

    static QWidget* launch( const QRect & rect, const KRenameFile::List & list, bool loadprofile = true );

 private:
    KRenameImpl( KRenameWindow* window, const KRenameFile::List & list );

    /** Creates all menu items and actions.
     */
    void setupActions();

    /** Connect all gui components with their slots
     */
    void setupSlots();

    /** Adds a single url to the list of files which will 
     *  be renamed.
     *
     *  If you have a KUrl::List use addFilesOrDirs which is 
     *  faster.
     *
     *  \param url must be an existing file or directory.
     *
     *  \see addFilesOrDirs
     */
    void addFileOrDir( const KUrl & url );

    /** Adds a list of urls to the list of files which will 
     *  be renamed.
     *
     *  Adding a whole list is much faster than adding single
     *  files.
     *
     *  \param list of existing files or directories
     *  \param filter only add files or directories matching this filter
     *  \param recursively if true all directories will be added recursively
     *  \param dirsWithFiles add directory names along with their contents
     *  \param dirsOnly add only directories and no files
     *  \param hidden add also hidden files and directories
     *
     *  \see addFileOrDir
     */
    void addFilesOrDirs( const KUrl::List & list, const QString & filter = "*", 
                         bool recursively = false, bool dirsWithFiles = false, 
                         bool dirsOnly = false, bool hidden = false );

    /** Parses commandline options
     */
    void parseCmdLineOptions();

 private slots:

    /** Start KRename's selftest:
     *  I.e several unit tests.
     */
    void selfTest();

    /** Called when the user clicks the "Add..." button.
     *  open a dialog to select files for adding to KRename.
     */
    void slotAddFiles();

    /** Called when the user clicks the "Remove" buttons.
     *  Remove selected files from the list.
     */
    void slotRemoveFiles();

    /** Called when the user clicks the "Remove All..." buttons.
     *  Remove all files from the list.
     */
    void slotRemoveAllFiles();

    /** Updates the count of files 
     *  in the file list of KRename
     */
    void slotUpdateCount();

    /** Updates the preview of the filenames after renaming
     */
    void slotUpdatePreview();

    /** This slot shows a dialog with advanced numbering settings
     *  - start index
     *  - stepping
     *  - reset counter on each new directory
     *  - skip numbers list
     *
     */
    void slotAdvancedNumberingDlg();

    /** This slots shows a dialog which allows the user
     *  to confortably insert a part of an oldfilename into the 
     *  new filename (part is to be understood as "substring")
     */
    void slotInsertPartFilenameDlg();

    /** This slot shows a dialog which allows the user to configure find and replace
     */
    void slotFindReplaceDlg();

    /** Start the actual renaming
     *  with the current settings
     */
    void slotStart();

    /** Clean's up and deletes the lister after he has done
     *  all his work.
     *
     *  \param lister a ThreadedLister which has done his work
     */
    void slotListerDone( ThreadedLister* lister );

    /** Show a TokenHelpDialog that works on a QLineEdit
     *
     *  \param edit insert user selected tokens into this linedit
     */
    void slotTokenHelpDialog(QLineEdit* edit);

    /** Called when the user selects another extension split mode
     *
     *  @param splitMode the mode which is used to split filename and extension
     *  @param dot the n-th dot to use for splitting if splitMode = eSplitMode_CustomDot
     */
    void slotExtensionSplitModeChanged( ESplitMode splitMode, int dot );

 private:
    KRenameWindow*        m_window;
    KRenameModel*         m_model;
    KRenamePreviewModel*  m_previewModel;
    
    KRenameFile::List     m_vector;

    BatchRenamer          m_renamer;

    ESplitMode            m_lastSplitMode; ///< The last used split mode
    int                   m_lastDot;       ///< The last used dot value for splitmode

    PluginLoader*         m_pluginLoader;  ///< Global plugin loader instance
};

#if 0 
// Own includes
/*
#include "batchrenamer.h"
#include "krenamedcop.h"
*/
class HelpDialog;
class HelpDialogData;
class KAction;
class KComboBox;
class KJanusWidget;
class KMyHistoryCombo;
class KMyListBox;
class KMyListView;
class KMenuBar;
class KPopupMenu;
class KPushButton;
class KToggleAction;
class KURL;
class KURLRequester;
class KIntNumInput;
class MyHelpCombo;
class MyLabel;
class Plugin;
class PluginLoader;
class QButtonGroup;
class QCheckBox;
class QGroupBox;
class QFileInfo;
class QFrame;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QListView;
class QPushButton;
class QRect;
class QWidgetStack;
class QRadioButton;
class QVBoxLayout;
class QVGroupBox;
class QWidget;

#include <kguiitem.h>
KPushButton* createButton( KGuiItem item, QWidget* parent );

class KRenameImpl : public QObject, public KRenameDCOP {
    Q_OBJECT
    
    friend class ProfileManager;
    friend class ProfileDlg;
    friend class tabs;

    public: 
        KRenameImpl( QWidget* p, KMenuBar* m, QPushButton* finish );
        ~KRenameImpl();

        void setWizardMode( bool mode );
        
        /*
         * public because both get called from KRenameImpl::launch()
         */
        void updatePre();
        void addFileOrDir( const KURL & name );

        void setup( bool wizardmode );
        void changeParent( QWidget* p, KMenuBar* m, QPushButton* finish, QRect r );
        static QWidget* launch( QRect rect, const KURL::List & list, KRenameImpl* impl = 0, bool loadprofile = true );

        /** DCOP functions we have to implement
          */
        void addFileOrDir( const QString & name );
        const QString fileNameTemplate() const;
        inline int counterStart() const;
  
        void setExtensionTemplate( const QString & t );
        const QString extensionTemplate() const;

        void setUseExtension( bool b );
        bool useExtension() const;
        
        QStringList tokens() const;

        QString parseString( const QString & token, const QString & string );
        void addDir( const QString & name, const QString & filter, bool recursive, bool hidden, bool dirnames );

        const QString title( int index ) const;
        
        /** @returns true if the user has specified a profile on the commandline
         *           other wise false is returned
         */
        inline bool hasCommandlineProfile() const;

    public slots:
        void setFileNameTemplate( const QString & t );
        void setCounterStart( int index );
                
    signals:
        void pageDone( QWidget* page, const QString & title );
        void showPage( int page );
        void enableFinish( bool b );

    private slots:
        void about();
        void addFile();
        bool checkErrors();
        void clearList();
        void enableControls();
        void toggleImagePreview();
        void moveUp();
        void moveDown();
        void help();
        void removeFile();
        void removeFile( int index );
        void preferences();
        void replace();
        void saveConfig();
        void start();
        void updateCount();
        void undo();
        void changeUndoScriptName();
        void pluginHelpChanged();
        void getCoordinates();
        void changeNumbers();
        void updateDots();
        void updatePreview();
        void showTokenHelp();
        void toggleName();
        void changed();
        void loadFilePlugins();
        void reloadFilePluginData();
        void manageProfiles();
        QString easyOptions( KComboBox* combo, KMyHistoryCombo* custom );
        void slotEasy1();
        void slotEasy2();
        void slotEasy3();
        void slotEasy4();

        /** Update the preview only if the passed
         *  plugin is enabled for use.
         */
        void updatePluginPreview( Plugin* p );

    private:
        static int numRealTimePreview;

        /** Change the GUI mode according to the current setting of m_wizard
          */
        void changeGUIMode();
        
        /** Returns COPY if optionCopy is checked, RENAME if optionRename is checked
          * ...
          */
        int currentRenameMode();
        
        void loadConfig();
        void fillStructures( BatchRenamer* b, bool preview );
        bool setupBatchRenamer( BatchRenamer* b, bool preview );
        void splitFilename( QFileInfo* fi, QString* base, QString* extension );

        void setupActions();
        void setupPage1();
        void setupPage2();
        void setupPage3();
        void setupPage4();
        void setupTab1();
        void setupFileTab1();
        void setupFileTab2();
        void setupPages();
        void updateHist();
        void parseCommandline();
        void addTitle( QWidget* p, QVBoxLayout* layout, QString title );

        QValueList<manualchanges> changes;
        void refreshColumnMode();
        void parseWizardMode();
        void getHelpDialogString( QLineEdit* edit );
        
    protected:
        QWidget* parent;
        KMenuBar* menuBar;
        KPopupMenu* mnuSort;
        KAction* loadPlugins;
        
        QPushButton* finishButton;

        PluginLoader* plugin;

        QWidgetStack* fileTab;
        
        QWidget* page_1;
        QWidget* page_2;
        KJanusWidget* page_3;
        QWidget* page_4;

        bool m_wizard;
        bool m_loadplugins;
        bool m_switching;
        bool m_autosize;
        int m_hist;
        int m_index;
        int m_step;
	bool m_reset;

        bool m_hasCommandlineProfile;

        KPushButton* buttonUp;
        KPushButton* buttonDown;
        KPushButton* buttonUp2;
        KPushButton* buttonDown2;
        KPushButton* buttonAdd;
        KPushButton* buttonRemove;
        KPushButton* buttonRemoveAll;
        KPushButton* buttonReplace;
        KPushButton* buttonEasyReplace;
        KPushButton* buttonHelp;
        KPushButton* buttonMove;
        KPushButton* buttonCoord;
        KPushButton* buttonNumber;
        
        KPushButton* buttonEasy1;
        KPushButton* buttonEasy2;
        KPushButton* buttonEasy3;
        KPushButton* buttonEasy4;
        KComboBox* comboSort;
        
        QLabel* description;
        QLabel* description2;
        QLabel* description3;
        QLabel* description4;

        QLabel* labelTemplate;
        QLabel* labelHelp;
        QLabel* labelCount;
        QLabel* labelPoint;

        KMyListBox* fileList;
        KMyListView* preview;
        QButtonGroup* groupOptions;

        QRadioButton* optionCopy;
        QRadioButton* optionMove;
        QRadioButton* optionRename;
        QRadioButton* optionLink;

        QGroupBox* groupExtension;
        QVGroupBox* groupUndo;

        KMyHistoryCombo* dirname;
        KURLRequester* urlrequester;
        KURLRequester* undorequester;
        KMyHistoryCombo* filename;
        KMyHistoryCombo* extemplate;

        QCheckBox* checkName;
        QCheckBox* checkExtension;
        QCheckBox* checkOverwrite;
        QCheckBox* checkPreview;
        QCheckBox* checkUndoScript;

        KComboBox* comboExtension;

        MyHelpCombo* comboHelp;
        HelpDialog* helpDialog;
        HelpDialogData* helpDialogData;

        BatchRenamer* b;
        QValueList<int> skip;
        QValueList<replacestrings> rep;

        // ==========
        // Easy mode:
        // ==========
        
        KComboBox* comboKRenamePrefix;
        KComboBox* comboKRenameSuffix;
        KComboBox* comboKRenameFilename;
        KComboBox* comboKRenameExtension;
        
        KMyHistoryCombo* comboPrefix;
        KMyHistoryCombo* comboSuffix;        
        KMyHistoryCombo* comboCustom;
        KMyHistoryCombo* comboCustomExtension;
        
        KIntNumInput* spinStart;
        KIntNumInput* spinNull;        
        
        // ===========
        // Layout:
        // ===========

        QHBoxLayout* pageLayout;
        QVBoxLayout* pageLayout_2;
        QVBoxLayout* pageLayout_3;
        QVBoxLayout* pageLayout_4;

        QHBoxLayout* tabLayout_0;
        QHBoxLayout* tabLayout_1;
        QHBoxLayout* tabLayout_2;
        QVBoxLayout* tabLayout_3;
        QVBoxLayout* groupAdvancedExtensionLayout;
        QVBoxLayout* groupOptionsLayout;
        QHBoxLayout* groupDirLayout;
        QVBoxLayout* groupNumberLayout;
        QHBoxLayout* groupExtensionLayout;

        // page1
        QHBoxLayout* Layout2;
        QVBoxLayout* Layout3;
        QHBoxLayout* Layout4;
        QVBoxLayout* Layout5;

        // page4
        QVBoxLayout* Layout10;
        QHBoxLayout* Layout15;
        QVBoxLayout* Layout16;
        QHBoxLayout* Layout22;
        QVBoxLayout* Layout23;

        // tab
        QHBoxLayout* Layout100;
        QHBoxLayout* Layout101;
};


int KRenameImpl::counterStart() const
{
    return m_index;
}

bool KRenameImpl::hasCommandlineProfile() const
{
    return m_hasCommandlineProfile;
}
#endif // 0
#endif
