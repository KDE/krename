/***************************************************************************
                          krenameimpl.cpp  -  description
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

#include "krenameimpl.h"
#include "krenameimpl.moc"
#include "filedialogextwidget.h"
#include "krenamemodel.h"
#include "krenametest.h"
#include "krenamewindow.h"

#include "numberdialog.h"
#include "insertpartfilenamedlg.h"
#include "plugin.h"
#include "pluginloader.h"
#include "progressdialog.h"
#include "replacedialog.h"
#include "threadedlister.h"
#include "tokenhelpdialog.h"

#include "modeltest.h"

#include <kaction.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <kmenu.h>
#include <kmenubar.h>
#include <kmessagebox.h>
#include <kstandardaction.h>

#include <kio/netaccess.h>

#include <QStringListModel>
#include <QTimer>

KRenameImpl::KRenameImpl( KRenameWindow* window, const KRenameFile::List & list )
    : QObject( (QObject*)window ), m_window( window ),
      m_lastSplitMode( eSplitMode_FirstDot ), m_lastDot( 0 )
{
    setupActions();
    setupSlots();

    m_model = new KRenameModel( &m_vector );
    m_window->setModel( m_model );

    connect( m_model,  SIGNAL(filesDropped()),   SLOT(slotUpdateCount()));

    m_previewModel = new KRenamePreviewModel( &m_vector );
    m_window->setPreviewModel( m_previewModel );

    m_renamer.setFiles( &m_vector );
    m_model->addFiles( list );

    m_pluginLoader = PluginLoader::Instance();
    m_pluginLoader->registerForUpdates( this );

    loadConfig();

    parseCmdLineOptions();
    slotUpdateCount();

    connect( kapp, SIGNAL( aboutToQuit() ), this, SLOT( saveConfig() ) );
}

KRenameImpl::~KRenameImpl()
{
    m_pluginLoader->deregisterForUpdates( this );
    delete m_model;
}

QWidget* KRenameImpl::launch( const QRect & rect, const KRenameFile::List & list, bool loadprofile )
{
    KSharedConfigPtr config = KGlobal::config();

    KConfigGroup groupGui = config->group( QString("GUISettings") );
    bool firststart  = groupGui.readEntry( "firststart4", QVariant(true) ).toBool();

    if( firststart ) 
    {
        // WELCOME TO KRENAME
    }

    KRenameWindow* w  = new KRenameWindow( NULL );
    KRenameImpl* impl = new KRenameImpl( w, list );
	// Windows shows KRename otherwise outside of the visible
	// screen area
	if( !rect.isNull() )
		w->setGeometry( rect );

    /*
    // it is time to load a default profile now (if the user has specified one)
    if( loadprofile && !k->hasCommandlineProfile() && ProfileManager::hasDefaultProfile() )
	ProfileManager::loadDefaultProfile( k );
    else if ( !k->hasCommandlineProfile() )
        w->show();
    */

    w->show();

    return w;
}

void KRenameImpl::setupActions()
{
    KMenu* mnuExtra    = new KMenu( i18n("E&xtras"), m_window );

    KMenu* mnuHelp     = m_window->helpMenu( QString::null, true );

    KAction* actProfiles = new KAction( i18n("&Profiles..."), m_window );

    m_window->menuBar()->addMenu( mnuExtra );
    m_window->menuBar()->addMenu( mnuHelp );

    mnuExtra->addAction( actProfiles );

    connect(actProfiles, SIGNAL(triggered(bool)), SLOT(slotManageProfiles()));
}

void KRenameImpl::setupSlots()
{
    connect( m_window, SIGNAL(addFiles()),       SLOT(slotAddFiles()));
    connect( m_window, SIGNAL(removeFiles()),    SLOT(slotRemoveFiles()));
    connect( m_window, SIGNAL(removeAllFiles()), SLOT(slotRemoveAllFiles()));

    connect( m_window, SIGNAL(updatePreview()),  SLOT(slotUpdatePreview()));

    connect( m_window, SIGNAL(accepted()),       SLOT(slotStart()));

    QObject::connect( m_window, SIGNAL(renameModeChanged(ERenameMode)), &m_renamer, SLOT(setRenameMode(ERenameMode)));
    QObject::connect( m_window, SIGNAL(filenameTemplateChanged(const QString &)), 
                      &m_renamer, SLOT(setFilenameTemplate(const QString &)));
    QObject::connect( m_window, SIGNAL(extensionTemplateChanged(const QString &)), 
                      &m_renamer, SLOT(setExtensionTemplate(const QString &)));
    QObject::connect( m_window, SIGNAL(overwriteFilesChanged(bool)), 
                      &m_renamer, SLOT(setOverwriteExistingFiles(bool)));

    QObject::connect( m_window, SIGNAL(startIndexChanged(int)), 
                      &m_renamer, SLOT(setNumberStartIndex(int)));

    connect( m_window, SIGNAL(extensionSplitModeChanged(ESplitMode,int)), SLOT(slotExtensionSplitModeChanged(ESplitMode,int)));

    connect( m_window, SIGNAL(showAdvancedNumberingDialog()),  SLOT(slotAdvancedNumberingDlg()));
    connect( m_window, SIGNAL(showInsertPartFilenameDialog()), SLOT(slotInsertPartFilenameDlg()));
    connect( m_window, SIGNAL(showFindReplaceDialog()),        SLOT(slotFindReplaceDlg()));
    connect( m_window, SIGNAL(showTokenHelpDialog(QLineEdit*)),SLOT(slotTokenHelpDialog(QLineEdit*)));
}

void KRenameImpl::addFileOrDir( const KUrl & url )
{
    KRenameFile       item( url );
    KRenameFile::List list;

    list.append( item );

    m_model->addFiles( list );

    this->slotUpdateCount();
}

void KRenameImpl::addFilesOrDirs( const KUrl::List & list, const QString & filter, 
                                  bool recursively, bool dirsWithFiles, bool dirsOnly, bool hidden )
{
    KUrl::List::ConstIterator it   = list.begin();
    
    while( it != list.end() )
    {
	qDebug("Number of items: %s", (*it).prettyUrl().toUtf8().data() );

        KRenameFile item( *it );
        if( item.isDirectory() )
        {
            KApplication::setOverrideCursor( Qt::BusyCursor );

            ThreadedLister* thl = new ThreadedLister( *it, m_window, m_model );
            connect( thl, SIGNAL( listerDone( ThreadedLister* ) ), SLOT( slotListerDone( ThreadedLister* ) ) );
            
            thl->setFilter( filter );
            thl->setListDirnamesOnly( dirsOnly );
            thl->setListHidden( hidden );
            thl->setListRecursively( recursively );
            thl->setListDirnames( dirsWithFiles );

            thl->start();
        }
        else 
        {
            if( !dirsOnly ) 
            {
                KRenameFile::List list;
                list.append( item );

                m_model->addFiles( list );
            }
        }

        ++it;
    }

    this->slotUpdateCount();
}

void KRenameImpl::parseCmdLineOptions()
{
    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();

    if( args->isSet( "test" ) )
        QTimer::singleShot( 0, this, SLOT( selfTest() ) );

    /*
    QCStringList optlist = args->getOptionList ( "r" );
    for (QCStringList::ConstIterator it=optlist.begin(); it!=optlist.end(); ++it)
    {

        KURL url;
        url.setPath( *it );
        fileList->addDir( url, "*", false, true, false );
    }
    */

    // Add all files from the commandline options
    KUrl::List list;
    for( int i = 0; i < args->count(); i++)
        list.append( args->url( i ) );

    this->addFilesOrDirs( list );

    /*
    // load the profile first, so that we do not overwrite other
    // commandline settings
    QCString templ = args->getOption( "profile" );
    if( !templ.isEmpty() )
    {
        m_hasCommandlineProfile = true;
        ProfileManager::loadProfile( QString( templ ), this );
    }

    if( !args->isSet( "previewitems" ) )
        numRealTimePreview = -1;
    else
        numRealTimePreview = QString( args->getOption( "previewitems" ) ).toInt();

    templ = args->getOption( "template" );
    if( !templ.isEmpty() )
        filename->setText( templ );

    templ = args->getOption( "extension" );
    if( !templ.isEmpty() ) 
    {
        extemplate->setText( templ );
        checkExtension->setChecked( false );
    }

    templ = args->getOption( "copy" );
    if( !templ.isEmpty() ) 
    {
        urlrequester->setURL( templ );
        optionCopy->setChecked( true );
    }

    templ = args->getOption( "move" );
    if( !templ.isEmpty() ) 
    {
        urlrequester->setURL( templ );
        optionMove->setChecked( true );
    }
        
    QCStringList uselist = args->getOptionList ( "use-plugin" );
    if( !uselist.isEmpty() ) 
    {
        for(unsigned int i = 0; i < uselist.count(); i++ )
            uselist[i] = uselist[i].lower();
    
        QPtrListIterator<PluginLoader::PluginLibrary> it( plugin->libs );
        while ( it.current() ) 
        {
            if( uselist.contains( (*it)->plugin->getName().lower().utf8() ) )
                (*it)->check->setChecked( true );
                
            ++it;
        }

        pluginHelpChanged();
    }
    
    bool startnow = args->isSet( "start" );
    
    // Free some memory
    args->clear();      

    enableControls();
    updateCount();
    updatePreview();
    
    if( fileList->count() )
    {
        // we got already filenames over the commandline, so show directly the last
        // page of the wizard
        emit showPage( m_wizard ? 3 : 4 );
    }

    if( startnow ) 
    {
        // As file adding runs in a another trhread,
        // there might be adding in progress but not yet
        // all files in the list.
        // so let's wait for file adding to finish first
        // before starting.
        while( fileList->runningAddListeners() > 0 )
            kapp->processEvents();

        if( fileList->count() ) 
            // start renaming
            QTimer::singleShot( 200, this, SLOT( start() ) );
    }
    */
}


void KRenameImpl::slotAddFiles()
{
    FileDialogExtWidget* widget = new FileDialogExtWidget();
    KFileDialog dialog( KUrl("kfiledialog://krename"), i18n("*|All files and directories"), 
			m_window, widget );
    dialog.setOperationMode( KFileDialog::Opening );
    dialog.setMode( KFile::File | KFile::Files | KFile::Directory | KFile::ExistingOnly );

    if( dialog.exec() == QDialog::Accepted ) 
    {
	qDebug("Number of items: %i", dialog.selectedUrls().count() );
        this->addFilesOrDirs( dialog.selectedUrls(), dialog.currentFilter(), 
			      widget->addRecursively(), widget->addDirsWithFiles(),
                              widget->addDirsOnly(), widget->addHidden() );
    }
}

