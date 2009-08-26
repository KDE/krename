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

// OS includes
#ifndef _WIN32
#include <unistd.h>
#endif // _WIN32
#include <sys/types.h>

#include "../config-krename.h"

int main(int argc, char *argv[])
{
    KCmdLineOptions options;
    options.add( "+[file]", ki18n("file will be added to the list of files for renaming") );
    options.add( "r +[dir]", ki18n("add directory recursively") );
    options.add( "template +", ki18n("set a template") );
    options.add( "extension +", ki18n("set a template for the file extension") );
    options.add( "use-plugin +", ki18n("enable a plugin for use") );
    options.add( "copy +[dir]", ki18n("copy files to directory or url") );
    options.add( "move +[dir]", ki18n("move files to directory or url") );
    options.add( "link +[dir]", ki18n("link files to directory or url") );
    //options.add( "profile +[profile]", ki18n("load the profile named [profile] on startup") );
    options.add( "start", ki18n("start renaming immediately") );
    options.add( "test", ki18n("start KRename's selftest (developers only)") );
    // This option was never implemented in the KDE4 version:
    //options.add( "previewitems <num>",  ki18n("only show <num> preview items") );

    KAboutData aboutData( "krename", "krename", ki18n("KRename"), VERSION, ki18n(
                              "<qt><p>KRename is a batch file renamer which can rename a"
                              "list of files based on a set of expressions.</p>"
                              "<p>If you like KRename you may want to support it."
                              "Testing, bug fixes and feature request are as welcome."
                              "as financial support (everybody needs money ;) maSee help files for details.</p></qt>" ),
                          KAboutData::License_GPL, ki18n("KRename Build:" __DATE__ " " __TIME__ )
                          , ki18n("(c) 2001-2009, Dominik Seichter\n"),
                          "http://www.krename.net", "domseichter@web.de" );

    aboutData.addAuthor( ki18n("Dominik Seichter"), ki18n("developer and maintainer"), "domseichter@web.de",
                         "http://www.krename.net" );
    aboutData.addAuthor( ki18n("Stefan \"Stonki\" Onken"),
                         ki18n("Website, testing, very good ideas and keeping me coding!"),
                         "support@stonki.de", "http://www.stonki.de" );

    aboutData.addCredit( ki18n("Trevor Semeniuk"),  ki18n("Thanks to him for creating RedHat 7.x packages and some other help."),
                         "semeniuk@ee.ualberta.ca", "http://www.semeniuk.net" );
    aboutData.addCredit( ki18n("Groult Richard"),   ki18n("Fixed a bug with startIndex and added the BatchRenamer class\n"
                                                      "to his excellent image viewer showimg."),
                        "rgroult@jalix.org", "http://ric.jalix.org/" );
    aboutData.addCredit( ki18n("Michael Elvers"),  ki18n("Fixed a bug that caused krename not closing open files."),
                         "m_elvers@yahoo.com", "http://come.to/melvers" );
    aboutData.addCredit( ki18n("Andreas Pour"),  ki18n("Thanks for his great job at apps.kde.com and help with contributing krename to apps.kde.com."),
                         "pour@mieterra.com", "http://apps.kde.com" );
    aboutData.addCredit( ki18n("Charles Samuels"), ki18n("Thanks for noatun and the ID3/Ogg Tag code is based on his noatun modules."),
                         "charles@kde.org", "http://noatun.kde.org/" );
    aboutData.addCredit( ki18n("Franz Schmid"), ki18n("Gave me a good start into writing plugins with his application scribus."),
                         "Franz.Schmid@altmuehlnet.de", "http://web2.altmuehlnet.de/fschmid/index.html" );
    aboutData.addCredit( ki18n("Rolf Magnus"), ki18n("Parts of the PNG support are copied from his KFile plugin for png support."),
                         "ramagnus@kde.org" );
    aboutData.addCredit( ki18n("Michael v.Ostheim"), ki18n("Created the Gentoo Ebuild scripts for Krename."),
                         "MvOstheim@web.de", "http://www.vonostheim.de" );
    aboutData.addCredit( ki18n("Brandon Low"), ki18n("Some GCC 3.1 fixes for Gentoo."),
                         "lostlogic@gentoo.org", "http://www.gentoo.org" );
    aboutData.addCredit( ki18n("Per Oyvind Karlsen"), ki18n("Thanks for creating the Mandrake RPM"),
                         "peroyvind@delonic.no" );
    aboutData.addCredit( ki18n("Daniele Medri"), ki18n("Italian translation"), "madrid@linuxmeeting.net" );
    aboutData.addCredit( ki18n("Stephan Johach"), ki18n("Provided a gcc3.x namespace patch"), "lucardus@onlinehome.de" );
    aboutData.addCredit( ki18n("Michael Zugaro"), ki18n("Provided the new preview and move features") , "michael.zugaro@college-de-france.fr" );
    aboutData.addCredit( ki18n("Rene Gass"), ki18n("Fixed problems with the spec file and contributed rpms for every SuSE version you can imagine and is also the new Gentoo maintainer for KRename"), "kde-package@gmx.de" );
    aboutData.addCredit( ki18n("Mark Ziegler"), ki18n("Provided SuSE RPMs and very good suggestions"), "mark.ziegler@rakekniven.de" );
    aboutData.addCredit( ki18n("Jose Rodriguez"), ki18n("Contributed a Spanish translation"), "chmpmi@eresmas.net" );
    aboutData.addCredit( ki18n("Steven P. Ulrick"), ki18n("Provided a RedHat RPM and was big help in improving KRename"), "steve@afolkey2.net" );
    aboutData.addCredit( ki18n("UTUMI Hirosi"), ki18n("Translated KRename to Japanese"), "utuhiro@mx12.freecom.ne.jp" );
    aboutData.addCredit( ki18n("Nicolas Benoit"), ki18n("Translated KRename into French"), "nbenoit@tuxfamily.org" );
    aboutData.addCredit( ki18n("Krzysztof Pawlak"), ki18n("Translated KRename into Polish"), "jmnemonic@gazeta.pl" );
    aboutData.addCredit( ki18n("Ilya Ivkov"), ki18n("Translated KRename into Russian"), "ilya-ivkov@yandex.ru" );    
    aboutData.addCredit( ki18n("Asim Husanovic"), ki18n("Translated KRename into Bosnian"), "asim.h@megatel.ba" );    
    aboutData.addCredit( ki18n("Michal Smoczyk"), ki18n("Polish Translation"), "msmoczyk@wp.pl" );    
    aboutData.addCredit( ki18n("Pavel Fric"), ki18n("Czech Translation"), "pavelfric@seznam.cz" );    
    aboutData.setTranslator(ki18nc("_: NAME OF TRANSLATORS", "Your names"),
                            ki18nc("_: EMAIL OF TRANSLATORS", "Your emails"));
 

    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineArgs::addCmdLineOptions( options );

    KApplication a;
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );

    QWidget* krename = KRenameImpl::launch( QRect( 0, 0, 0, 0 ), KRenameFile::List() );

#ifndef _WIN32
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
#endif // _WIN32

/*
 * Activate this warning message for unstable development releases.
 */
/*    KMessageBox::sorry( krename, i18n(
    "<b>Warning !</b> This is a development release which may cause damage to your files!"
    "<br>Make backups before using KRename." ));
*/
    return a.exec();
}

