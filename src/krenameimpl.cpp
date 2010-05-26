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
      m_lastSplitMode( eSplitMode_LastDot ), m_lastDot( 0 ),
      m_runningThreadedListersCount( 0 )
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
    //KRenameImpl* impl = new KRenameImpl( w, list );
    new KRenameImpl( w, list );
	// Windows shows KRename otherwise outside of the visible
	// screen area
	if( !rect.isNull() )
		w->setGeometry( rect );

    /*
    // it is time to load a default profile now (if the user has specified one)
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
    // Disabled profiles as they are not implemented right now
    //KMenu* mnuExtra    = new KMenu( i18n("E&xtras"), m_window );

    KMenu* mnuHelp     = m_window->helpMenu( QString::null, true );

    //KAction* actProfiles = new KAction( i18n("&Profiles..."), m_window );

    //m_window->menuBar()->addMenu( mnuExtra );
    m_window->menuBar()->addMenu( mnuHelp );

    //mnuExtra->addAction( actProfiles );

    //connect(actProfiles, SIGNAL(triggered(bool)), SLOT(slotManageProfiles()));
}

void KRenameImpl::setupSlots()
{
    connect( m_window, SIGNAL(addFiles()),       SLOT(slotAddFiles()));
    connect( m_window, SIGNAL(removeFiles()),    SLOT(slotRemoveFiles()));
    connect( m_window, SIGNAL(removeAllFiles()), SLOT(slotRemoveAllFiles()));

    connect( m_window, SIGNAL(updatePreview()),  SLOT(slotUpdatePreview()));
    connect( m_window, SIGNAL(updateCount()),    SLOT(slotUpdateCount()));

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
    KRenameFile       item( url, m_lastSplitMode, m_lastDot );
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
        KRenameFile item( *it, m_lastSplitMode, m_lastDot );
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

            m_runningThreadedListersCount++;
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
    bool gotFilenames = false;
    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();

    if( args->isSet( "test" ) )
        QTimer::singleShot( 0, this, SLOT( selfTest() ) );

    // Add all recursive directoris
    KUrl::List recursiveList;
    QStringList optlist = args->getOptionList ( "r" );
    for (QStringList::ConstIterator it=optlist.begin(); it!=optlist.end(); ++it)
    {

        KUrl url;
        url.setPath( *it );

        qDebug("Adding recursive: %s", (*it).toUtf8().data());
        recursiveList.append( url  );
    }

    if( !recursiveList.isEmpty() ) 
    {
        gotFilenames = true;
        // Add all directories recursive, but no hiden files
        this->addFilesOrDirs( recursiveList, "*", true, false, false, false );
    }


    // Add all files from the commandline options
    KUrl::List list;
    for( int i = 0; i < args->count(); i++)
        list.append( args->url( i ) );

    if( !list.isEmpty() ) 
    {
        gotFilenames = true;
    }

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
    */

    QString templ = args->getOption( "template" );
    if( !templ.isEmpty() )
        m_window->setFilenameTemplate( templ, false );

    QString extension = args->getOption( "extension" );
    if( !extension.isEmpty() ) 
        m_window->setExtensionTemplate( extension, false );

    QString copyDir = args->getOption( "copy" );
    if( !copyDir.isEmpty() )
    {
        m_window->setRenameMode( eRenameMode_Copy );
        m_window->setDestinationUrl( KUrl( copyDir ) );
    }

    QString moveDir = args->getOption( "move" );
    if( !moveDir.isEmpty() )
    {
        m_window->setRenameMode( eRenameMode_Move );
        m_window->setDestinationUrl( KUrl( moveDir ) );
    }

    QString linkDir = args->getOption( "link" );
    if( !linkDir.isEmpty() )
    {
        m_window->setRenameMode( eRenameMode_Link );
        m_window->setDestinationUrl( KUrl( linkDir ) );
    }

/*        
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

*/
    bool startnow = args->isSet( "start" );

    // Free some memory
    args->clear();      

    
    if( gotFilenames )
    {
        // we got already filenames over the commandline, so show directly the last
        // page of the wizard
        m_window->showFilenameTab();
    }

    if( startnow ) 
    {
        qDebug("Waiting for listenters: %i\n", m_runningThreadedListersCount );
        // As file adding runs in a another trhread,
        // there might be adding in progress but not yet
        // all files in the list.
        // so let's wait for file adding to finish first
        // before starting.
        while( m_runningThreadedListersCount > 0 )
            kapp->processEvents();

        if( m_vector.count() > 0 )
            // start renaming
            QTimer::singleShot( 200, this, SLOT( slotStart() ) );
    }
}