void KRenameImpl::slotRemoveFiles()
{
    if( m_window->selectedFileItems().count() )
    {
        m_model->removeFiles( m_window->selectedFileItems() );
        this->slotUpdateCount();
    }
}

void KRenameImpl::slotRemoveAllFiles()
{
    // TODO: Show message box: Do you really want to remove all files.
    if( KMessageBox::questionYesNo( m_window, i18n("Do you really want to remove all files from the list?"),
                                    i18n("KRename"), KStandardGuiItem::yes(), KStandardGuiItem::no(),
                                    "KRenameRemoveAllFromFileList" ) == KMessageBox::Yes )
    {
        m_vector.clear();
        m_window->resetFileList();


        this->slotUpdateCount();
    }
}

void KRenameImpl::selfTest()
{
    KRenameTest* test = new KRenameTest();
    test->startTest();    
    
    new ModelTest( m_model );
    //new ModelTest( m_previewModel );

    // Make _really_ sure it comes to front
    test->show();
    test->raise();
    test->activateWindow();
}

void KRenameImpl::slotUpdateCount()
{
    m_window->setCount( m_vector.size() );
    m_window->slotEnableControls();

    this->slotUpdatePreview();
}

void KRenameImpl::slotUpdatePreview()
{
    KApplication::setOverrideCursor( Qt::WaitCursor );
    m_renamer.processFilenames();
    KApplication::restoreOverrideCursor();

    m_previewModel->refresh();
    //m_window->m_pageSimple->listPreview->reset();
}

void KRenameImpl::slotAdvancedNumberingDlg()
{
    NumberDialog dialog( m_renamer.numberStartIndex(), m_renamer.numberStepping(), 
                         m_renamer.numberReset(), m_renamer.numberSkipList() ,m_window );
    if( dialog.exec() == QDialog::Accepted ) 
    {
        m_renamer.setNumberStartIndex( dialog.startIndex() );
        m_renamer.setNumberStepping( dialog.numberStepping() );
        m_renamer.setNumberReset( dialog.resetCounter() );
        m_renamer.setNumberSkipList( dialog.skipNumbers() );

	m_window->setNumberStartIndex( dialog.startIndex() );

        slotUpdatePreview();
    }
}

void KRenameImpl::slotInsertPartFilenameDlg()
{
    InsertPartFilenameDlg dialog( m_vector.first().srcFilename() );

    if( dialog.exec() == QDialog::Accepted ) 
    {
        m_window->setFilenameTemplate( dialog.command(), true );

        // Update preview will called from KRenameWindow because of the changed template
        // slotUpdatePreview();s
    }
}

void KRenameImpl::slotFindReplaceDlg()
{
    ReplaceDialog dialog( m_renamer.replaceList(), m_window );

    if( dialog.exec() == QDialog::Accepted ) 
    {
        m_renamer.setReplaceList( dialog.replaceList() );
        slotUpdatePreview();
    }
}

void KRenameImpl::slotListerDone( ThreadedLister* lister ) 
{
    // Delete the listener
    delete lister;

    // restore cursor
    KApplication::restoreOverrideCursor();

    // update preview
    slotUpdateCount();
    slotUpdatePreview();
} 

void KRenameImpl::slotTokenHelpDialog(QLineEdit* edit)
{
    TokenHelpDialog dialog( edit, m_window );

    // add built-in tokens
    QStringList help;
    help.append("$;;" + i18n("old filename") );
    help.append("%;;" + i18n("old filename converted to lower case") );
    help.append("&;;" + i18n("old filename converted to upper case") );
    help.append("*;;" + i18n("first letter of every word upper case") );
    help.append("[&1][%2-];;" + i18n("first letter of filename upper case") );
    help.append("#;;" + i18n("number (try also ##, ###, ... for leading zeros)") );
    help.append("#{0;1};;" + i18n("counter with custom start value 0 and custom stepping 1") );
    help.append("/;;" + i18n("create a subdirectory") );
    help.append("[$x-y];;" + i18n("character x to y of old filename") );
    help.append("[$x;y];;" + i18n("y characters of old filename starting at x") );
    help.append("[$dirname];;" + i18n("insert name of directory") );
    help.append("[$dirname.];;" + i18n("insert name of parent directory") );
    help.append("[#length-0];;" + i18n("insert the length of the input filename") );
    help.append("[trimmed];;" + i18n("strip whitespaces leading and trailing") );
    help.append("[trimmed;.*];;" + i18n("strip whitespaces leading and trailing of an arbitrary string") );
    dialog.add( i18n("Built-in Functions" ), help, SmallIcon("krename"), true );

    help.clear();
    help.append( "\\$;;" + i18n("Insert '$'") );
    help.append( "\\%;;" + i18n("Insert '%'") );
    help.append( "\\&;;" + i18n("Insert '&'") );
    help.append( "\\*;;" + i18n("Insert '*'") );
    help.append( "\\/;;" + i18n("Insert '/'") );
    help.append( "\\\\;;" + i18n("Insert '\\\\'") );
    help.append( "\\[;;" + i18n("Insert '['") );
    help.append( "\\];;" + i18n("Insert ']'") );
    help.append( "\\#;;" + i18n("Insert '#'") );
    dialog.add( i18n("Special Characters" ), help, SmallIcon("krename") );

    // add plugin tokens
    QList<Plugin*>::const_iterator it = m_pluginLoader->plugins().begin();
    while( it != m_pluginLoader->plugins().end() )
    {
        help.clear();
        help = (*it)->help();
        if( !help.isEmpty() )
            dialog.add( (*it)->name(), help, (*it)->icon() );

        ++it;
    }

    dialog.exec();
}

void KRenameImpl::slotExtensionSplitModeChanged( ESplitMode splitMode, int dot )
{
    // only change the splitMode if it has really change since the last time
    if( splitMode != m_lastSplitMode ||
        dot != m_lastDot ) 
    {
        KRenameFile::List::iterator it = m_vector.begin();
    
        while( it != m_vector.end() )
        {
            (*it).setCurrentSplitMode( splitMode, dot );
            ++it;
        }

        slotUpdatePreview();
    }

    m_lastSplitMode = splitMode;
    m_lastDot       = dot;
}

void KRenameImpl::slotStart()
{
    ProgressDialog* progress = new ProgressDialog();
    progress->print( i18n("Starting conversion of %1 files.", m_vector.count()) );

    // Make sure the GUI will not delete our models
    m_window->setModel( NULL );
    m_window->setPreviewModel( NULL );

    // Get some properties from the gui and initialize BatchRenamer
    const KUrl & destination = m_window->destinationUrl();
    if( m_renamer.renameMode() != eRenameMode_Rename && 
        !KIO::NetAccess::exists( destination, true, NULL ) )
    {
        int m = KMessageBox::warningContinueCancel( m_window, i18n("The directory %1 does not exist. "
                                                                   "Do you want KRename to create it for you?",  
                                                                       destination.prettyUrl() ) );
        if( m == KMessageBox::Cancel )
            return;

        if( !KIO::NetAccess::mkdir( destination, NULL ) )
        {
            KMessageBox::error( m_window, i18n("The directory %1 could not be created.").arg( destination.prettyUrl() ) );
            return;
        }
    }

    m_renamer.setDestinationDir( destination );

    // save the configuration
    // requires access to the window
    saveConfig();

    // show the progress dialog
    progress->show();
    progress->raise();
    progress->activateWindow();

    // delete the GUI
    //delete m_window;
    //m_window = NULL;
    m_window->hide();
    m_window = NULL;

    // Process files with addiational properties which were not 
    // necessary or available in the preview
    m_renamer.processFilenames();

    // Do the actual renaming
    m_renamer.processFiles( progress );

    // We are done - ProgressDialog will restart us if necessary
    //delete this;
}


void KRenameImpl::loadConfig() 
{
    KSharedConfigPtr config = KGlobal::config();

    KConfigGroup groupGui = config->group( QString("GUISettings") );
    //groupGui.readEntry( "firststart4", QVariant(true) ).toBool();
    m_window->setPreviewEnabled( 
	groupGui.readEntry( "ImagePreview", QVariant(false) ).toBool() );

    m_window->setPreviewNamesEnabled( 
	groupGui.readEntry( "ImagePreviewName", QVariant(false) ).toBool() );

    m_window->setAdvancedMode( 
	groupGui.readEntry( "Advanced", QVariant(false) ).toBool() );

    int index = groupGui.readEntry( "StartIndex", QVariant(1) ).toInt();
    int step  = groupGui.readEntry( "Stepping", QVariant(1) ).toInt();

    m_renamer.setNumberStepping( step );
    // Will call batch renamer
    m_window->setNumberStartIndex( index );

    int sortMode = groupGui.readEntry( "FileListSorting", QVariant(0) ).toInt();
    m_window->setSortMode( sortMode );

    int width = groupGui.readEntry( "Column0", QVariant(m_window->previewColumnWidth( 0 )) ).toInt();
    if( width > 0 )
		m_window->setPreviewColumnWidth( 0, width );

    width = groupGui.readEntry( "Column1", QVariant(m_window->previewColumnWidth( 1 )) ).toInt();
    if( width > 0 )
	    m_window->setPreviewColumnWidth( 1, width );


    // save Plugin configuration
    KConfigGroup groupPlugins = config->group( QString("PluginSettings") );
    m_pluginLoader->loadConfig( groupPlugins );
}

void KRenameImpl::saveConfig() 
{
    KSharedConfigPtr config = KGlobal::config();

    KConfigGroup groupGui = config->group( QString("GUISettings") );
    groupGui.writeEntry( "firststart4", false );
    groupGui.writeEntry( "ImagePreview", m_window->isPreviewEnabled() );
    groupGui.writeEntry( "ImagePreviewName", m_window->isPreviewNamesEnabled() );
    groupGui.writeEntry( "StartIndex", m_window->numberStartIndex() );
    groupGui.writeEntry( "Stepping", m_renamer.numberStepping() );
    groupGui.writeEntry( "FileListSorting", m_window->sortMode() );
    groupGui.writeEntry( "Column0", m_window->previewColumnWidth( 0 ) );
    groupGui.writeEntry( "Column1", m_window->previewColumnWidth( 1 ) );
    groupGui.writeEntry( "Advanced", m_window->isAdvancedMode() );

    KConfigGroup groupWindow = config->group( QString("WindowSettings") );

    groupWindow.writeEntry( "Maximized", m_window->isMaximized() );
    groupWindow.writeEntry( "Width", m_window->width() );
    groupWindow.writeEntry( "Height", m_window->height() );
    groupWindow.writeEntry( "XPos", m_window->x() );
    groupWindow.writeEntry( "YPos", m_window->y() );

    // save Plugin configuration
    KConfigGroup groupPlugins = config->group( QString("PluginSettings") );
    m_pluginLoader->saveConfig( groupPlugins );

    config->sync();
}

