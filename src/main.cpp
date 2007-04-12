/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Die Mai 15 15:34:19 CEST 2001
    copyright            : (C) 2001 by Dominik Seichter
    email                : domseichter@web.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your2192 option) any later version.                               *
 *                                                                         *
 ***************************************************************************/

// Qt includes
#include <qrect.h>
#include <qwidget.h>

// KDE includes
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kconfig.h>
#include <kimageio.h>
#include <klocale.h>
#include <kmessagebox.h>

// Own includes
#include "krenameimpl.h"
/*
#include "wizard.h"
#include "tabs.h"
#include "firststartdlg.h"
*/

// OS includes
#include <unistd.h>
#include <sys/types.h>

#ifndef VERSION
    #define VERSION "3.9.0"
#endif

static KCmdLineOptions options[] =
{
    { "+[file]", I18N_NOOP("file will be added to the list of files for renaming"), 0},
    { "r +[dir]", I18N_NOOP("add directory recursively"), 0 },
    { "template +", I18N_NOOP("set a template"), 0 },
    { "extension +", I18N_NOOP("set a template for the file extension"), 0 },
    { "use-plugin +", I18N_NOOP("enable a plugin for use"), 0 },
    { "copy +[dir]", I18N_NOOP("copy files to directory"), 0 },
    { "move +[dir]", I18N_NOOP("move files to directory"), 0 },
    { "profile +[profile]", I18N_NOOP("load the profile named [profile] on startup"), 0 },
    { "start", I18N_NOOP("start renaming immediately"), 0 },
    { "previewitems <num>",  I18N_NOOP("only show <num> preview items"), 0 },
    KCmdLineLastOption
};

int main(int argc, char *argv[])
{
    KAboutData aboutData( "krename", I18N_NOOP("KRename"), VERSION, I18N_NOOP(
                              "KRename is a batch file renamer which can rename a\n"
                              "list of files based on a set of expressions.\n\n"
                              "If you like KRename you may want to support it.\n"
                              "Testing, bug fixes and feature request are as welcome\n"
                              "as financial support (everybody needs money ;)\nSee help files for details.\n" ),
                          KAboutData::License_GPL, "KRename Build:" __DATE__ " " __TIME__
                          , "(c) 2001-2007, Dominik Seichter\n",
                          "http://www.krename.net", "domseichter@web.de" );

    aboutData.addAuthor("Dominik Seichter", 0, "domseichter@web.de",
                        "http://www.krename.net" );
    aboutData.addAuthor("Stefan \"Stonki\" Onken",
                        I18N_NOOP("Website, testing, very good ideas and keeping me coding!"),
                        "support@stonki.de", "http://www.stonki.de" );

    aboutData.addCredit("Trevor Semeniuk",  I18N_NOOP("Thanks to him for creating RedHat 7.x packages and some other help."),
                        "semeniuk@ee.ualberta.ca", "http://www.semeniuk.net" );
    aboutData.addCredit("Groult Richard",   I18N_NOOP("Fixed a bug with startIndex and added the BatchRenamer class\n"
                                                      "to his excellent image viewer showimg."),
                        "rgroult@jalix.org", "http://ric.jalix.org/" );
    aboutData.addCredit("Michael Elvers",  I18N_NOOP("Fixed a bug that caused krename not closing open files."),
                        "m_elvers@yahoo.com", "http://come.to/melvers" );
    aboutData.addCredit("Andreas Pour",  I18N_NOOP("Thanks for his great job at apps.kde.com and help with contributing krename to apps.kde.com."),
                        "pour@mieterra.com", "http://apps.kde.com" );
    aboutData.addCredit("Charles Samuels", I18N_NOOP("Thanks for noatun and the ID3/Ogg Tag code is based on his noatun modules."),
                        "charles@kde.org", "http://noatun.kde.org/" );
    aboutData.addCredit("Franz Schmid", I18N_NOOP("Gave me a good start into writing plugins with his application scribus."),
                        "Franz.Schmid@altmuehlnet.de", "http://web2.altmuehlnet.de/fschmid/index.html" );
    aboutData.addCredit("Rolf Magnus", I18N_NOOP("Parts of the PNG support are copied from his KFile plugin for png support."),
                        "ramagnus@kde.org" );
    aboutData.addCredit("Michael v.Ostheim", I18N_NOOP("Created the Gentoo Ebuild scripts for Krename."),
                        "MvOstheim@web.de", "http://www.vonostheim.de" );
    aboutData.addCredit("Brandon Low", I18N_NOOP("Some GCC 3.1 fixes for Gentoo."),
                        "lostlogic@gentoo.org", "http://www.gentoo.org" );
    aboutData.addCredit("Per �vind Karlsen", I18N_NOOP("Thanks for creating the Mandrake RPM"),
                        "peroyvind@delonic.no" );
    aboutData.addCredit("Daniele Medri", I18N_NOOP("Italian translation"), "madrid@linuxmeeting.net" );
    aboutData.addCredit("Stephan Johach", I18N_NOOP("Provided a gcc3.x namespace patch"), "lucardus@onlinehome.de" );
    aboutData.addCredit("Micha� Zugaro", I18N_NOOP("Provided the new preview and move features") , "michael.zugaro@college-de-france.fr" );
    aboutData.addCredit("Rene Gass", I18N_NOOP("Fixed problems with the spec file and contributed rpms for every SuSE version you can imagine and is also the new Gentoo maintainer for KRename"), "kde-package@gmx.de" );
    aboutData.addCredit("Mark Volkert", I18N_NOOP("Provided SuSE RPMs and very good suggestions"), "mark.volkert@rakekniven.de" );
    aboutData.addCredit("Jose Rodriguez", I18N_NOOP("Contributed a Spanish translation"), "chmpmi@eresmas.net" );
    aboutData.addCredit("Steven P. Ulrick", I18N_NOOP("Provided a RedHat RPM and was big help in improving KRename"), "steve@afolkey2.net" );
    aboutData.addCredit("UTUMI Hirosi", I18N_NOOP("Translated KRename to Japanese"), "utuhiro@mx12.freecom.ne.jp" );
    aboutData.addCredit("Nicolas Benoit", I18N_NOOP("Translated KRename into French"), "nbenoit@tuxfamily.org" );
    aboutData.addCredit("Krzysztof Pawlak", I18N_NOOP("Translated KRename into Polish"), "jmnemonic@gazeta.pl" );
    aboutData.addCredit("Ilya Ivkov", I18N_NOOP("Translated KRename into Russian"), "ilya-ivkov@yandex.ru" );    
    aboutData.addCredit("Asim Husanovic", I18N_NOOP("Translated KRename into Bosnian"), "asim.h@megatel.ba" );    

    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineArgs::addCmdLineOptions( options );

    KApplication a;
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );

    QWidget* krename = KRenameImpl::launch( QRect( 0, 0, 0, 0 ), QStringList() );

    /* Check if Krename
     * was started from root!
     */
    unsigned int uid = geteuid();
    if( uid == 0 )
        KMessageBox::information( krename, i18n(
            "<b>Krename was started from root!</b><br>"
            "When started from root, Krename may damage your "
            "system if you do not know exactly what you are "
            "doing!"
        ), i18n("Error"), "KrenameRootWarning" );

/*
 * Activate this warning message for unstable development releases.
 */
/*    KMessageBox::sorry( krename, i18n(
    "<b>Warning !</b> This is a development release which may cause damage to your files!"
    "<br>Make backups before using KRename." ));
*/
    return a.exec();
}

