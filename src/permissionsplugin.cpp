/***************************************************************************
                      permissionsplugin.cpp  -  description
                             -------------------
    begin                : Sun Mar 9 2008
    copyright            : (C) 2002 by Dominik Seichter
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

#include "permissionsplugin.h"

#include <kiconloader.h>
#include <klocale.h>

#include "ui_permissionspluginwidget.h"

#include <QDialogButtonBox>

// OS includes
#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <unistd.h>

// Only maxentries users are listed in the plugin
// increase if you need more
#define MAXENTRIES 1000

PermissionsPlugin::PermissionsPlugin( PluginLoader* loader )
    : QObject( NULL ), Plugin( loader ), m_curPermission( S_IRUSR | S_IWUSR | S_IRGRP )
{
    m_widget = new Ui::PermissionsPluginWidget();

    int i;
    uid_t uid = getuid();

    // Get all users on the system
    struct passwd* user;
    setpwent();
    for ( i=0; ((user = getpwent()) != 0L) && (i < MAXENTRIES); i++)
        if( uid == 0 || uid == user->pw_uid )
        {
            m_users.append( QString::fromLatin1(user->pw_name) );
        }
    endpwent();

    // Get all groups on the system
    struct group *ge;
    user = getpwuid( uid );
    setgrent();
    for (i=0; ((ge = getgrent()) != 0L) && (i < MAXENTRIES); i++) 
    {
        if( uid == 0 ) 
        {
            // Add all groups if we are run as root
            m_groups.append( QString::fromLatin1(ge->gr_name) );
        } 
        else 
        {
            // If the current user is member of this group: add it
            char** members = ge->gr_mem;
            char* member;
           
            while( (member = *members) != 0L ) 
            {
                if( strcmp(user->pw_name,member) == 0  ) 
                {
                    m_groups.append( QString::fromLatin1(ge->gr_name) );
                    break;
                }

                ++members;
            }
        }
    }
    endgrent();

    // add the users group
    ge = getgrgid ( user->pw_gid );
    if( ge ) 
    {
        QString name = QString::fromLatin1(ge->gr_name);
        if (name.isEmpty())
            name.setNum(ge->gr_gid);

        m_groups.append( name );
    }

    // sort both lists
    m_users.sort();
    m_groups.sort();
}

PermissionsPlugin::~PermissionsPlugin()
{

}

const QString PermissionsPlugin::name() const
{
    return i18n("Permissions");
}

const QPixmap PermissionsPlugin::icon() const
{
    return KIconLoader::global()->loadIcon( "document-properties", KIconLoader::NoGroup, KIconLoader::SizeSmall );
}

QString PermissionsPlugin::processFile( BatchRenamer*, int, const QString & filenameOrToken, EPluginType )
{
    const QString & filename = filenameOrToken;

    if( !KUrl( filename ).isLocalFile() ) 
        return i18n("PermissionsPlugin works only with local files. %1 is a remote file.", filename);

    if( m_widget->checkPermissions->isChecked() )
    {
        if( chmod( filename.toUtf8().data(), (mode_t)m_curPermission ) == -1 )
            return i18n("Can't chmod %1.", filename);
    }

    if( m_widget->checkOwner->isChecked() )
    {
        uid_t uid = getUid( m_widget->comboUser->currentText() );
        gid_t gid = getGid( m_widget->comboGroup->currentText() );

        if( chown( filename.toUtf8().data(), uid, gid ) )
            return i18n("Can't chown %1.", filename);
    }

    return QString::null;
}

void PermissionsPlugin::createUI( QWidget* parent ) const
{
    m_widget->setupUi( parent );

    m_widget->labelAdvanced->setVisible( false );

    m_widget->comboUser->insertItems( 0, m_users );
    m_widget->comboGroup->insertItems( 0, m_groups );

    m_widget->comboPermOwner->setCurrentIndex( 2 );
    m_widget->comboPermGroup->setCurrentIndex( 1 );
    m_widget->comboPermOthers->setCurrentIndex( 0 );

    connect( m_widget->checkOwner, SIGNAL( clicked(bool) ), SLOT( slotEnableControls() ) ); 
    connect( m_widget->checkPermissions, SIGNAL( clicked(bool) ), SLOT( slotEnableControls() ) ); 
    connect( m_widget->pushButton, SIGNAL( clicked(bool) ), SLOT( slotAdvancedPermissions() ) ); 
    connect( m_widget->comboPermOwner, SIGNAL( activated(int) ), SLOT( slotUpdatePermissions() ) ); 
    connect( m_widget->comboPermGroup, SIGNAL( activated(int) ), SLOT( slotUpdatePermissions() ) ); 
    connect( m_widget->comboPermOthers, SIGNAL( activated(int) ), SLOT( slotUpdatePermissions() ) ); 
    connect( m_widget->checkFolder, SIGNAL( clicked(bool) ), SLOT( slotUpdatePermissions() ) );
}

void PermissionsPlugin::slotEnableControls()
{
    m_widget->groupOwner->setEnabled( m_widget->checkOwner->isChecked() );
    m_widget->groupPermissions->setEnabled( m_widget->checkPermissions->isChecked() );
}

void PermissionsPlugin::slotAdvancedPermissions() 
{
    QDialog dialog;

    QLabel *la, *cl[3];
    QGridLayout* gl;
    QCheckBox* permBox[3][4];

    QVBoxLayout* layout = new QVBoxLayout( &dialog );
    QGroupBox* groupPermission = new QGroupBox ( i18n("Access permissions"), &dialog );

    gl = new QGridLayout (groupPermission);
    //gl->addRowSpacing(0, 10);

    la = new QLabel(i18n("Class"), groupPermission);
    gl->addWidget(la, 1, 0);

    la = new QLabel( i18n("Read"), groupPermission );
    gl->addWidget (la, 1, 1);

    la = new QLabel( i18n("Write"), groupPermission );
    gl->addWidget (la, 1, 2);

    la = new QLabel( i18n("Exec"), groupPermission );
    QSize size = la->sizeHint();
    size.setWidth(size.width() + 15);
    la->setFixedSize(size);
    gl->addWidget (la, 1, 3);

    la = new QLabel( i18n("Special"), groupPermission );
    gl->addWidget(la, 1, 4);

    cl[0] = new QLabel( i18n("User"), groupPermission );
    gl->addWidget (cl[0], 2, 0);

    cl[1] = new QLabel( i18n("Group"), groupPermission );
    gl->addWidget (cl[1], 3, 0);

    cl[2] = new QLabel( i18n("Others"), groupPermission );
    gl->addWidget (cl[2], 4, 0);

    la = new QLabel(i18n("UID"), groupPermission);
    gl->addWidget(la, 2, 5);

    la = new QLabel(i18n("GID"), groupPermission);
    gl->addWidget(la, 3, 5);

    la = new QLabel(i18n("Sticky"), groupPermission);
    gl->addWidget(la, 4, 5);

    int fperm[3][4] = {
        {S_IRUSR, S_IWUSR, S_IXUSR, S_ISUID},
        {S_IRGRP, S_IWGRP, S_IXGRP, S_ISGID},
        {S_IROTH, S_IWOTH, S_IXOTH, S_ISVTX}
    };

    for (int row = 0; row < 3 ; ++row) {
        for (int col = 0; col < 4; ++col) {
            QCheckBox *cb = new QCheckBox(groupPermission);
            permBox[row][col] = cb;
            gl->addWidget (permBox[row][col], row+2, col+1);

            cb->setChecked( fperm[row][col] & m_curPermission );
        }
    }

    QDialogButtonBox* box = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog );
    connect( box, SIGNAL( accepted() ), &dialog, SLOT(accept() ) );
    connect( box, SIGNAL( rejected() ), &dialog, SLOT(reject() ) );

    layout->addWidget( groupPermission );
    layout->addWidget( box );

    if( dialog.exec() == QDialog::Accepted ) 
    {
        m_curPermission = 0;
        for (int row = 0;row < 3; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                if( permBox[row][col]->isChecked() )
                    m_curPermission |= fperm[row][col];
            }
        }

        //setCurrentPermissions( m_curPermission );
        m_widget->labelAdvanced->setVisible( true );
        m_widget->comboPermOwner->setEnabled( false );
        m_widget->comboPermGroup->setEnabled( false );
        m_widget->comboPermOthers->setEnabled( false );
        m_widget->checkFolder->setEnabled( false );
    }
}

void PermissionsPlugin::slotUpdatePermissions()
{
    int fpermUser [3] = { 0, S_IRUSR, S_IRUSR | S_IWUSR };
    int fpermGroup[3] = { 0, S_IRGRP, S_IRGRP | S_IWGRP };
    int fpermOther[3] = { 0, S_IROTH, S_IROTH | S_IWOTH };

    m_curPermission = 0;
    m_curPermission |= (fpermUser[m_widget->comboPermOwner->currentIndex()]);
    m_curPermission |= (fpermGroup[m_widget->comboPermGroup->currentIndex()]);
    m_curPermission |= (fpermOther[m_widget->comboPermOthers->currentIndex()]);

    m_widget->checkFolder->setTristate( false );
    if( m_widget->checkFolder->isChecked() ) 
    {
        m_widget->checkFolder->setChecked( true );
        m_curPermission |= S_IXUSR;
        m_curPermission |= S_IXGRP;
        m_curPermission |= S_IXOTH;
    }
}

int PermissionsPlugin::getGid( const QString & group ) const
{
    int i, r = 0;
    struct group *ge;
    setgrent();
    for (i=0; ((ge = getgrent()) != 0L) && (i < MAXENTRIES); i++)
        if( !strcmp( ge->gr_name, group.toUtf8().data() ) )
        {
            r = ge->gr_gid;
            break;
        }

    endgrent();
    return r;
}

int PermissionsPlugin::getUid( const QString & owner ) const
{
    int i, r = 0;
    struct passwd *user;
    setpwent();
    for (i=0; ((user = getpwent()) != 0L) && (i < MAXENTRIES); i++)
        if( !strcmp(user->pw_name, owner.toUtf8().data()) )
        {
            r = user->pw_uid;
            break;
        }

    endpwent();
    return r;
}

/*
void PermissionsPlugin::setCurrentPermissions( int perm ) 
{
    m_widget->comboPermOwner->setCurrentIndex( 0 );
    m_widget->comboPermGroup->setCurrentIndex( 0 );
    m_widget->comboPermOthers->setCurrentIndex( 0 );

    int fpermUser [3] = { 0, S_IRUSR, S_IRUSR | S_IWUSR };
    int fpermGroup[3] = { 0, S_IRGRP, S_IRGRP | S_IWGRP };
    int fpermOther[3] = { 0, S_IROTH, S_IROTH | S_IWOTH };
    
    int i;
    for( i=2; i>=0; i-- )
    {
        if( (fpermUser[i] & perm) ) 
        {
            m_widget->comboPermOwner->setCurrentIndex( i );
            break;
        }
    }

    for( i=2; i>=0; i-- )
    {
        if( (fpermGroup[i] & perm) ) 
        {
            m_widget->comboPermGroup->setCurrentIndex( i );
            break;
        }
    }

    for( i=2; i>=0; i-- )
    {
        if( (fpermOther[i] & perm) ) 
        {
            m_widget->comboPermOthers->setCurrentIndex( i );
            break;
        }
    }

    if( (perm & S_IXUSR) && 
        (perm & S_IXGRP) &&
        (perm & S_IXOTH) )
    {
        m_widget->checkFolder->setTristate( false );
        m_widget->checkFolder->setChecked( true );
    }
    else
    {
        if( (perm & S_IXUSR) || 
            (perm & S_IXGRP) ||
            (perm & S_IXOTH) )
            m_widget->checkFolder->setCheckState( Qt::PartiallyChecked );
    }

    m_curPermission = perm;
}
*/

#include "permissionsplugin.moc"