#if 0
// Own includes
#include "ProgressDialog.h"
#include "confdialog.h"
#include "fileoperation.h"
#include "kmylistview.h"
#include "kmyhistorycombo.h"
#include "pluginloader.h"
#include "replacedialog.h"
#include "undodialog.h"
#include "helpdialog.h"
#include "coorddialog.h"
#include "numberdialog.h"
#include "firststartdlg.h"
#include "wizard.h"
#include "tabs.h"
#include "dsdirselectdialog.h"
#include "krenamedcop.h"
#include "profiledlg.h"

// KDE includes
#include <dcopclient.h>
#include <kaboutapplication.h>
#include <kapplication.h>
#include <kaction.h>
#include <kcmdlineargs.h>
#include <kdirselectdialog.h>
#include <kfiledialog.h>
#include <kfileitem.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kmenubar.h>
#include <kpopupmenu.h>
#include <kurlrequester.h>
#include <ktempfile.h> 
#include <klocale.h>
#include <kio/netaccess.h>

// QT includes
#include <qbuttongroup.h>
#include <qcursor.h>
#include <qgrid.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qrect.h>
#include <qregexp.h>
#include <qtabwidget.h>
#include <qtimer.h>
#include <qtooltip.h>
#include <qvgroupbox.h>
#include <qvbox.h>
#include <qvbuttongroup.h>
#include <qwidget.h>
#include <qwidgetstack.h> 

#define ID_WIZARD 2905
#define ID_TAB 2904

KPushButton* createButton( KGuiItem item, QWidget* parent )
{
    return new KPushButton( item.iconSet(), item.text(), parent );
}

KRenameImpl::KRenameImpl( QWidget* p, KMenuBar* m, QPushButton* finish )
    : DCOPObject( "KRename" ), QObject( (QObject*)p ),
      parent( p ), menuBar( m ), finishButton( finish )
{
    m_hasCommandlineProfile = false;

    // Load Plugins
    plugin = PluginLoader::instance();
    helpDialogData = new HelpDialogData();
    // Maybe parent instead of 0 but might crash when switching from wizard to tab mode
    helpDialog = new HelpDialog( helpDialogData, NULL, NULL, false );

    m_switching = false;
    
    kapp->dcopClient()->registerAs( kapp->name() );

}

KRenameImpl::~KRenameImpl()
{
    delete helpDialog;
}

int KRenameImpl::numRealTimePreview = -1;

void KRenameImpl::setup( bool wizardmode )
{
    parent->setCaption( "KRename" ); // "KRename by Dominik Seichter
    setupActions();
    setupPages();
    updateCount();

    connect( buttonAdd, SIGNAL( clicked() ), this, SLOT( addFile() ));
    connect( buttonRemoveAll, SIGNAL( clicked()), this, SLOT( clearList() ));
    connect( buttonRemove, SIGNAL( clicked()), this, SLOT( removeFile() ));
    connect( buttonHelp, SIGNAL( clicked()), this, SLOT( showTokenHelp() ));
    connect( finishButton, SIGNAL( clicked()), this, SLOT( start() ));

    connect( checkExtension, SIGNAL( clicked()), this, SLOT( enableControls() ));
    connect( optionRename, SIGNAL( clicked()), this, SLOT( enableControls() ));
    connect( optionMove, SIGNAL( clicked()), this, SLOT( enableControls() ));
    connect( optionCopy, SIGNAL( clicked()), this, SLOT( enableControls() ));
    connect( optionLink, SIGNAL( clicked()), this, SLOT( enableControls() ));
    connect( checkUndoScript, SIGNAL( clicked()), this, SLOT( enableControls() ));

    connect( fileList, SIGNAL( updateCount()), this, SLOT( updateCount() ));
    connect( fileList, SIGNAL( updatePreview()), this, SLOT( updatePreview() ));
    connect( fileList, SIGNAL( deletePressed()), this, SLOT( removeFile() ));
    connect( fileList, SIGNAL( addFiles()), this, SLOT( addFile() ));
    connect( buttonUp, SIGNAL( clicked()), fileList, SLOT( moveUp() ));
    connect( buttonDown, SIGNAL( clicked()), fileList, SLOT( moveDown() ));
    connect( buttonUp, SIGNAL( clicked()), this, SLOT( updatePreview() ));
    connect( buttonDown, SIGNAL( clicked()), this, SLOT( updatePreview() ));
    connect( buttonUp2, SIGNAL( clicked()), this, SLOT( moveUp() ));
    connect( buttonDown2, SIGNAL( clicked()), this, SLOT( moveDown() ));
    connect( buttonReplace, SIGNAL( clicked()), this, SLOT( replace() ));
    connect( buttonEasyReplace, SIGNAL( clicked()), this, SLOT( replace() ));
    connect( buttonCoord, SIGNAL( clicked()), this, SLOT( getCoordinates() ));
    connect( comboSort, SIGNAL( activated(int)), fileList, SLOT( setSorting(int) ));
    
    connect( filename, SIGNAL( delayedTextChanged() ), this, SLOT( updatePreview() ));
    connect( extemplate, SIGNAL( delayedTextChanged() ), this, SLOT( updatePreview() ));
    connect( checkExtension, SIGNAL( clicked()), this, SLOT( updatePreview() ));
    connect( comboExtension, SIGNAL( activated(int) ), this, SLOT( updatePreview() ));
    connect( checkPreview, SIGNAL( clicked()), this, SLOT( toggleImagePreview() ));
    connect( preview, SIGNAL( itemChanged() ), this, SLOT( updatePreview() ));
    connect( preview, SIGNAL( removeItem(int) ), this, SLOT( removeFile(int) ));
    connect( preview, SIGNAL( addFile() ), this, SLOT( addFile() ));
    connect( undorequester, SIGNAL( urlSelected(const QString &)), this, SLOT( changeUndoScriptName() ));
    connect( kapp, SIGNAL( aboutToQuit() ), this, SLOT( saveConfig() ) );
    connect( buttonMove, SIGNAL( clicked()), fileList, SLOT( moveMode() ));
    connect( checkName, SIGNAL( clicked()), this, SLOT( toggleName() ));
    connect( buttonNumber, SIGNAL( clicked()), this, SLOT( changeNumbers() ));

    // Load Configuration
    loadConfig();
    page_1->setEnabled( true );
    KApplication::restoreOverrideCursor();
        
    filename->setText( "$" );   // no i18n() ! ;)
    extemplate->setText( "$" );

    // do it now so that it can be overwritten by commandline args
    setWizardMode( wizardmode );

    parseCommandline();
    enableControls();
}

const QString KRenameImpl::title( int index ) const
{
    return i18n( pageTitle[index] );
}

void KRenameImpl::parseCommandline()
{
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    QCStringList optlist = args->getOptionList ( "r" );
    for (QCStringList::ConstIterator it=optlist.begin(); it!=optlist.end(); ++it)
    {

        KURL url;
        url.setPath( *it );
        fileList->addDir( url, "*", false, true, false );
    }

    //fileList->setUpdatesEnabled( false );
    //fileList->viewport()->setUpdatesEnabled( false );
    for( int i = 0; i < args->count(); i++)
        fileList->addFile( args->url( i ) );
    //fileList->viewport()->setUpdatesEnabled( true );
    //fileList->setUpdatesEnabled( true );

    // load the profile first, so that we do not overwrite other
    // commandline settings
    QCString templ = args->getOption( "profile" );
    if( !templ.isEmpty() )
    {
        m_hasCommandlineProfile = true;
        ProfileManager::loadProfile( QString( templ ), this );
    }

    if( !args->isSet( "previewitems" ) )
        numRealTimePreview = -1;
    else
        numRealTimePreview = QString( args->getOption( "previewitems" ) ).toInt();

    templ = args->getOption( "template" );
    if( !templ.isEmpty() )
        filename->setText( templ );

    templ = args->getOption( "extension" );
    if( !templ.isEmpty() ) 
    {
        extemplate->setText( templ );
        checkExtension->setChecked( false );
    }

    templ = args->getOption( "copy" );
    if( !templ.isEmpty() ) 
    {
        urlrequester->setURL( templ );
        optionCopy->setChecked( true );
    }

    templ = args->getOption( "move" );
    if( !templ.isEmpty() ) 
    {
        urlrequester->setURL( templ );
        optionMove->setChecked( true );
    }
        
    QCStringList uselist = args->getOptionList ( "use-plugin" );
    if( !uselist.isEmpty() ) 
    {
        for(unsigned int i = 0; i < uselist.count(); i++ )
            uselist[i] = uselist[i].lower();
    
        QPtrListIterator<PluginLoader::PluginLibrary> it( plugin->libs );
        while ( it.current() ) 
        {
            if( uselist.contains( (*it)->plugin->getName().lower().utf8() ) )
                (*it)->check->setChecked( true );
                
            ++it;
        }

        pluginHelpChanged();
    }
    
    bool startnow = args->isSet( "start" );
    
    // Free some memory
    args->clear();      

    enableControls();
    updateCount();
    updatePreview();
    
    if( fileList->count() )
    {
        // we got already filenames over the commandline, so show directly the last
        // page of the wizard
        emit showPage( m_wizard ? 3 : 4 );
    }

    if( startnow ) 
    {
        // As file adding runs in a another trhread,
        // there might be adding in progress but not yet
        // all files in the list.
        // so let's wait for file adding to finish first
        // before starting.
        while( fileList->runningAddListeners() > 0 )
            kapp->processEvents();

        if( fileList->count() ) 
            // start renaming
            QTimer::singleShot( 200, this, SLOT( start() ) );
    }
}

void KRenameImpl::setupPages()
{
    setupPage1();
    setupPage2();
    setupPage3();
    setupPage4();

    // Disable dirname ;)
    enableControls();
}

