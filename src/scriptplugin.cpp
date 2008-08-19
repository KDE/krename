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
#include <kiconloader.h>
#include <klistwidget.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <QLabel>
#include <QHBoxLayout>

#include <kjs/kjsinterpreter.h>

#include "ui_scriptplugindialog.h"
#include "ui_scriptpluginwidget.h"
#include "batchrenamer.h"
#include "krenamefile.h"

ScriptPlugin::ScriptPlugin( PluginLoader* loader )
    : QObject(), 
      Plugin( loader ), m_parent( NULL )
{
    m_name = i18n("JavaScript Plugin");
    m_icon = "applications-development";
    m_interpreter = new KJSInterpreter();
    m_widget = new Ui::ScriptPluginWidget();

    this->addSupportedToken("js;.*");

    m_help.append( "[js;4+5];;" + i18n("Insert a snippet of JavaScript code (4+5 in this case)") );
}

ScriptPlugin::~ScriptPlugin()
{
    delete m_widget;
    delete m_interpreter;
}

QString ScriptPlugin::processFile( BatchRenamer* b, int index, 
				   const QString & filenameOrToken, EPluginType )
{
    QString token( filenameOrToken );
    QString script;

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
    connect( m_widget->buttonTest,    SIGNAL(clicked(bool)), SLOT(slotTest()) );

    const_cast<ScriptPlugin*>(this)->slotEnableControls();
}

void ScriptPlugin::initKRenameVars( const KRenameFile & file, int index )
{
    m_interpreter->globalObject().setProperty( m_interpreter->globalContext(), 
					       "krename_index", index );
    m_interpreter->globalObject().setProperty( m_interpreter->globalContext(),
					       "krename_url", file.srcUrl().url() );
    m_interpreter->globalObject().setProperty( m_interpreter->globalContext(),
					       "krename_filename", file.srcFilename() );
    m_interpreter->globalObject().setProperty( m_interpreter->globalContext(),
					       "krename_extension", file.srcExtension() );
    m_interpreter->globalObject().setProperty( m_interpreter->globalContext(),
					       "krename_directory", file.srcDirectory() );
}

void ScriptPlugin::slotEnableControls()
{
    bool bEnable = (m_widget->listVariables->selectedItems().count() != 0);

    m_widget->buttonRemove->setEnabled( bEnable );
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
}

void ScriptPlugin::slotSave()
{
}

void ScriptPlugin::slotTest()
{
}


void ScriptPlugin::loadConfig( KConfigGroup & group )
{

}

void ScriptPlugin::saveConfig( KConfigGroup & group ) const
{

}
