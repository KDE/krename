/***************************************************************************
                   scriptplugin.cpp  -  description
                             -------------------
    begin                : Fri Nov 9 2007
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

#include "scriptplugin.h"

#include <kapplication.h>
#include <kconfiggroup.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <klistwidget.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ktemporaryfile.h>

#include <kio/netaccess.h>

#include <QFile>
#include <QLabel>
#include <QHBoxLayout>
#include <QMenu>
#include <QTextStream>

#include <kjs/kjsinterpreter.h>

#include "ui_scriptplugindialog.h"
#include "ui_scriptpluginwidget.h"
#include "batchrenamer.h"
#include "krenamefile.h"

const char* ScriptPlugin::s_pszFileDialogLocation = "kfiledialog://krenamejscript";
const char* ScriptPlugin::s_pszVarNameIndex       = "krename_index";
const char* ScriptPlugin::s_pszVarNameUrl         = "krename_url";
const char* ScriptPlugin::s_pszVarNameFilename    = "krename_filename";
const char* ScriptPlugin::s_pszVarNameExtension   = "krename_extension";
const char* ScriptPlugin::s_pszVarNameDirectory   = "krename_directory";


ScriptPlugin::ScriptPlugin( PluginLoader* loader )
    : QObject(), 
      Plugin( loader ), m_parent( NULL )
{
    m_name = i18n("JavaScript Plugin");
    m_icon = "applications-development";
    m_interpreter = new KJSInterpreter();
    m_menu   = new QMenu();
    m_widget = new Ui::ScriptPluginWidget();

    this->addSupportedToken("js;.*");

    m_help.append( "[js;4+5];;" + i18n("Insert a snippet of JavaScript code (4+5 in this case)") );

    m_menu->addAction( i18n("Index of the current file"),     this, SLOT(slotInsertIndex()));
    m_menu->addAction( i18n("URL of the current file"),       this, SLOT(slotInsertUrl()));
    m_menu->addAction( i18n("Filename of the current file"),  this, SLOT(slotInsertFilename()));
    m_menu->addAction( i18n("Extension of the current file"), this, SLOT(slotInsertExtension()));
    m_menu->addAction( i18n("Directory of the current file"), this, SLOT(slotInsertDirectory()));
}

ScriptPlugin::~ScriptPlugin()
{
    delete m_widget;
    delete m_menu;
    delete m_interpreter;
}

QString ScriptPlugin::processFile( BatchRenamer* b, int index, 
				   const QString & filenameOrToken, EPluginType )
{
    QString token( filenameOrToken );
    QString script;
    QString definitions = m_widget->textCode->toPlainText();

    if( token.contains( ";" ) )
    {
        script = token.section( ';', 1 ); // all sections from 1 to the last
        token  = token.section( ';', 0, 0 ).toLower();
    } else 
        token = token.toLower();

    if( token == "js" ) 
    {
	// Setup interpreter
	const KRenameFile & file = b->files()->at( index );
	initKRenameVars( file, index );

	// Make sure definitions are executed first
	script = definitions + "\n" + script;

	KJSResult result = m_interpreter->evaluate( script, NULL );
	if( result.isException() )
	{
	    qDebug( "JavaScript Error: %s", result.errorMessage().toUtf8().data() );
	    return QString::null;
	}

	return result.value().toString( m_interpreter->globalContext() );
    }

    return QString::null;
}

const QPixmap ScriptPlugin::icon() const
{
    return KIconLoader::global()->loadIcon( m_icon, KIconLoader::NoGroup, KIconLoader::SizeSmall );
}

void ScriptPlugin::createUI( QWidget* parent ) const
{
    QStringList labels;
    labels << i18n("Variable Name");
    labels << i18n("Initial Value");

    const_cast<ScriptPlugin*>(this)->m_parent = parent;
    m_widget->setupUi( parent );
    m_widget->listVariables->setColumnCount( 2 );
    m_widget->listVariables->setHeaderLabels( labels );

    connect( m_widget->listVariables, SIGNAL(itemSelectionChanged()), SLOT(slotEnableControls()) );
    connect( m_widget->buttonAdd,     SIGNAL(clicked(bool)), SLOT(slotAdd()) );
    connect( m_widget->buttonRemove,  SIGNAL(clicked(bool)), SLOT(slotRemove()) );
    connect( m_widget->buttonLoad,    SIGNAL(clicked(bool)), SLOT(slotLoad()) );
    connect( m_widget->buttonSave,    SIGNAL(clicked(bool)), SLOT(slotSave()) );
    connect( m_widget->textCode,      SIGNAL(textChanged()), SLOT(slotEnableControls()) );

    const_cast<ScriptPlugin*>(this)->slotEnableControls();

    QPixmap openIcon   = KIconLoader::global()->loadIcon( "document-open", KIconLoader::NoGroup, KIconLoader::SizeSmall );
    QPixmap saveIcon   = KIconLoader::global()->loadIcon( "document-save-as", KIconLoader::NoGroup, KIconLoader::SizeSmall );
    QPixmap removeIcon = KIconLoader::global()->loadIcon( "list-remove", KIconLoader::NoGroup, KIconLoader::SizeSmall );
    QPixmap addIcon    = KIconLoader::global()->loadIcon( "list-add", KIconLoader::NoGroup, KIconLoader::SizeSmall );

    m_widget->buttonLoad->setIcon( openIcon );
    m_widget->buttonSave->setIcon( saveIcon );
    m_widget->buttonAdd->setIcon( addIcon );
    m_widget->buttonRemove->setIcon( removeIcon );

    m_widget->buttonInsert->setMenu( m_menu );
}

void ScriptPlugin::initKRenameVars( const KRenameFile & file, int index )
{
    // KRename definitions
    m_interpreter->globalObject().setProperty( m_interpreter->globalContext(), 
					       ScriptPlugin::s_pszVarNameIndex,
					       index );
    m_interpreter->globalObject().setProperty( m_interpreter->globalContext(),
					       ScriptPlugin::s_pszVarNameUrl,
					       file.srcUrl().url() );
    m_interpreter->globalObject().setProperty( m_interpreter->globalContext(),
					       ScriptPlugin::s_pszVarNameFilename,
					       file.srcFilename() );
    m_interpreter->globalObject().setProperty( m_interpreter->globalContext(),
					       ScriptPlugin::s_pszVarNameExtension,
					       file.srcExtension() );
    m_interpreter->globalObject().setProperty( m_interpreter->globalContext(),
					       ScriptPlugin::s_pszVarNameDirectory, 
					       file.srcDirectory() );


    // User definitions, set them only on first file
    if( index == 0 ) 
    {
	for( int i=0; i<m_widget->listVariables->topLevelItemCount(); i++ )
	{
	    // TODO, we have to know the type of the variable!
	    QTreeWidgetItem* item = m_widget->listVariables->topLevelItem( i );
	    if( item ) 
	    {
		m_interpreter->globalObject().setProperty( m_interpreter->globalContext(),
							   item->text( 0 ),
							   item->text( 1 ).toUtf8().data() );
	    }
	}
    }
}

void ScriptPlugin::insertVariable( const char* name )
{
    m_widget->textCode->insertPlainText( QString( name ) );
}

void ScriptPlugin::slotEnableControls()
{
    bool bEnable = (m_widget->listVariables->selectedItems().count() != 0);
    m_widget->buttonRemove->setEnabled( bEnable );

    bEnable = !m_widget->textCode->toPlainText().isEmpty();
    m_widget->buttonSave->setEnabled( bEnable );
}

void ScriptPlugin::slotAdd()
{
    QDialog                dialog;
    Ui::ScriptPluginDialog dlg;

    dlg.setupUi( &dialog );
    
    if( dialog.exec() == QDialog::Accepted ) 
    {
	QString name  = dlg.lineName->text();
	QString value = dlg.lineValue->text();

	// Build a Java script statement
	QString script = name + " = " + value + ";";
	
	KJSInterpreter interpreter;
	KJSResult result = m_interpreter->evaluate( script, NULL );
	if( result.isException() )
	{
	    KMessageBox::error( m_parent, 
				i18n("A JavaScript error has occured: ") +
				result.errorMessage(), this->name() );
	}
	else
	{
	    QTreeWidgetItem* item = new QTreeWidgetItem();
	    item->setText( 0, name );
	    item->setText( 1, value );

	    m_widget->listVariables->addTopLevelItem( item );
	}
    }
}

void ScriptPlugin::slotRemove()
{
    QTreeWidgetItem* item = m_widget->listVariables->currentItem();
    if( item ) 
    {
	m_widget->listVariables->invisibleRootItem()->removeChild( item );
	delete item;
    }
}

void ScriptPlugin::slotLoad()
{
    if( !m_widget->textCode->toPlainText().isEmpty() && 
	KMessageBox::questionYesNo( m_parent, 
				    i18n("All currently entered definitions will be lost. Do you want to continue?") ) 
	== KMessageBox::No )
    {
	return;
    }

    KFileDialog dialog( KUrl(ScriptPlugin::s_pszFileDialogLocation), 
			i18n("*|All files and directories"), 
			m_parent );
    dialog.setOperationMode( KFileDialog::Opening );
    dialog.setMode( KFile::File | KFile::ExistingOnly );

    if( dialog.exec() == QDialog::Accepted ) 
    {
	// Also support remote files
	QString tmpFile;
	if( KIO::NetAccess::download( dialog.selectedUrl(), tmpFile, m_parent ) )
	{
	    QFile file(tmpFile);
	    if( file.open(QFile::ReadOnly | QFile::Text) )
	    {
		QTextStream in(&file);
		QString text = in.readAll();
		m_widget->textCode->setPlainText( text );

		file.close();
	    }
	    else
		KMessageBox::error(m_parent, i18n("Unable to open %1 for reading.", tmpFile ) );

	    KIO::NetAccess::removeTempFile( tmpFile );
	}
	else 
	    KMessageBox::error(m_parent, KIO::NetAccess::lastErrorString() );
    }

    slotEnableControls();
}

void ScriptPlugin::slotSave()
{
    KFileDialog dialog( KUrl(ScriptPlugin::s_pszFileDialogLocation), 
			i18n("*|All files and directories"), 
			m_parent );
    dialog.setOperationMode( KFileDialog::Saving );
    dialog.setMode( KFile::File );

    if( dialog.exec() == QDialog::Accepted ) 
    {
	const KUrl url = dialog.selectedUrl();
	if( KIO::NetAccess::exists( url, KIO::NetAccess::DestinationSide, m_parent ) )
	{
	    int m = KMessageBox::warningYesNo( m_parent, i18n("The file %1 does already exists. "
							      "Do you want to overwrite it?", url.prettyUrl()) );

	    if( m == KMessageBox::No )
		return;
	}

	QString tmpName = url.path();
	if( !url.isLocalFile() )
	{
	    KTemporaryFile temp;
	    tmpName = temp.fileName();
	}


	QFile file(tmpName);
	if( file.open(QIODevice::WriteOnly | QIODevice::Text) )
	{
	    QTextStream out(&file);
	    out << m_widget->textCode->toPlainText();
	    out.flush();
	    file.close();

	    if( !url.isLocalFile() )
	    {
		if( !KIO::NetAccess::upload( tmpName, url, m_parent ) )
		    KMessageBox::error(m_parent, KIO::NetAccess::lastErrorString() );

		file.remove();
	    }
	}
	else
	{
	    KMessageBox::error(m_parent, i18n("Unable to open %1 for writing.", tmpName ) );
	    if( !url.isLocalFile() )
		file.remove();
	}
    }

    slotEnableControls();
}

void ScriptPlugin::slotTest()
{
}

void ScriptPlugin::slotInsertIndex()
{
    this->insertVariable( ScriptPlugin::s_pszVarNameIndex ); 
}

void ScriptPlugin::slotInsertUrl()
{
    this->insertVariable( ScriptPlugin::s_pszVarNameUrl ); 
}

void ScriptPlugin::slotInsertFilename()
{
    this->insertVariable( ScriptPlugin::s_pszVarNameFilename ); 
}

void ScriptPlugin::slotInsertExtension()
{
    this->insertVariable( ScriptPlugin::s_pszVarNameExtension ); 
}

void ScriptPlugin::slotInsertDirectory()
{
    this->insertVariable( ScriptPlugin::s_pszVarNameDirectory ); 
}

void ScriptPlugin::loadConfig( KConfigGroup & group )
{
    QStringList variableNames;
    QStringList variableValues;

    variableNames  = group.readEntry( "JavaScriptVariableNames",  variableNames );
    variableValues = group.readEntry( "JavaScriptVariableValues", variableValues );

    int min = qMin( variableNames.count(), variableValues.count() );

    for( int i=0; i < min; i++ )
    {
	QTreeWidgetItem* item = new QTreeWidgetItem();
	item->setText( 0, variableNames[i] );
	item->setText( 1, variableValues[i] );
	
	m_widget->listVariables->addTopLevelItem( item );
    }

    m_widget->textCode->setPlainText( group.readEntry( "JavaScriptDefinitions", QString() ) );
}

void ScriptPlugin::saveConfig( KConfigGroup & group ) const
{
    QStringList variableNames;
    QStringList variableValues;
    
    for( int i=0; i<m_widget->listVariables->topLevelItemCount(); i++ )
    {
	QTreeWidgetItem* item = m_widget->listVariables->topLevelItem( i );
	if( item ) 
	{
	    variableNames  << item->text( 0 );
	    variableValues << item->text( 1 );
	}
    }

    group.writeEntry( "JavaScriptVariableNames",  variableNames );
    group.writeEntry( "JavaScriptVariableValues", variableValues );
    group.writeEntry( "JavaScriptDefinitions", m_widget->textCode->toPlainText() );
}