void KRenameImpl::setupPage1()
{
    page_1 = new QWidget( parent );

    pageLayout = new QHBoxLayout( page_1, 11, 6 );
    Layout3 = new QVBoxLayout( 0, 0, 6 );
    Layout4 = new QHBoxLayout( 0, 0, 6 );
    Layout5 = new QVBoxLayout( 0, 0, 6 );

    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    QSpacerItem* spacer2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

    fileList = new KMyListBox( page_1 );

    buttonAdd = new KPushButton( page_1 );
    buttonAdd->setText( i18n( "&Add..." ) );

    buttonRemove = new KPushButton( page_1 );
    buttonRemove->setText( i18n( "Re&move" ) );

    buttonRemoveAll = new KPushButton( page_1 );
    buttonRemoveAll->setText( i18n( "R&emove All" ) );

    comboSort = new KComboBox( false, page_1 );
    comboSort->insertItem( i18n("Sort: Unsorted") );
    comboSort->insertItem( i18n("Sort: Ascending") );
    comboSort->insertItem( i18n("Sort: Descending") );
    comboSort->insertItem( i18n("Sort: Random") );
    comboSort->insertItem( i18n("Sort: Numeric") );
    
    checkPreview = new QCheckBox( page_1 );
    checkPreview->setText( i18n("&Preview") );

    labelCount = new QLabel( page_1 );

    buttonUp = new KPushButton( page_1 );
    buttonUp->setPixmap( BarIcon( "1uparrow" ) );

    buttonDown = new KPushButton( page_1 );
    buttonDown->setPixmap( BarIcon( "1downarrow" ) );

    checkName = new QCheckBox( page_1 );
    checkName->setText( i18n("&Display name") );

    buttonMove = new KPushButton( page_1 );
    buttonMove->setPixmap( BarIcon( "move" ) );

    Layout5->addWidget( buttonUp );
    Layout5->addWidget( buttonMove );
    Layout5->addWidget( buttonDown );

    Layout4->addLayout( Layout5 );
    Layout4->addItem( spacer2 );
    
    Layout3->addWidget( buttonAdd );
    Layout3->addWidget( buttonRemove );
    Layout3->addWidget( buttonRemoveAll );
    Layout3->addWidget( comboSort );
    Layout3->addWidget( checkPreview );
    Layout3->addWidget( checkName );
    Layout3->addWidget( labelCount );
    Layout3->addItem( spacer );
    Layout3->addLayout( Layout4 );
    Layout3->addItem( spacer );
    
    pageLayout->addWidget( fileList );
    pageLayout->setStretchFactor( fileList, 2 );
    pageLayout->addLayout( Layout3 );

    emit pageDone( page_1, i18n( pageTitle[0] ) );

    QToolTip::add( buttonAdd, i18n( "Add one or more files or directories" ) );
    QToolTip::add( buttonRemove, i18n( "Remove selected files" ) );
    QToolTip::add( buttonRemoveAll, i18n( "Remove all files" ) );
    QToolTip::add( checkPreview, i18n( "Enable/disable preview of pictures." ) );
    QToolTip::add( labelCount, i18n( "Displays the number of files in the list." ) );
    QToolTip::add( buttonUp, i18n( "Move selected items up" ));
    QToolTip::add( buttonDown, i18n( "Move selected items down" ));
    QToolTip::add( checkName, i18n( "Enable/disable display of file name." ) );
    QToolTip::add( buttonMove, i18n( "Move selected items (select the new location with the mouse)" ));
}

void KRenameImpl::setupPage2()
{
    page_2 = new QWidget( parent );

    pageLayout_2 = new QVBoxLayout( page_2, 6, 6 );

    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );

    groupOptions = new QButtonGroup( page_2 );
    groupOptions->setTitle( i18n( "O&ptions" ) );
    groupOptions->setColumnLayout(0, Qt::Vertical );
    groupOptions->layout()->setSpacing( 6 );
    groupOptions->layout()->setMargin( 11 );
    groupOptionsLayout = new QVBoxLayout( groupOptions->layout() );
    groupOptionsLayout->setAlignment( Qt::AlignTop );

    optionRename = new QRadioButton( groupOptions );
    optionRename->setText( i18n( "&Rename input files" ) );
    optionRename->setChecked( true );

    optionCopy = new QRadioButton( groupOptions );
    optionCopy->setText( i18n( "Cop&y files to destination directory" ) );

    optionMove = new QRadioButton( groupOptions );
    optionMove->setText( i18n( "&Move files to destination directory" ) );

    optionLink = new QRadioButton( groupOptions );
    optionLink->setText( i18n("Create symbolic &links in destination directory") );
    
    dirname = new KMyHistoryCombo( false, groupOptions, "Path" );
    urlrequester = new KURLRequester( dirname, groupOptions );
    urlrequester->setMode( KFile::Directory | KFile::ExistingOnly );
    
    checkOverwrite = new QCheckBox( groupOptions );
    checkOverwrite->setText( i18n( "&Overwrite existing files" ) );

    groupOptionsLayout->addWidget( optionRename );
    groupOptionsLayout->addWidget( optionCopy );
    groupOptionsLayout->addWidget( optionMove );
    groupOptionsLayout->addWidget( optionLink );
    groupOptionsLayout->addWidget( urlrequester );
    groupOptionsLayout->addItem( spacer_3 );
    groupOptionsLayout->addWidget( checkOverwrite );

    groupUndo = new QVGroupBox( page_2 );
    groupUndo->setTitle( i18n("&Undo Renaming") );

    checkUndoScript = new QCheckBox( i18n("&Create an undo script"), groupUndo );

    undorequester = new KURLRequester( groupUndo );
    undorequester->setEnabled( false );
    undorequester->setMode( KFile::File | KFile::LocalOnly  );
    undorequester->setFilter( i18n("*.krename|KRename undo scripts (*.krename)\n"
                                   "*|All Files (*)") );
    undorequester->fileDialog()->setOperationMode( KFileDialog::Saving );

    pageLayout_2->addWidget( groupOptions );
    pageLayout_2->addWidget( groupUndo );
    pageLayout_2->addItem( spacer_3 );

    emit pageDone( page_2, i18n( pageTitle[1] ) );

    QToolTip::add( optionRename, i18n( "Input files will be renamed." ) );
    QToolTip::add( optionCopy, i18n( "Copies all files to the destination directory and renames them." ) );
    QToolTip::add( optionMove, i18n( "Moves all files to the destination directory and renames them." ) );
}

void KRenameImpl::setupPage3()
{
    page_3 = new KJanusWidget( parent, "janus", KJanusWidget::TreeList );
    page_3->setShowIconsInTreeList( true );
    page_3->setTreeListAutoResize( true );

    setupTab1();
    
    emit pageDone( page_3, i18n( pageTitle[2] ) );
}

void KRenameImpl::setupPage4()
{
    page_4 = new QWidget( parent );
    pageLayout_4 = new QVBoxLayout( page_4, 11, 6 );

    fileTab = new QWidgetStack( page_4 );
    setupFileTab1();
    setupFileTab2();
    
    Layout22 = new QHBoxLayout( 0, 0, 6 );
    Layout23 = new QVBoxLayout( 0, 0, 6 );
    
    preview = new KMyListView( &changes, fileList, page_4 );
    preview->setSorting( -1 );
    preview->addColumn( i18n( "Origin" ) );
    preview->addColumn( i18n( "Renamed" ) );

    buttonUp2 = new KPushButton( page_4 );
    buttonUp2->setPixmap( BarIcon( "1uparrow" ) );

    buttonDown2 = new KPushButton( page_4 );
    buttonDown2->setPixmap( BarIcon( "1downarrow" ) );

    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding );

    Layout23->addItem( spacer );
    Layout23->addWidget( buttonUp2 );
    Layout23->addWidget( buttonDown2 );
    Layout23->addItem( spacer );

    Layout22->addWidget( preview );
    Layout22->addLayout( Layout23 );
    Layout22->setStretchFactor( preview, 2 );

    pageLayout_4->addWidget( fileTab );
    pageLayout_4->addLayout( Layout22 );

    emit pageDone( page_4, i18n( pageTitle[3] ) );

    QToolTip::add( filename, i18n( "Add a template.<br>Example: <i>picture###</i>" ) );
    QToolTip::add( extemplate, i18n( "Add a template for the file extension.<br>It behaves like the filename template.") );
    QToolTip::add( checkExtension, i18n("Checking this checkbox is the same as setting the extension template to $"));
    QToolTip::add( buttonUp2, i18n( "Move selected items up" ));
    QToolTip::add( buttonDown2, i18n( "Move selected items down" ));
    QToolTip::add( preview, i18n( "Double click on an item to modify it." ));
    QToolTip::add( buttonHelp, i18n("Help Dialog with all tokens supported by KRename."));
    QToolTip::add( buttonReplace, i18n("<qt>Find and replace characters or part string of the source filename in the destination filename.</qt>") );
    QToolTip::add( buttonEasyReplace, i18n("<qt>Find and replace characters or part string of the source filename in the destination filename.</qt>") );
    QToolTip::add( comboExtension, i18n("<qt>You can use \".gz\" and \".tar.gz\" as file extension of the file backup.tar.gz depending on this setting.</qt>") );
}