void KRenameImpl::slotAddFiles()
{
    FileDialogExtWidget* widget = new FileDialogExtWidget();
    KFileDialog dialog( KUrl("kfiledialog://krename"), 
                        i18n("*|All files and directories"), 
                        m_window, widget );
    dialog.setOperationMode( KFileDialog::Opening );
    dialog.setMode( KFile::Files | KFile::Directory | KFile::ExistingOnly );

    if( dialog.exec() == QDialog::Accepted ) 
    {
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

    qDebug("Listener Done ListenersCount: %i", m_runningThreadedListersCount);

    m_runningThreadedListersCount--;

    if( m_runningThreadedListersCount < 0 ) 
    {
        // To be safe
        qDebug("m_runningThreadedListersCount=%i", m_runningThreadedListersCount);
        m_runningThreadedListersCount = 0;
    }
} 

void KRenameImpl::slotTokenHelpDialog(QLineEdit* edit)
{
    TokenHelpDialog dialog( m_model, &m_renamer, edit, m_window );

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

    m_model->setExtensionSplitMode( m_lastSplitMode, m_lastDot );
}

void KRenameImpl::slotStart()
{
    ProgressDialog* progress = new ProgressDialog( m_lastSplitMode, m_lastDot );
    progress->print( i18n("Starting conversion of %1 files.", m_vector.count()) );

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

    // Make sure the GUI will not delete our models
    m_window->setModel( NULL );
    m_window->setPreviewModel( NULL );

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
	groupGui.readEntry( "ImagePreview2", QVariant(true) ).toBool() );

    m_window->setPreviewNamesEnabled( 
	groupGui.readEntry( "ImagePreviewName2", QVariant(true) ).toBool() );

    m_window->setAdvancedMode( 
	groupGui.readEntry( "Advanced", QVariant(false) ).toBool() );

    int index = groupGui.readEntry( "StartIndex", QVariant(1) ).toInt();
    int step  = groupGui.readEntry( "Stepping", QVariant(1) ).toInt();

    m_renamer.setNumberStepping( step );
    // Will call batch renamer
    m_window->setNumberStartIndex( index );

    int sortMode = groupGui.readEntry( "FileListSorting", QVariant(0) ).toInt();
    m_window->setSortMode( sortMode );

    ESplitMode lastSplitMode = static_cast<ESplitMode>(groupGui.readEntry( "ExtensionSplitMode", static_cast<int>(m_lastSplitMode) ));
    int lastDot = groupGui.readEntry( "ExtensionSplitDot", m_lastDot );    
    m_window->setExtensionSplitMode( lastSplitMode, lastDot );
    this->slotExtensionSplitModeChanged( lastSplitMode, lastDot );
    
    // load Plugin configuration
    KConfigGroup groupPlugins = config->group( QString("PluginSettings") );
    m_pluginLoader->loadConfig( groupPlugins );

    m_window->loadConfig();
}

void KRenameImpl::saveConfig() 
{
    // Me might get a saveConfig signal because of signals and slots
    // even if m_window was already delted. So ignore these events
    if(!m_window) 
        return;

    m_window->saveConfig();

    KSharedConfigPtr config = KGlobal::config();


    KConfigGroup groupGui = config->group( QString("GUISettings") );
    groupGui.writeEntry( "firststart4", false );
    groupGui.writeEntry( "ImagePreview2", m_window->isPreviewEnabled() );
    groupGui.writeEntry( "ImagePreviewName2", m_window->isPreviewNamesEnabled() );
    groupGui.writeEntry( "StartIndex", m_window->numberStartIndex() );
    groupGui.writeEntry( "Stepping", m_renamer.numberStepping() );
    groupGui.writeEntry( "FileListSorting", m_window->sortMode() );
    groupGui.writeEntry( "Advanced", m_window->isAdvancedMode() );
    groupGui.writeEntry( "ExtensionSplitMode", static_cast<int>(m_lastSplitMode) );
    groupGui.writeEntry( "ExtensionSplitDot", m_lastDot );    

    // save Plugin configuration
    KConfigGroup groupPlugins = config->group( QString("PluginSettings") );
    m_pluginLoader->saveConfig( groupPlugins );

    config->sync();
}

