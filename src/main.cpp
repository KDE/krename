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

#include <kaboutdata.h>

#include <kconfig.h>
#include <kimageio.h>
#include <klocale.h>
#include <kmessagebox.h>



#include <klocalizedstring.h>

// Own includes
#include "krenameimpl.h"

// OS includes
#ifndef _WIN32
#include <unistd.h>
#endif // _WIN32
#include <sys/types.h>
#include <QApplication>
#include <KAboutData>
#include <KLocalizedString>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "../config-krename.h"

int main(int argc, char *argv[])
{
    KLocalizedString::setApplicationDomain("krename");

    QApplication app(argc, argv); // PORTING SCRIPT: move this to before the KAboutData initialization

    KAboutData aboutData("krename", i18n("KRename"), VERSION);
    aboutData.setShortDescription(i18n(
                              "KRename is a batch file renamer which can rename a "
                              "list of files based on a set of expressions."));
    aboutData.setOtherText(i18n(
                              "If you like KRename you may want to support it. "
                              "Testing, bug fixes and feature requests are as welcome "
                              "as financial support (everybody needs money ;). See help files for details." ));
    aboutData.setLicense(KAboutLicense::GPL_V3);
    aboutData.setCopyrightStatement(i18n("(c) 2001-2012, Dominik Seichter\n"));
    aboutData.setHomepage("http://www.krename.net");
    

    aboutData.addAuthor( i18n("Dominik Seichter"), i18n("developer and maintainer"), "domseichter@web.de",
                         "http://www.krename.net" );
    aboutData.addAuthor( i18n("Stefan \"Stonki\" Onken"),
                         i18n("Website, testing, very good ideas and keeping me coding!"),
                         "support@stonki.de", "http://www.stonki.de" );

    aboutData.addCredit( i18n("Arpad Biro"), i18n("Helped to fix style guide issues and made improvements to user messages."), "biro.arpad@gmail.com" );    
    aboutData.addCredit( i18n("Trevor Semeniuk"),  i18n("Thanks to him for creating RedHat 7.x packages and some other help."),
                         "semeniuk@ee.ualberta.ca", "http://www.semeniuk.net" );
    aboutData.addCredit( i18n("Groult Richard"),   i18n("Fixed a bug with startIndex and added the BatchRenamer class\n"
                                                      "to his excellent image viewer showimg."),
                        "rgroult@jalix.org", "http://ric.jalix.org/" );
    aboutData.addCredit( i18n("Michael Elvers"),  i18n("Fixed a bug that caused krename not closing open files."),
                         "m_elvers@yahoo.com", "http://come.to/melvers" );
    aboutData.addCredit( i18n("Andreas Pour"),  i18n("Thanks for his great job at apps.kde.com and help with contributing krename to apps.kde.com."),
                         "pour@mieterra.com", "http://apps.kde.com" );
    aboutData.addCredit( i18n("Charles Samuels"), i18n("Thanks for noatun and the ID3/Ogg Tag code is based on his noatun modules."),
                         "charles@kde.org", "http://noatun.kde.org/" );
    aboutData.addCredit( i18n("Franz Schmid"), i18n("Gave me a good start into writing plugins with his application scribus."),
                         "Franz.Schmid@altmuehlnet.de", "http://web2.altmuehlnet.de/fschmid/index.html" );
    aboutData.addCredit( i18n("Rolf Magnus"), i18n("Parts of the PNG support are copied from his KFile plugin for png support."),
                         "ramagnus@kde.org" );
    aboutData.addCredit( i18n("Michael v.Ostheim"), i18n("Created the Gentoo Ebuild scripts for Krename."),
                         "MvOstheim@web.de", "http://www.vonostheim.de" );
    aboutData.addCredit( i18n("Brandon Low"), i18n("Some GCC 3.1 fixes for Gentoo."),
                         "lostlogic@gentoo.org", "http://www.gentoo.org" );
    aboutData.addCredit( i18n("Per Oyvind Karlsen"), i18n("Thanks for creating the Mandrake RPM"),
                         "peroyvind@delonic.no" );
    aboutData.addCredit( i18n("Vincenzo Reale"), i18n("Italian translation"), "smart2128@baslug.org");
    aboutData.addCredit( i18n("Daniele Medri"), i18n("Italian translation work"), "madrid@linuxmeeting.net" );
    aboutData.addCredit( i18n("Stephan Johach"), i18n("Provided a gcc3.x namespace patch"), "lucardus@onlinehome.de" );
    aboutData.addCredit( i18n("Michael Zugaro"), i18n("Provided the new preview and move features") , "michael.zugaro@college-de-france.fr" );
    aboutData.addCredit( i18n("Rene Gass"), i18n("Fixed problems with the spec file and contributed rpms for every SuSE version you can imagine and is also the new Gentoo maintainer for KRename"), "kde-package@gmx.de" );
    aboutData.addCredit( i18n("Mark Ziegler"), i18n("Provided SuSE RPMs and very good suggestions"), "mark.ziegler@rakekniven.de" );
    aboutData.addCredit( i18n("Jose Rodriguez"), i18n("Contributed a Spanish translation"), "chmpmi@eresmas.net" );
    aboutData.addCredit( i18n("Steven P. Ulrick"), i18n("Provided a RedHat RPM and was big help in improving KRename"), "steve@afolkey2.net" );
    aboutData.addCredit( i18n("UTUMI Hirosi"), i18n("Translated KRename to Japanese"), "utuhiro@mx12.freecom.ne.jp" );
    aboutData.addCredit( i18n("Nicolas Benoit"), i18n("Translated KRename into French"), "nbenoit@tuxfamily.org" );
    aboutData.addCredit( i18n("Krzysztof Pawlak"), i18n("Translated KRename into Polish"), "jmnemonic@gazeta.pl" );
    aboutData.addCredit( i18n("Ilya Ivkov"), i18n("Translated KRename into Russian"), "ilya-ivkov@yandex.ru" );    
    aboutData.addCredit( i18n("Asim Husanovic"), i18n("Translated KRename into Bosnian"), "asim.h@megatel.ba" );    
    aboutData.addCredit( i18n("Michal Smoczyk"), i18n("Polish Translation"), "msmoczyk@wp.pl" );    
    aboutData.addCredit( i18n("Pavel Fric"), i18n("Czech Translation"), "pavelfric@seznam.cz" );    
    aboutData.setTranslator(i18nc("_: NAME OF TRANSLATORS", "Your names"),
                            i18nc("_: EMAIL OF TRANSLATORS", "Your emails"));
 

    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();

    parser.addPositionalArgument(QLatin1String("files"), i18n("Files to be added to the list to be renamed"), i18n("[files...]"));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("r"), i18n("add directory recursively"), i18n("directory")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("template"), i18n("set a template")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("extension"), i18n("set a template for the file extension")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("use-plugin"), i18n("enable a plugin for use")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("copy"), i18n("copy files to directory or url"), i18n("path or url")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("move"), i18n("move files to directory or url"), i18n("path or url")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("link"), i18n("link files to directory or url"), i18n("path or url")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("start"), i18n("start renaming immediately")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("test"), i18n("start KRename's selftest (developers only)")));
    // This option was never implemented in the KDE4 version:
    //parser.addOption(QCommandLineOption(QStringList() << QLatin1String("previewitems"), i18n("only show <num> preview items"), QLatin1String("num")));

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    app.setQuitOnLastWindowClosed(true);

    QWidget* krename = KRenameImpl::launch( QRect( 0, 0, 0, 0 ), KRenameFile::List(), &parser );

#ifndef Q_OS_WIN
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
    return app.exec();
}