void KRenameImpl::setupFileTab1()
{
    QWidget* tab = new QWidget( fileTab );

    labelTemplate = new QLabel( tab );
    labelTemplate->setText( i18n( "&Template:" ) );

    filename = new KMyHistoryCombo( true, tab, "Template" );
    labelTemplate->setBuddy( filename );

    QLabel* labelDot = new QLabel( tab );
    labelDot->setText("<b>.</b>");

    extemplate = new KMyHistoryCombo( true, tab, "ExTemplate" );
    extemplate->setEnabled( FALSE );

    buttonHelp = new KPushButton( i18n("&Functions..."), tab );
    buttonHelp->setIconSet( SmallIcon("help") );

    checkExtension = new QCheckBox( tab );
    checkExtension->setText( i18n( "&Use extension of the input file" ) );
    checkExtension->setChecked( TRUE );

    comboExtension = new KComboBox( false, tab );

    buttonReplace = new KPushButton( i18n("Find &and Replace..."), tab );
    buttonReplace->setIconSet( SmallIconSet("find") );

    buttonNumber = new KPushButton( i18n("&Numbering..."), tab );
    buttonCoord = new KPushButton( i18n("&Insert Part of Filename..."), tab );

    helpDialog->setLineEdit( filename->lineEdit() );
    
    QStringList help;
    help.append("$;;" + i18n("old filename") );
    help.append("%;;" + i18n("old filename converted to lower case") );
    help.append("&;;" + i18n("old filename converted to upper case") );
    help.append("*;;" + i18n("first letter of every word upper case") );
    help.append("[&1][%2-];;" + i18n("first letter of filename upper case") );
    help.append("#;;" + i18n("number (try also ##, ###, ... for leading zeros)") );
    help.append("#{0;1};;" + i18n("counter with custom start value 0 and custom stepping 1") );
    help.append("/;;" + i18n("create a subdirectory") );
    help.append("\\;;" + i18n("strip whitespaces leading and trailing") );
    help.append("[$x-y];;" + i18n("character x to y of old filename") );
    help.append("[$x;y];;" + i18n("y characters of old filename starting at x") );
    help.append("[$dirname];;" + i18n("insert name of directory") );
    help.append("[$dirname.];;" + i18n("insert name of parent directory") );
    help.append("[#length-0];;" + i18n("insert the length of the input filename") );
    helpDialogData->add( i18n("Built-in Functions:" ), &help, SmallIcon("krename"), true );

    help.clear();
    help.append( "\\$;;" + i18n("Insert '$'") );
    help.append( "\\%;;" + i18n("Insert '%'") );
    help.append( "\\&;;" + i18n("Insert '&'") );
    help.append( "\\*;;" + i18n("Insert '*'") );
    help.append( "\\/;;" + i18n("Insert '/'") );
    help.append( "\\\\;;" + i18n("Insert '\\\\'") );
    help.append( "\\[;;" + i18n("Insert '['") );
    help.append( "\\];;" + i18n("Insert ']'") );
    help.append( "\\#;;" + i18n("Insert '#'") );
    helpDialogData->add( i18n("Special Characters:" ), &help, SmallIcon("krename") );

    /*
        //TODO: FIX this tooltip
    QToolTip::add( buttonHelp,
    i18n(
        "<b>$</b> old filename<br><b>%</b> old filename"
        " converted to lower case<br><b>&</b> old filename converted to upper case<br>"
        "<b>*</b> first letter of every word uppercase<br>"
        "<b>#</b> Adds a number to the filename (add more #'s for leading zeros)<br>"
        "<b>/</b> creates a subdirectory<br>"
        "<b>[$x-y]</b> Characters x to y of the old filename<br>"
        "<b>[%x-y]</b> Characters x to y converted to lower case"
    ) );
    */

    QSpacerItem* spacer_9 = new QSpacerItem( 200, 20, QSizePolicy::Maximum, QSizePolicy::Maximum );

    QVBoxLayout* layout = new QVBoxLayout( tab );
    QHBoxLayout* Layout1 = new QHBoxLayout( 0, 6, 6 );
    QHBoxLayout* Layout2 = new QHBoxLayout( 0, 6, 6 );
    QHBoxLayout* ExtensionLayout = new QHBoxLayout( 0, 6, 6 );
    
    Layout1->addWidget( labelTemplate );
    Layout1->addWidget( filename );
    Layout1->setStretchFactor( filename, 4 );
    Layout1->addWidget( labelDot );
    Layout1->addWidget( extemplate );
    Layout1->addWidget( buttonHelp );

    Layout2->addWidget( buttonReplace );    
    Layout2->addWidget( buttonNumber );
    Layout2->addWidget( buttonCoord );
    Layout2->addItem( spacer_9 );
    
    ExtensionLayout->addWidget( checkExtension );
    ExtensionLayout->addItem( spacer_9 );
    ExtensionLayout->addWidget( new QLabel( i18n("File extension starts at:"), tab ) );
    ExtensionLayout->addWidget( comboExtension );
    
    layout->addLayout( Layout1 );
    layout->addLayout( ExtensionLayout );
    layout->addLayout( Layout2 );
    layout->addItem( spacer_9 );
    
    fileTab->addWidget( tab, ID_TAB );
}

void KRenameImpl::setupFileTab2()
{
    QWidget* tab = new QWidget( page_4 );

    QHBoxLayout* layout = new QHBoxLayout( tab, 6, 6 );
    
    QVGroupBox* vgroup1 = new QVGroupBox( i18n("&Filename"), tab );
    QGrid* grid = new QGrid( 4, vgroup1 );
        
    QLabel* l = new QLabel( i18n("&Prefix:"), grid );
    comboKRenamePrefix = new KComboBox( false, grid );
    comboPrefix = new KMyHistoryCombo( true, grid, "comboPrefix" );   
    buttonEasy1 = new KPushButton( grid );
    l->setBuddy( comboPrefix );
    
    l = new QLabel( i18n("&Suffix:"), grid );
    comboKRenameSuffix = new KComboBox( false, grid, "comboSuffix"  );
    comboSuffix = new KMyHistoryCombo( true, grid );
    buttonEasy2 = new KPushButton( grid );
    l->setBuddy( comboSuffix );

    QStringList comboItems;
    comboItems.append( i18n("Convert to lower case ") );
    comboItems.append( i18n("Convert to upper case") );
    comboItems.append( i18n("Capitalize") ); 
    
    l = new QLabel( i18n("&Name:"), grid );
    comboKRenameFilename = new KComboBox( false, grid );
    comboKRenameFilename->insertItem( i18n("Use original name") );
    comboKRenameFilename->insertStringList( comboItems );
    comboKRenameFilename->insertItem( i18n("Custom name") );
    l->setBuddy( comboKRenameFilename );
    
    comboCustom = new KMyHistoryCombo( true, grid, "comboCustom" );
    buttonEasy3 = new KPushButton( grid );
             
    l = new QLabel( i18n("&Extension:"), grid );
    comboKRenameExtension = new KComboBox( false, grid );
    comboKRenameExtension->insertItem( i18n("Use original extension") );
    comboKRenameExtension->insertStringList( comboItems );
    comboKRenameExtension->insertItem( i18n("Custom extension") );
    l->setBuddy( comboKRenameExtension );
    
    comboCustomExtension = new KMyHistoryCombo( true, grid, "comboCustomExtension" );
    buttonEasy4 = new KPushButton( grid );
    
    QVBox* rightBox = new QVBox( tab );
    QVGroupBox* group = new QVGroupBox( i18n("&Number" ), rightBox );
    
    spinStart = new KIntNumInput( group );
    spinStart->setLabel( i18n( "Start &index:" ), AlignLeft | AlignVCenter );

    spinNull = new KIntNumInput( spinStart, 1, group );        
    spinNull->setLabel( i18n("&Number of digits:"), AlignLeft | AlignVCenter );
    spinNull->setRange( 1, 100, 1, false );
    
    buttonEasyReplace = new KPushButton( i18n("&Find and Replace..."), rightBox );
    buttonEasyReplace->setIconSet( SmallIconSet("find") );
    
    layout->addWidget( vgroup1 );
    layout->addWidget( rightBox );
    
    comboKRenamePrefix->insertItem( "" );
    comboKRenamePrefix->insertItem( i18n("Number") );
    comboKRenamePrefix->insertItem( i18n("Date") );
    
    comboKRenameSuffix->insertItem( "" );
    comboKRenameSuffix->insertItem( i18n("Number") );
    comboKRenameSuffix->insertItem( i18n("Date") );
    
    enableControls();
        
    connect( comboPrefix, SIGNAL( textChanged( const QString &)), this, SLOT( changed()));
    connect( comboSuffix, SIGNAL( textChanged( const QString &)), this, SLOT( changed()));
    connect( comboKRenameFilename, SIGNAL( activated(int)), this, SLOT( changed()));
    connect( comboCustom, SIGNAL( textChanged( const QString & ) ), this, SLOT( changed() ));
    connect( comboKRenamePrefix, SIGNAL(activated(int)), this, SLOT(changed()));
    connect( comboKRenameExtension, SIGNAL( activated(int)), this, SLOT( changed()));
    connect( comboCustomExtension, SIGNAL( textChanged( const QString & ) ), this, SLOT( changed() ));
    connect( comboKRenameSuffix, SIGNAL(activated(int)), this, SLOT(changed()));
    connect( spinNull, SIGNAL( valueChanged(int) ), this, SLOT( changed() ));
    connect( spinStart, SIGNAL( valueChanged(int) ), this, SLOT( changed() ));   
        
    buttonEasy1->setIconSet( SmallIcon("help") );
    buttonEasy2->setIconSet( SmallIcon("help") );
    buttonEasy3->setIconSet( SmallIcon("help") );
    buttonEasy4->setIconSet( SmallIcon("help") );
    
    buttonEasy1->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
    buttonEasy2->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
    buttonEasy3->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
    buttonEasy4->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
    
    const QString easy_text( i18n("<qt>Insert a special KRename command which inserts file information into the filename.</qt>") );
    
    connect( buttonEasy1, SIGNAL( clicked() ), this, SLOT( slotEasy1() ) );
    connect( buttonEasy2, SIGNAL( clicked() ), this, SLOT( slotEasy2() ) );
    connect( buttonEasy3, SIGNAL( clicked() ), this, SLOT( slotEasy3() ) );
    connect( buttonEasy4, SIGNAL( clicked() ), this, SLOT( slotEasy4() ) );
    
    QToolTip::add( buttonEasy1, easy_text );
    QToolTip::add( buttonEasy2, easy_text );
    QToolTip::add( buttonEasy3, easy_text );
    QToolTip::add( buttonEasy4, easy_text );
    
    fileTab->addWidget( tab, ID_WIZARD );
}

QString KRenameImpl::easyOptions( KComboBox* combo, KMyHistoryCombo* custom )
{
    QString t = QString::null;
    if( combo->currentItem() == 0 )
        t = "$";
    else if( combo->currentItem() == 1 )
        t = "%";
    else if( combo->currentItem() == 2 )
        t = "&";
    else if( combo->currentItem() == 3 )
        t = "*";
    else if( combo->currentItem() == 4 )
        t = custom->text();
    
    return t;
}

void KRenameImpl::changed()
{
    QString t = easyOptions( comboKRenameFilename, comboCustom );
    t = comboPrefix->text() + t + comboSuffix->text();

    for( int i=0; i<spinNull->value();i++)
    {
        if( comboKRenamePrefix->currentItem() == 1 )
            t.prepend("#");
        if( comboKRenameSuffix->currentItem() == 1 )
            t.append("#");
    }

    if( comboKRenamePrefix->currentItem() == 2 )        
        t.prepend( "[date]" );
    
    if( comboKRenameSuffix->currentItem() == 2 )
        t.append( "[date]" );

    QString e = easyOptions( comboKRenameExtension, comboCustomExtension );
    
    bool update = false;
    update = ( counterStart() != spinStart->value() && t == fileNameTemplate() );
    setCounterStart( spinStart->value() );
    
    filename->setText( t );
    extemplate->setText( e );
    checkExtension->setChecked( (comboKRenameExtension->currentItem() == 0) );
    
    if( update )
        updatePre();
        
    enableControls();
}

void KRenameImpl::parseWizardMode()
{
    /** This function is not very accrurate and 
      * guesses most cases.
      * But it works pretty well and is IMHO enough for this
      * simple dialog.
      */
    QString t = filename->text();
    
    spinNull->setValue( t.contains( "#" ) );
    
    if( t.startsWith( "[date]") ) 
    {
        t = t.right( t.length() - 6 );
        comboKRenamePrefix->setCurrentItem( 2 );
    }
    else if( t.startsWith( "#") )
    {
        while( t.startsWith( "#" ) )
            t = t.right( t.length() - 1 );
        comboKRenamePrefix->setCurrentItem( 1 );
    }
    
    if( t.endsWith( "[date]" ) )
    {
        t = t.left( t.length() - 6 );
        comboKRenameSuffix->setCurrentItem( 2 );
    }
    else if( t.endsWith( "#") )
    {
        while( t.endsWith( "#" ) )
            t = t.left( t.length() - 1 );
        
        comboKRenameSuffix->setCurrentItem( 1 );
    }
    
    int pos = -1;
    if( (pos = t.find( "$" )) > -1 )
        comboKRenameFilename->setCurrentItem( 0 );
    else if( (pos = t.find( "%" )) > -1 )
        comboKRenameFilename->setCurrentItem( 1 );
    else if( (pos = t.find( "&" )) > -1 )
        comboKRenameFilename->setCurrentItem( 2 );
    else if( (pos = t.find( "*" )) > -1 ) 
        comboKRenameFilename->setCurrentItem( 3 );
    else
    {
        comboKRenameFilename->setCurrentItem( 4 );
        comboCustom->setText( t );
    }
        
    if( pos > 0 )
    {
        comboPrefix->setText( t.left( pos ) );
        comboSuffix->setText( t.mid( pos + 1, t.length() - pos - 1) );
    }
    
    spinStart->setValue( m_index );
}

void KRenameImpl::setupTab1()
{
    /*
     * Draw plugin user interfaces
     */

    QFrame* parent;
    QVBoxLayout* Layout;
    QVBoxLayout* gLayout;
    QGroupBox* g;
    QWidget* page;

    /* strangely I do not have to delete this stuff
     * after plugins have been reloaded.
     * could cause a memory leak though
     */
    while( ( page = page_3->pageWidget( 0 ) ) )
        page_3->removePage( page );

    QPtrListIterator<PluginLoader::PluginLibrary> it( plugin->libs ); // iterator for dict
    while ( it.current() ) {
        if( !(*it)->plugin->alwaysUsed() )
        {
            parent = page_3->addPage( (*it)->plugin->getName(), QString::null, (*it)->plugin->getIcon() );
        
            Layout = new QVBoxLayout( parent, 11, 6, "Layout");
            g = new QGroupBox( parent );
            gLayout = new QVBoxLayout( g, 11, 6, "gLayout" );

            (*it)->check = new QCheckBox( i18n("&Use this plugin"), parent );
            connect( (*it)->check, SIGNAL( clicked() ), this, SLOT( pluginHelpChanged() ) );
            connect( (*it)->plugin, SIGNAL( previewChanged( Plugin* ) ), this, SLOT( updatePluginPreview( Plugin* ) ) );
            Layout->addWidget( (*it)->check );
            (*it)->plugin->drawInterface( g, gLayout );

            Layout->addWidget( g );
            
        } else
            (*it)->plugin->addHelp( helpDialogData );
            
        ++it;
    }

    page_3->showPage( 1 );
}

void KRenameImpl::addFile()
{
    bool auto_up = false;
    
    DSDirSelectDialog* dsd = new DSDirSelectDialog( parent );
    if( dsd->exec() == QDialog::Accepted ) {
        KURL::List slist = dsd->selectedURLs();
        KURL::List::Iterator it = slist.begin();

        for ( ; it != slist.end(); ++it )
        {
            if( !fileList->isFile( *it, false ) )
            {
                if( dsd->onlyDirs() )
                    fileList->addDirName( *it, dsd->currentFilter(), dsd->hidden(), dsd->recursively() );
                else
                    fileList->addDir( *it, dsd->currentFilter(), dsd->hidden(), dsd->recursively(), dsd->dirs() );
            }
            else
            {
                fileList->addFile( *it, true );
                auto_up = true;
            }
        }
        
        if( auto_up )
        {
            updatePreview();
            updateCount();
        }
    }
}

void KRenameImpl::clearList()
{
    fileList->clear();
    changes.clear();
    updateCount();
    updatePreview();
}

void KRenameImpl::removeFile()
{
    unsigned int i = 0;
    do {
        if(fileList->isSelected( i ))
            removeFile( i );
        else
            i++;
    } while( i < fileList->count() );

    updateCount();
    updatePreview();
}

void KRenameImpl::removeFile( int index )
{
    KURL url = fileList->url( index );
    fileList->removeItem( index );
    // we have this to display the add files url label when count() == 0
    fileList->positionLabel();
    // remove this file from changes, too
    for( unsigned int i = 0; i < changes.count(); i++ )
        if( changes[i].url == url )
            changes.remove( changes.at( i ) );

    updateCount();
    updatePreview();
}

void KRenameImpl::help()
{
    kapp->invokeHelp();
}

void KRenameImpl::start()
{
    if(!checkErrors())
        return;

    // Let the plugins check for erorrs
    QPtrListIterator<PluginLoader::PluginLibrary> it( plugin->libs );
    while ( it.current() ) {
        if( (*it)->usePlugin && !(*it)->plugin->checkError() )
            return;
        ++it;
    }

    ProgressDialog* p = new ProgressDialog( 0, "p" );
    p->print(QString( i18n("Starting conversion of %1 files.") ).arg(fileList->count()));

    // Save History
    dirname->saveSettings();
    filename->saveSettings();
    extemplate->saveSettings();

    KConfig * config = kapp->config();
    config->setGroup("FileExtension");
    config->writeEntry("FileExtensionCombo", comboExtension->currentItem() );

    // save the configuration
    saveConfig();

    b = new BatchRenamer();
    if( setupBatchRenamer( b, false ) )
    {
        p->show();
        parent->hide();
        b->processFiles( p, this );
    }

    // Memory leak!!! :  ??
    //delete parent;
}

void KRenameImpl::enableControls()
{
    comboCustom->setEnabled( comboKRenameFilename->currentItem() == 4 );
    comboCustomExtension->setEnabled( comboKRenameExtension->currentItem() == 4 );
    buttonEasy3->setEnabled( comboKRenameFilename->currentItem() == 4 );
    buttonEasy4->setEnabled( comboKRenameExtension->currentItem() == 4 );
    urlrequester->setEnabled( !optionRename->isChecked() );
    checkUndoScript->setEnabled( !optionCopy->isChecked() && !optionLink->isChecked() );
    undorequester->setEnabled( checkUndoScript->isChecked() && checkUndoScript->isEnabled() );
    extemplate->setEnabled( !checkExtension->isChecked() );
    loadPlugins->setEnabled( !plugin->filePluginsLoaded() );
}

bool KRenameImpl::checkErrors()
{
    if( filename->text().isEmpty() ) {
        KMessageBox::sorry( parent, i18n("Specify a template to use for renaming files.") );
        return false;
    }
    if( dirname->text().isEmpty() && !optionRename->isChecked()) {
        KMessageBox::sorry( parent, i18n("Please give a destination directory !") );
        emit showPage( 2 );
        return false;
    }
    if( checkUndoScript->isChecked() && !optionCopy->isChecked() && undorequester->url().isEmpty() ) {
        KMessageBox::sorry( parent, i18n("Please give the name of the undo script!") );
        showPage( 2 );
        return false;
    }

    return true;
}

void KRenameImpl::updateCount()
{
    labelCount->setText( QString( i18n("Files: <b>%1</b>") ).arg(fileList->count()));

    buttonCoord->setEnabled( (bool)fileList->count() );
    buttonReplace->setEnabled( (bool)fileList->count() );
    buttonEasyReplace->setEnabled( (bool)fileList->count() );
    emit enableFinish( (bool)fileList->count() );
    buttonRemove->setEnabled( (bool)fileList->count() );
    buttonRemoveAll->setEnabled( (bool)fileList->count() );
    buttonMove->setEnabled( (bool)fileList->count() );
    buttonUp->setEnabled( (bool)fileList->count() );
    buttonUp2->setEnabled( (bool)fileList->count() );
    buttonDown->setEnabled( (bool)fileList->count() );
    buttonDown2->setEnabled( (bool)fileList->count() );

    updateDots();
}

void KRenameImpl::fillStructures( BatchRenamer* b, bool preview )
{
    QValueList<data> f;
    unsigned int max = (preview && KRenameImpl::numRealTimePreview > -1 ? KRenameImpl::numRealTimePreview : fileList->count());

    for( unsigned int i = 0; i < max; i++) {
        KURL url = fileList->url( i );
        QFileInfo fi( url.path() );
        
        data fdata;
        fdata.dir = fileList->dir( i );
        if( !fdata.dir ) {
            splitFilename( &fi, &fdata.src.name, &fdata.src.extension );
        } else {
            fdata.src.name = BatchRenamer::findDirName( "dirname", url.path() );
            fdata.src.extension = "";
        }

        fdata.src.url = url;

        /* Let's do some error checking for files
         * called: file_xy.
         */
        if( url.path().right(1) == "." )
            // File ends with a dot
            fdata.src.name.append( "." );

        fdata.src.directory = fi.dirPath( true );
        if( fdata.src.directory.right(1) != "/" )
            fdata.src.directory.append("/");

        f.append( fdata );
    }

    /*
     * Tell plugins that they should fill their structure, too.
     * Check also which plugins should be used !
     */
    QPtrListIterator<PluginLoader::PluginLibrary> it( plugin->libs );
    for( int z = 0; it.current(); ++it, z++ ) {
        if( (*it)->plugin->alwaysUsed() || ((*it)->check && (*it)->check->isChecked()) )
            (*it)->plugin->fillStructure();
    }

    b->setFiles( f );
}

void KRenameImpl::updatePre()
{
    updatePreview();
    updateCount();
    toggleImagePreview();
}

void KRenameImpl::addFileOrDir( const QString & name )
{
    fileList->addFile( name, false );
}

void KRenameImpl::addFileOrDir( const KURL & name )
{
    fileList->addFile( name, true );
}

bool KRenameImpl::setupBatchRenamer( BatchRenamer* b, bool preview )
{
    b->setText( filename->text() );
    b->setExText( ( checkExtension->isChecked() ? QString("$") : extemplate->text() ) );
    b->setIndex( m_index );
    b->setResetCounter( m_reset );
    b->setStep( m_step );
    b->setSkipList( skip );
    b->setReplaceList( rep );
    b->setChanges( changes );
    
    if( !preview ) {
        QString url = urlrequester->url();
        if( url.right(1) != "/" )
            url.append( "/" );
        
        if( !KIO::NetAccess::exists( KURL( url ) ) )
        {
            int m = KMessageBox::warningContinueCancel( parent, i18n("The directory %1 does not exist. "
            "KRename will create it for you.").arg( url ) );
            if( m == KMessageBox::Cancel )
                return false;
            
            int i = 0;
            QString d = "/";
            while( (d += url.section( "/", i, i, QString::SectionSkipEmpty )) && ! d.isEmpty() ) { // asignment here!
                if( !KIO::NetAccess::exists( d ) && !KIO::NetAccess::mkdir( d ) )
                {
                    break;
                }
                d.append( "/" );
                i++;
            }
        }
            
        b->setDirname( url );
        b->setOverwrite( checkOverwrite->isChecked() );
        b->setUndo( checkUndoScript->isChecked() && !optionCopy->isChecked() );
        if( checkUndoScript->isChecked() && !optionCopy->isChecked() )
            b->setUndoScript( undorequester->url() );

        b->setMode( currentRenameMode() );
    }

    fillStructures( b, preview );
    return true;
}

int KRenameImpl::currentRenameMode()
{
    if( optionCopy->isChecked() )
        return COPY;
    else if( optionMove->isChecked() )
        return MOVE;
    else if( optionRename->isChecked() )
        return RENAME;
    else if( optionLink->isChecked() )
        return LINK;
    // prevent a compiler warning
    return RENAME;
}

void KRenameImpl::updatePluginPreview( Plugin* p ) 
{
    QPtrListIterator<PluginLoader::PluginLibrary> it( plugin->libs ); // iterator for dict
    while ( it.current() ) {
        if( (*it)->plugin == p && (*it)->usePlugin ) {
            updatePreview();
            return;
        }

        ++it;
    }
}

void KRenameImpl::updatePreview()
{
    // is there a number in the filename or extension
    if( filename->text().contains( "#" ) || extemplate->text().contains( "#" ) )
        buttonNumber->setEnabled( true );
    else
        buttonNumber->setEnabled( false );

    if( filename->isEmpty() && extemplate->isEmpty() && !checkExtension->isChecked() )
        return;

    int h = preview->horizontalScrollBar()->value();
    int v = preview->verticalScrollBar()->value();
    
    preview->clear();

    if( fileList->count() <= 0 )
        return;

    BatchRenamer b;
    if( setupBatchRenamer( &b, true ) )
    {
        KApplication::setOverrideCursor( Qt::WaitCursor );
        b.createPreview( preview );
        KApplication::restoreOverrideCursor();
    }
    preview->horizontalScrollBar()->setValue( h );
    preview->verticalScrollBar()->setValue( v );
}

void KRenameImpl::loadConfig()
{
    KConfig* config = kapp->config();

    config->setGroup("HistSize");
    m_hist = config->readNumEntry("HistoryItems", 10 );
    updateHist();

    config->setGroup("GUISettings");
    // Image Preview
    checkPreview->setChecked(config->readBoolEntry("ImagePreview", false ));
    fileList->setPreview( checkPreview->isChecked() );
    m_index = config->readNumEntry("StartIndex", 1);
    m_step = 1;
    m_reset = false;
    m_loadplugins = config->readBoolEntry("LoadFilePlugins", true );
    m_autosize = config->readBoolEntry("ColumnAutosize", false );
    checkName->setChecked(config->readBoolEntry("Name", false ));
    fileList->setPreviewSize( config->readNumEntry("PreviewSize", 80) );
    fileList->setName( checkName->isChecked() );
    fileList->setSorting( config->readNumEntry("FileListSorting", KMyListBox::UNSORTED ) );
    comboSort->setCurrentItem( fileList->sorting() );
    
    CoordDialog::m_inversion = config->readBoolEntry("Inverse", false);
    m_wizard = config->readBoolEntry( "GUIWizardMode", true );

    int width = config->readNumEntry( "Width", parent->width() );
    int height = config->readNumEntry( "Height", parent->height() );
    if( config->readBoolEntry("Maximized", false ) ) {
        parent->showMaximized();
    } else {
        parent->resize( width, height );
    }
    
    refreshColumnMode();
    if( !m_autosize ) {
        preview->setColumnWidth( 0, 
            config->readNumEntry("Column0", (width/2) - 40 ) );
        preview->setColumnWidth( 1, 
            config->readNumEntry("Column1", (width/2) - 40 ) );
                
        if( preview->columnWidth( 0 ) > (width/2) )
        {
            preview->setColumnWidth( 0, (width/2) - 40 );
            preview->setColumnWidth( 1, (width/2) - 40 );
        }
    }

    if( !checkPreview->isChecked() ) {
        checkName->setChecked( true );
        checkName->setEnabled( false );
    }

    //parent->show();
    page_1->setEnabled( false );
    KApplication::setOverrideCursor( Qt::WaitCursor );
    plugin->loadPlugins( m_loadplugins );
    setupTab1();
    config->setGroup("GUISettings");

    // File Extension
    config->setGroup("FileExtension");
    // default LAST DOT
    comboExtension->setCurrentItem( config->readNumEntry("FileExtensionCombo", 1 ) );
}

void KRenameImpl::saveConfig()
{
    KConfig* config = kapp->config();

    config->setGroup("HistSize");
    config->writeEntry("HistoryItems", m_hist );

    config->setGroup("GUISettings");
    config->writeEntry("firststart", false );
    config->writeEntry("ImagePreview", checkPreview->isChecked() );
    config->writeEntry("StartIndex", m_index );
    config->writeEntry("Maximized", parent->isMaximized() );
    config->writeEntry("Width", parent->width() );
    config->writeEntry("Height", parent->height() );
    config->writeEntry("XPos", parent->x() );
    config->writeEntry("YPos", parent->y() );
    config->writeEntry("LoadFilePlugins", m_loadplugins );
    config->writeEntry("ColumnAutosize", m_autosize );
    config->writeEntry("Name", checkName->isChecked() );
    config->writeEntry("PreviewSize", fileList->getPreviewSize() );
    config->writeEntry("Inverse", CoordDialog::m_inversion );
    config->writeEntry("Column0", preview->columnWidth( 0 ) );
    config->writeEntry("Column1", preview->columnWidth( 1 ) );
    config->writeEntry("GUIWizardMode", m_wizard );
    config->writeEntry("FileListSorting", fileList->sorting() );

    config->sync();
}

void KRenameImpl::replace()
{
    ReplaceDialog* r = new ReplaceDialog( rep, parent );
    if( r->exec() == QDialog::Accepted )
    {
        rep = r->getList();
        updatePreview();
    }
}

void KRenameImpl::toggleImagePreview()
{
    KApplication::setOverrideCursor( Qt::waitCursor );
    fileList->setPreview( checkPreview->isChecked() );
    checkName->setEnabled( checkPreview->isChecked() );
    if( !checkPreview->isChecked() )
        checkName->setChecked( true );
    fileList->setName( checkName->isChecked() );
    KApplication::restoreOverrideCursor();
}

void KRenameImpl::toggleName()
{
    KApplication::setOverrideCursor( Qt::waitCursor );
    fileList->setName( checkName->isChecked() );
    KApplication::restoreOverrideCursor();
}

void KRenameImpl::preferences()
{
    ConfDialog* cd = new ConfDialog( parent, "cd" );
    cd->setLoadPlugins( m_loadplugins );
    cd->setThumbSize( fileList->getPreviewSize() );
    cd->setAutosize( m_autosize );
    cd->setHistoryItems( m_hist );
    cd->setUseWizard( m_wizard );

    if( cd->exec() == QDialog::Accepted ) {
        bool oldwiz = m_wizard;
    
        m_loadplugins = cd->loadplugins();
        m_autosize = cd->autosize();
        m_hist = cd->historyItems();
        m_wizard = cd->useWizard();

        refreshColumnMode();

//        fileList->setHiddenDirs( cd->hiddendir() );

        if( fileList->getPreviewSize() != cd->thumbSize() ) {
            fileList->setPreviewSize( cd->thumbSize() );
            fileList->setPreview( checkPreview->isChecked() );
        }

        updatePreview();
        updateHist();

        if( oldwiz != m_wizard )
            changeGUIMode();
    }
}

void KRenameImpl::changeGUIMode()
{
    /* GUI Mode was changed by the user */
    saveConfig();

    int x = parent->x();
    int y = parent->y();
    int w = parent->width();
    int h = parent->height();

    QWidget* oldparent = parent;
    QWidget* krename = KRenameImpl::launch( QRect( x, y, w, h ), QStringList(), this, false );
    
    m_switching = true;
    oldparent->removeChild( this );
    oldparent->hide();
    krename->insertChild( this );
    krename->show();

    oldparent->close();
    m_switching = false;            
}

void KRenameImpl::moveUp()
{
    QListViewItem* item = preview->selectedItem();
    if(item) {
        int i = preview->itemIndex( item );
        fileList->moveUp( i );
        updatePreview();
        if( (i-1) > 0 ) {
            preview->setCurrentItem( preview->itemAtIndex( i-1 ) );
            preview->ensureItemVisible( preview->itemAtIndex( i-1 ) );
        }
    }
}

void KRenameImpl::moveDown()
{
    QListViewItem* item = preview->selectedItem();
    if(item) {
        int i = preview->itemIndex( item );
        fileList->moveDown( i );
        updatePreview();
        preview->setCurrentItem( preview->itemAtIndex( i+1 ) );
        preview->ensureItemVisible( preview->itemAtIndex( i+1 ) );
    }
}

void KRenameImpl::undo()
{
    UndoDialog* u = new UndoDialog( parent );
    u->exec();
    delete u;
}

void KRenameImpl::changeUndoScriptName()
{
    const char* EXTENSION = ".krename";
    QString script = undorequester->url();
    if( script.right( 8 ) != EXTENSION ) {
        script += EXTENSION;
        undorequester->setURL( script );
    }
}

void KRenameImpl::pluginHelpChanged()
{
    QPtrListIterator<PluginLoader::PluginLibrary> it( plugin->libs ); // iterator for dict
    while ( it.current() ) {
        if( !(*it)->plugin->alwaysUsed() ) {
            (*it)->plugin->removeHelp( helpDialogData );

            if( (*it)->check )
            {
                (*it)->usePlugin = (*it)->check->isChecked();
                if( (*it)->check->isChecked() )
                    (*it)->plugin->addHelp( helpDialogData );
            }
        } 
        else
        {
            (*it)->plugin->removeHelp( helpDialogData );
            (*it)->plugin->addHelp( helpDialogData );
        }
        
        ++it;
    }
}

void KRenameImpl::showTokenHelp()
{
    helpDialog->show();
}

void KRenameImpl::getCoordinates()
{
    QString name;
    QListViewItem* item = preview->selectedItem();
    if( item )
        name = item->text( 0 );
    else {
        item = preview->itemAtIndex( 0 );
        if( item )
            name = item->text( 0 );
    }

    QFileInfo fi( name );
    QString extension;
    splitFilename( &fi, &name, &extension );
    CoordDialog* cd = new CoordDialog( name, parent );
    if( cd->exec() == QDialog::Accepted ) {
        QString t = filename->text();

        if( filename->text() == "$" )
            filename->setText( cd->coords() );
        else {
            t.insert( filename->cursorPosition(), cd->coords() );
            filename->setText( t );
        }
    }

    delete cd;
}

void KRenameImpl::splitFilename( QFileInfo* fi, QString* base, QString* extension )
{
    // first dot
    if( comboExtension->currentItem() == 0 ) {
        *base = fi->baseName();
        *extension = fi->extension( true );
    // last dot
    } else if( comboExtension->currentItem() == 1 ) {
        *extension = fi->extension( false );
        if( !extension->isEmpty() )
            *base = fi->fileName().left( fi->fileName().length() - extension->length() - 1 );
        else
            *base = fi->baseName();
    } else {
    // custom value
        int dot = comboExtension->currentText().toInt();

        QString file = FileOperation::getName( fi->filePath() );
        int p = 0;

        if( !file.contains( "." ) || !dot || dot > file.contains( "." ) ) {
            *base = file;
            *extension = "";
            return;
        }

        for( int i = 0; i < dot; i++ )
            p = file.find( ".", p + 1 );

        *base = file.left( p );
        *extension = file.right( file.length() - p - 1 );
    }
}

void KRenameImpl::refreshColumnMode()
{
    if( !m_autosize ) {
        preview->setColumnWidthMode( 0, QListView::Manual );
        preview->setColumnWidthMode( 1, QListView::Manual );
    } else {
        preview->setColumnWidthMode( 0, QListView::Maximum );
        preview->setColumnWidthMode( 1, QListView::Maximum );
    }
}

void KRenameImpl::updateHist()
{
    dirname->setMaxCount( m_hist );
    filename->setMaxCount( m_hist );
    extemplate->setMaxCount( m_hist );
}

void KRenameImpl::changeNumbers()
{
    NumberDialog* nd = new NumberDialog( skip, parent );
    nd->spinIndex->setValue( m_index );
    nd->spinStep->setValue( m_step );
    nd->checkResetCounter->setChecked( m_reset );

    if( nd->exec() ) {
        m_index = nd->spinIndex->value();
        m_step = nd->spinStep->value();
        skip = nd->getList();
        m_reset = nd->checkResetCounter->isChecked();

        updatePreview();
    }
}


void KRenameImpl::about()
{
    KAboutApplication * d = new KAboutApplication( kapp->aboutData(), parent );
    d->show();
}

void KRenameImpl::setWizardMode( bool mode )
{
    fileTab->raiseWidget( mode ? ID_WIZARD : ID_TAB );
    // Maybe we should create a WidgetStack here, too
    if( mode )
    {
        optionLink->hide();
        if( optionLink->isChecked() )
        {
            optionLink->setChecked( false );
            optionRename->setChecked( true );
            enableControls();
        }
    }
    else
        optionLink->show();
    
    if( mode )
        parseWizardMode();
}

void KRenameImpl::changeParent( QWidget* p, KMenuBar* m, QPushButton* finish, QRect r )
{
    parent = p;
    menuBar = m;
    finishButton = finish;

    connect( finishButton, SIGNAL( clicked()), this, SLOT( start() ));

    if( !r.isNull() ) {
        parent->resize( r.size() );
        parent->move( r.x(), r.y() );
    }
        
    setupActions();

    page_1->reparent( parent, QPoint( 0, 0 ) );
    page_2->reparent( parent, QPoint( 0, 0 ) );
    page_3->reparent( parent, QPoint( 0, 0 ) );
    page_4->reparent( parent, QPoint( 0, 0 ) );

    emit pageDone( page_1, i18n( pageTitle[0] ) );
    emit pageDone( page_2, i18n( pageTitle[1] ) );
    emit pageDone( page_3, i18n( pageTitle[2] ) );
    emit pageDone( page_4, i18n( pageTitle[3] ) );
}

QWidget* KRenameImpl::launch( QRect rect, const KURL::List & list, KRenameImpl* impl, bool loadprofile )
{
    KConfig* config = kapp->config();
    config->setGroup("GUISettings");
    bool firststart = config->readBoolEntry( "firststart", true );
    bool wizardmode = config->readBoolEntry( "GUIWizardMode", false );

    if( firststart ) {
        /* start the GUI Mode selction dialog */
        FirstStartDlg* fsd = new FirstStartDlg();
        fsd->exec();
        wizardmode = fsd->useWizard();

        config->setGroup("GUISettings");
        config->writeEntry( "firststart", false );
        config->writeEntry( "GUIWizardMode", wizardmode );
        config->sync();
    }

    QWidget* w = NULL;
    KRenameImpl* k = NULL;

    if( wizardmode ) {
        wizard* krename = new wizard( impl, rect );
        k = krename->getKRename();
        w = (QWidget*)krename;
    } else {
        tabs* krename = new tabs( impl, rect );
        k = krename->getKRename();
        w = (QWidget*)krename;
    }

    kapp->setMainWidget( w );

    for( unsigned int i = 0; i < list.count(); i++ )
        k->addFileOrDir( list[i] );

    k->updatePre();

    // it is time to load a default profile now (if the user has specified one)
    if( loadprofile && !k->hasCommandlineProfile() && ProfileManager::hasDefaultProfile() )
	ProfileManager::loadDefaultProfile( k );
    else if ( !k->hasCommandlineProfile() )
        w->show();

    return w;
}

void KRenameImpl::updateDots()
{
    int index = comboExtension->currentItem();
    comboExtension->clear();

    comboExtension->insertItem( i18n("First Dot") );
    comboExtension->insertItem( i18n("Last Dot") );

    unsigned int max = 0;
    for( unsigned int i = 0; i < fileList->count(); i++ ) {
        QString name = fileList->url( i ).fileName();
        int c = name.contains( "." );
        max = ( c > (int)max ? c : (int)max);
    }

    for( unsigned int i = 2; i <= max; i++ )
        comboExtension->insertItem( QString::number( i ) );

    comboExtension->setCurrentItem( index );
}

/** DCOP functions implementation
  */

void KRenameImpl::setFileNameTemplate( const QString & t )
{
    filename->setText( t );
}

const QString KRenameImpl::fileNameTemplate() const
{
    return filename->text();
}

void KRenameImpl::setExtensionTemplate( const QString & t )
{
    extemplate->setText( t );
}

const QString KRenameImpl::extensionTemplate() const
{
    return extemplate->text();
}

void KRenameImpl::setUseExtension( bool b )
{
    checkExtension->setChecked( b );
    enableControls();
}

bool KRenameImpl::useExtension() const
{
    return checkExtension->isChecked();
}

QStringList KRenameImpl::tokens() const
{
    return helpDialogData->tokens();
}

QString KRenameImpl::parseString( const QString & token, const QString & string )
{
    BatchRenamer b;
    b.setIndex( 0 );
    b.setStep( 0 );
   
    return b.processString( token, string, 0 );
}

void KRenameImpl::addDir( const QString & name, const QString & filter, bool recursive, bool hidden, bool dirnames )
{
    fileList->addDir( name, filter, hidden, recursive, dirnames );    
}

void KRenameImpl::setCounterStart( int index )
{
    m_index = index;
}

void KRenameImpl::loadFilePlugins()
{
    KApplication::setOverrideCursor( Qt::WaitCursor );        
    plugin->loadPlugins( true );
    KApplication::restoreOverrideCursor();
    pluginHelpChanged();
    
    enableControls();
}

void KRenameImpl::reloadFilePluginData()
{
    plugin->clearCache();
    updatePreview();
}

void KRenameImpl::manageProfiles()
{
    ProfileDlg dlg( this, parent, "dlg" );
    dlg.exec();
}

void KRenameImpl::getHelpDialogString( QLineEdit* edit )
{
    HelpDialog hdlg( helpDialogData, parent, "hdlg", true );
    hdlg.setLineEdit( edit );
    hdlg.updateHeadline();
    hdlg.updateItems();
    hdlg.exec();
}

void KRenameImpl::slotEasy1()
{
    getHelpDialogString( comboPrefix->lineEdit() );
}

void KRenameImpl::slotEasy2()
{
    getHelpDialogString( comboSuffix->lineEdit() );
}

void KRenameImpl::slotEasy3()
{
    getHelpDialogString( comboCustom->lineEdit() );
}

void KRenameImpl::slotEasy4()
{
    getHelpDialogString( comboCustomExtension->lineEdit() );
}
#endif // 0
