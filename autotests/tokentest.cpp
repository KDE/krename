// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#include "tokentest.h"

#include "batchrenamer.h"
#include "krenamefile.h"

#include <QtTest>

QTEST_MAIN(TokenTest)
Q_DECLARE_METATYPE(QLatin1String)

void TokenTest::initTestCase()
{
    qRegisterMetaType<QLatin1String>();
}

void TokenTest::testTokens_data()
{
    QTest::addColumn<QLatin1String>("token");
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QString>("expected");

    // Testing the basic KRename tokens
    QString filename(" Test File name ");
    QString directory1("krename");
    QString directory2("home");

    QTest::newRow("$ Test")
        << QLatin1String("$") << filename << filename;
    QTest::newRow("& Test")
        << QLatin1String("&") << filename << filename.toUpper();
    QTest::newRow("% Test")
        << QLatin1String("%") << filename << filename.toLower();
    QTest::newRow("* Test")
        << QLatin1String("*") << filename << " Test File Name ";
    QTest::newRow("# Test")
        << QLatin1String("#") << filename << QString::number(0);
    QTest::newRow("## Test")
        << QLatin1String("##") << filename << QString::number(0).asprintf("%02i", 0);
    QTest::newRow("### Test")
        << QLatin1String("###") << filename << QString().asprintf("%03i", 0);
    QTest::newRow("#### Test")
        << QLatin1String("####") << filename << QString().asprintf("%04i", 0);
    QTest::newRow("##### Test")
        << QLatin1String("#####") << filename << QString().asprintf("%05i", 0);
    QTest::newRow("#{100;2} Test")
        << QLatin1String("#{100;2}") << filename << QString::number(100);
    QTest::newRow("####{100;2} Test")
        << QLatin1String("####{100;2}") << filename
        << QString().asprintf("%04i", 100);
    QTest::newRow("####{2;2}## Test")
        << QLatin1String("####{2;2}##") << filename
        << QString().asprintf("%04i", 2) + QString().asprintf("%02i", 0);

    QTest::newRow("[1] Test")
        << QLatin1String("[1]") << filename << QString(filename[0]);
    QTest::newRow("[2] Test")
        << QLatin1String("[2]") << filename << QString(filename[1]);
    QTest::newRow("[3] Test")
        << QLatin1String("[3]") << filename << QString(filename[2]);
    QTest::newRow("[&4] Test")
        << QLatin1String("[&4]") << filename << QString(filename[3].toUpper());
    QTest::newRow("[$4] Test")
        << QLatin1String("[$4]") << filename << QString(filename[3]);
    QTest::newRow("[%4] Test")
        << QLatin1String("[%4]") << filename << QString(filename[3].toLower());
    QTest::newRow("[*4] Test")
        << QLatin1String("[*4]") << filename << QString(filename[3].toUpper());
    QTest::newRow("[4-] Test")
        << QLatin1String("[4-]") << filename
        << filename.right(filename.length() - 3);
    QTest::newRow("[&4-] Test")
        << QLatin1String("[&4-]") << filename
        << filename.right(filename.length() - 3).toUpper();
    QTest::newRow("[$4-] Test")
        << QLatin1String("[$4-]") << filename
        << filename.right(filename.length() - 3);
    QTest::newRow("[%4-] Test")
        << QLatin1String("[%4-]") << filename
        << filename.right(filename.length() - 3).toLower();
    QTest::newRow("[*4-] Test")
        << QLatin1String("[*4-]") << filename
        << "St File Name ";
    QTest::newRow("[4-] Test")
        << QLatin1String("[4-]") << filename
        << filename.right(filename.length() - 3);
    QTest::newRow("[&4-[length]] Test")
        << QLatin1String("[&4-[length]]") << filename
        << filename.right(filename.length() - 3).toUpper();
    QTest::newRow("[$4-[length]] Test")
        << QLatin1String("[$4-[length]]") << filename
        << filename.right(filename.length() - 3);
    QTest::newRow("[%4-[length]] Test")
        << QLatin1String("[%4-[length]]") << filename
        << filename.right(filename.length() - 3).toLower();
    QTest::newRow("[*4-[length]] Test")
        << QLatin1String("[*4-[length]]") << filename << "St File Name ";
    QTest::newRow("[trimmed;[4-]] Test")
        << QLatin1String("[trimmed;[4-]]") << filename
        << filename.right(filename.length() - 3).trimmed();
    QTest::newRow("[trimmed] Test")
        << QLatin1String("[trimmed]") << filename << filename.trimmed();
    QTest::newRow("[length] Test")
        << QLatin1String("[length]")
        << filename << QString::number(filename.length());
    QTest::newRow("[length-0] Test")
        << QLatin1String("[length-0]")
        << filename << QString::number(filename.length());
    QTest::newRow("[length-1] Test")
        << QLatin1String("[length-1]") << filename
        << QString::number(filename.length() - 1);
    QTest::newRow("[length-2] Test")
        << QLatin1String("[length-2]") << filename
        << QString::number(filename.length() - 2);
    QTest::newRow("[#length] Test")
        << QLatin1String("[#length]") << filename
        << QString::number(filename.length());
    QTest::newRow("[#length-0] Test")
        << QLatin1String("[#length-0]") << filename
        << QString::number(filename.length());
    QTest::newRow("[#length-1] Test")
        << QLatin1String("[#length-1]") << filename
        << QString::number(filename.length() - 1);
    QTest::newRow("[#length-2] Test")
        << QLatin1String("[#length-2]") << filename
        << QString::number(filename.length() - 2);
    QTest::newRow("[####length] Test")
        << QLatin1String("[####length]")
        << filename << QString().asprintf("%04i", filename.length());
    QTest::newRow("[####length-0] Test")
        << QLatin1String("[####length-0]") << filename
        << QString().asprintf("%04i", filename.length());
    QTest::newRow("[####length-1] Test")
        << QLatin1String("[####length-1]") << filename
        << QString().asprintf("%04i", filename.length() - 1);
    QTest::newRow("[####length-2] Test")
        << QLatin1String("[####length-2]") << filename
        << QString().asprintf("%04i", filename.length() - 2);
    QTest::newRow("[6-9] Test")
        << QLatin1String("[6-9]") << filename << filename.mid(5, 4);
    QTest::newRow("[&6-9] Test")
        << QLatin1String("[&6-9]") << filename << filename.mid(5, 4).toUpper();
    QTest::newRow("[$6-9] Test")
        << QLatin1String("[$6-9]") << filename << filename.mid(5, 4);
    QTest::newRow("[%6-9] Test")
        << QLatin1String("[%6-9]") << filename << filename.mid(5, 4).toLower();
    QTest::newRow("[*6-9] Test")
        << QLatin1String("[*6-9]") << filename << filename.mid(5, 4);
    QTest::newRow("[trimmed;[6-9]] Test")
        << QLatin1String("[trimmed;[6-9]]") << filename
        << filename.mid(5, 4).trimmed();
    QTest::newRow("[6;4] Test")
        << QLatin1String("[6;4]") << filename << filename.mid(5, 4);
    QTest::newRow("[&6;4] Test")
        << QLatin1String("[&6;4]") << filename << filename.mid(5, 4).toUpper();
    QTest::newRow("[$6;4] Test")
        << QLatin1String("[$6;4]") << filename << filename.mid(5, 4);
    QTest::newRow("[%6;4] Test")
        << QLatin1String("[%6;4]") << filename << filename.mid(5, 4).toLower();
    QTest::newRow("[*6;4] Test")
        << QLatin1String("[*6;4]") << filename << filename.mid(5, 4);
    QTest::newRow("[1;1{[length]}] Test")
        << QLatin1String("[1;1{[length]}]") << filename <<  "1";
    QTest::newRow("[trimmed;[6;4]] Test")
        << QLatin1String("[trimmed;[6;4]]") << filename
        << filename.mid(5, 4).trimmed();
    QTest::newRow("[trimmed; Hallo ] Test")
        << QLatin1String("[trimmed; Hallo ]") << filename << "Hallo";
    QTest::newRow("[dirname] Test")
        << QLatin1String("[dirname]") << filename << directory1;
    QTest::newRow("[&dirname] Test")
        << QLatin1String("[&dirname]") << filename << directory1.toUpper();
    QTest::newRow("[$dirname] Test")
        << QLatin1String("[$dirname]") << filename << directory1;
    QTest::newRow("[%dirname] Test")
        << QLatin1String("[%dirname]") << filename << directory1.toLower();
    // Don't change the capitalisation below just to match the app name style,
    // it'll break the test
    QTest::newRow("[*dirname] Test")
        << QLatin1String("[*dirname]") << filename << "Krename";
    QTest::newRow("[trimmed;[dirname]] Test")
        << QLatin1String("[trimmed;[dirname]]") << filename << directory1;
    QTest::newRow("[dirname.] Test")
        << QLatin1String("[dirname.]") << filename << directory2;
    QTest::newRow("[&dirname.] Test")
        << QLatin1String("[&dirname.]") << filename << directory2.toUpper();
    QTest::newRow("[$dirname.] Test")
        << QLatin1String("[$dirname.]") << filename << directory2;
    QTest::newRow("[%dirname.] Test")
        << QLatin1String("[%dirname.]") << filename << directory2.toLower();
    QTest::newRow("[*dirname.] Test")
        << QLatin1String("[*dirname.]") << filename << "Home";
    QTest::newRow("[trimmed;[dirname.]] Test")
        << QLatin1String("[trimmed;[dirname.]]") << filename  << directory2;
    QTest::newRow("[dirname..] Test")
        << QLatin1String("[dirname..]") << filename << "";
    QTest::newRow("[&dirname..] Test")
        << QLatin1String("[&dirname..]") << filename << "";
    QTest::newRow("[$dirname..] Test")
        << QLatin1String("[$dirname..]") << filename << "";
    QTest::newRow("[%dirname..] Test")
        << QLatin1String("[%dirname..]") << filename << "";
    QTest::newRow("[*dirname..] Test")
        << QLatin1String("[*dirname..]") << filename << "";
    // TODO: This test has strange effects that only occur if [dirname..] is an empty QString
    //QTest::newRow("[trimmed;[dirname..]] Test")
    //    << QLatin1String("[trimmed;[dirname..]]") << filename << filename.trimmed();
    QTest::newRow("Complex Test1")
        << QLatin1String("&[2-5]") << filename << filename.toUpper() + "Test";
    QTest::newRow("Complex Test2")
        << QLatin1String("%[2-5]") << filename << filename.toLower() + "Test";
    QTest::newRow("Complex Test3")
        << QLatin1String("$[2-5]") << filename << filename + "Test";
    QTest::newRow("Complex Test4")
        << QLatin1String("*[2-5]") << filename << " Test File Name Test";
    QTest::newRow("Complex Test5")
        << QLatin1String("[trimmed][2-5]") << filename
        << filename.trimmed() + "Test";
    QTest::newRow("Complex Test6")
        << QLatin1String("[&2-5]\\&[length-2]\\&[1;1{Hallo}]") << filename
        << "TEST&14&H";

    // Testing all special characters in KRename
    QTest::newRow("\\/ Test") << QLatin1String("\\/") << filename << "%2f";   // this is displayed as a slash,
    // unix filenames are not allowed
    // to contain a slash
    QTest::newRow("\\[ Test") << QLatin1String("\\[") << filename << "[";
    QTest::newRow("\\] Test") << QLatin1String("\\]") << filename << "]";
    QTest::newRow("\\$ Test") << QLatin1String("\\$") << filename << "$";
    QTest::newRow("\\* Test") << QLatin1String("\\*") << filename << "*";
    QTest::newRow("\\\\ Test") << QLatin1String("\\\\") << filename << "\\";
    QTest::newRow("\\& Test") << QLatin1String("\\&") << filename  << "&";
    QTest::newRow("\\% Test") << QLatin1String("\\%") << filename << "%";
    QTest::newRow("\\# Test") << QLatin1String("\\#") << filename << "#";

    // Testing filenames with special characters
    QString specialname("Test %1 File");
    QTest::newRow("File [ Test") << QLatin1String("$") << specialname.arg("[") << specialname.arg("[");
    QTest::newRow("File ] Test") << QLatin1String("$") << specialname.arg("]") << specialname.arg("]");
    QTest::newRow("File $ Test") << QLatin1String("$") << specialname.arg("$") << specialname.arg("$");
    QTest::newRow("File * Test") << QLatin1String("$") << specialname.arg("*") << specialname.arg("*");
    QTest::newRow("File \\ Test") << QLatin1String("$") << specialname.arg("\\") << specialname.arg("\\");
    QTest::newRow("File & Test") << QLatin1String("$") << specialname.arg("&") << specialname.arg("&");
    QTest::newRow("File % Test") << QLatin1String("$") << specialname.arg("%") << specialname.arg("%");
    // TODO: figure out why this fails, it works if I create the file manually and rename it via the GUI
    //QTest::newRow("File # Test") << QLatin1String("$") << specialname.arg("#") << specialname.arg("#");

    // Testing system functions
    QTest::newRow("Date Test")
        << QLatin1String("[date]") << filename
        << QDateTime::currentDateTime().toString("dd-MM-yyyy");
    QTest::newRow("dd-MM-yyyy Date Test")
        << QLatin1String("[date;dd-MM-yyyy]") << filename
        << QDateTime::currentDateTime().toString("dd-MM-yyyy");
    QTest::newRow("dd:MM:yyyy Date Test")
        << QLatin1String("[date;dd:MM:yyyy]") << filename
        << QDateTime::currentDateTime().toString("dd:MM:yyyy");
    QTest::newRow("yy.mm.dd Date Test")
        << QLatin1String("[date;yy.mm.dd]") << filename
        << QDateTime::currentDateTime().toString("yy.mm.dd");
    QTest::newRow("d Date Test")
        << QLatin1String("[date;d]") << filename
        << QDateTime::currentDateTime().toString("d");
    QTest::newRow("dd Date Test")
        << QLatin1String("[date;dd]") << filename
        << QDateTime::currentDateTime().toString("dd");
    QTest::newRow("ddd Date Test")
        << QLatin1String("[date;ddd]") << filename
        << QDateTime::currentDateTime().toString("ddd");
    QTest::newRow("dddd Date Test")
        << QLatin1String("[date;dddd]") << filename
        << QDateTime::currentDateTime().toString("dddd");
    QTest::newRow("M Date Test")
        << QLatin1String("[date;M]") << filename
        << QDateTime::currentDateTime().toString("M");
    QTest::newRow("MM Date Test")
        << QLatin1String("[date;MM]") << filename
        << QDateTime::currentDateTime().toString("MM");
    QTest::newRow("MMM Date Test")
        << QLatin1String("[date;MMM]") << filename
        << QDateTime::currentDateTime().toString("MMM");
    QTest::newRow("MMMM Date Test")
        << QLatin1String("[date;MMMM]") << filename
        << QDateTime::currentDateTime().toString("MMMM");
    QTest::newRow("yy Date Test")
        << QLatin1String("[date;yy]") << filename
        << QDateTime::currentDateTime().toString("yy");
    QTest::newRow("yyyy Date Test")
        << QLatin1String("[date;yyyy]") << filename
        << QDateTime::currentDateTime().toString("yyyy");

    QTest::newRow("h Date Test")
        << QLatin1String("[date;h]") << filename
        << QDateTime::currentDateTime().toString("h");
    QTest::newRow("hh Date Test")
        << QLatin1String("[date;hh]") << filename
        << QDateTime::currentDateTime().toString("hh");
    QTest::newRow("m Date Test")
        << QLatin1String("[date;m]") << filename
        << QDateTime::currentDateTime().toString("m");
    QTest::newRow("mm Date Test")
        << QLatin1String("[date;mm]") << filename
        << QDateTime::currentDateTime().toString("mm");
    QTest::newRow("s Date Test")
        << QLatin1String("[date;s]") << filename
        << QDateTime::currentDateTime().toString("s");
    QTest::newRow("ss Date Test")
        << QLatin1String("[date;ss]") << filename
        << QDateTime::currentDateTime().toString("ss");
    // Current computers are to slow to compare two milliseconds as the instruction is longer than
    // a millisecond.
    //QTest::newRow("z Date Test")
    //    << QLatin1String("[date;z]") << filename
    //    << QDateTime::currentDateTime().toString( "z");
    //QTest::newRow("zzz Date Test")
    //    << QLatin1String("[date;zzz]") << filename
    //    << QDateTime::currentDateTime().toString( "zzz");
    QTest::newRow("ap Date Test")
        << QLatin1String("[date;ap]") << filename
        << QDateTime::currentDateTime().toString("ap");
    QTest::newRow("AP Date Test")
        << QLatin1String("[date;AP]") << filename
        << QDateTime::currentDateTime().toString("AP");
    QTest::newRow("Day Test")
        << QLatin1String("[day]") << filename
        << QDateTime::currentDateTime().toString("dd");
    QTest::newRow("Month Test")
        << QLatin1String("[month]") << filename
        << QDateTime::currentDateTime().toString("MM");
    QTest::newRow("Year Test")
        << QLatin1String("[year]") << filename
        << QDateTime::currentDateTime().toString("yyyy");
    QTest::newRow("Hour Test")
        << QLatin1String("[hour]") << filename
        << QDateTime::currentDateTime().toString("hh");
    QTest::newRow("Minute Test")
        << QLatin1String("[minute]") << filename
        << QDateTime::currentDateTime().toString("mm");
    QTest::newRow("Second Test")
        << QLatin1String("[second]") << filename
        << QDateTime::currentDateTime().toString("ss");
    QTest::newRow("Time Test")
        << QLatin1String("[time]") << filename
        << QDateTime::currentDateTime().toString("hh-mm-ss");

    // Regression tests
    // Caused an infinite loop
    QTest::newRow("[1-2 [4-] Test")
        << QLatin1String("[1-2 [4-]") << filename
        << ' ' + filename.right(filename.length() - 3);
    QTest::newRow("###{329;13")
        << QLatin1String("###{329;13") << filename << "329";
    QTest::newRow("ABC {1")
        << QLatin1String("ABC {1") << filename << "ABC {1";

    // Brackets appeared as \[ in the final result
    QTest::newRow("1##-[$6-] Test")
        << QLatin1String("1##-[$6-]") << "1-07 Take Flight (Wings) [Pocketman]"
        << "100-Take Flight (Wings) [Pocketman]";
}

void TokenTest::testTokens()
{
    QFETCH(QLatin1String, token);
    QFETCH(QString, filename);
    QFETCH(QString, expected);

    QString directory("/home/krename/");
    KRenameFile::List list;
    KRenameFile file(QUrl(directory + filename), filename.isEmpty(), eSplitMode_FirstDot, 1);

    list.push_back(file);

    BatchRenamer b;
    b.setFilenameTemplate(token);
    b.setFiles(&list);
    b.processFilenames();

    QCOMPARE(list[0].dstFilename(), expected);
}

void TokenTest::testReplacing_data()
{
    QTest::addColumn<QLatin1String>("token");
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QString>("expected");
    QTest::addColumn<QString>("replace");
    QTest::addColumn<QString>("with");
    QTest::addColumn<bool>("regularExpression");

    QTest::newRow("Replace: Spaces")
        << QLatin1String("$") << "Filename with spaces"
        << "Filename_with_spaces" << " " << "_" << false;
    QTest::newRow("Replace: Nothing")
        << QLatin1String("$") << "Filename" << "Filename" << " " << "_"
        << false;
    QTest::newRow("Replace: Word")
        << QLatin1String("$") << "Filename with spaces"
        << "Filename HAS spaces" << "with" << "HAS" << false;
    QTest::newRow("Replace: $")
        << QLatin1String("$") << "Filename with $ and spaces"
        << "Filename with ! and spaces" << "$" << "!" << false;
    QTest::newRow("Replace: &")
        << QLatin1String("$") << "Filename with & and spaces"
        << "Filename with ! and spaces" << "&" << "!" << false;
    QTest::newRow("Replace: %")
        << QLatin1String("$") << "Filename with % and spaces"
        << "Filename with ! and spaces" << "%" << "!" << false;
    QTest::newRow("Replace: *")
        << QLatin1String("$") << "Filename with * and spaces"
        << "Filename with ! and spaces" << "*" << "!" << false;
    QTest::newRow("Replace: [")
        << QLatin1String("$") << "Filename with [ and spaces"
        << "Filename with ! and spaces" << "[" << "!" << false;
    QTest::newRow("Replace: ]")
        << QLatin1String("$") << "Filename with ] and spaces"
        << "Filename with ! and spaces" << "]" << "!" << false;
    // TODO: figure out why this fails, it works if I create the file manually and rename it via the GUI
    //QTest::newRow("File # Test") << QLatin1String("$") << specialname.arg("#") << specialname.arg("#");
    /*QTest::newRow("Replace: #")
        << QLatin1String("$") << QStringLiteral("Filename with # and spaces")
        << QStringLiteral("Filename with ! and spaces") << "#" << "!" << false;*/
    QTest::newRow("Replace: to $")
        << QLatin1String("$") << "Filename with spaces"
        << "Filename $ spaces" << "with" << "$" << false;
    QTest::newRow("Replace: to &")
        << QLatin1String("$") << "Filename with spaces"
        << "Filename & spaces" << "with" << "&" << false;
    QTest::newRow("Replace: to %")
        << QLatin1String("$") << "Filename with spaces"
        << "Filename % spaces" << "with" << "%" << false;
    QTest::newRow("Replace: to *")
        << QLatin1String("$") << "Filename with spaces"
        << "Filename * spaces" << "with" << "*" << false;
    QTest::newRow("Replace: to [")
        << QLatin1String("$") << "Filename with spaces"
        << "Filename [ spaces" << "with" << "[" << false;
    QTest::newRow("Replace: to ]")
        << QLatin1String("$") << "Filename with spaces"
        << "Filename ] spaces" << "with" << "]" << false;
    QTest::newRow("Replace: to #")
        << QLatin1String("$") << "Filename with spaces"
        << "Filename # spaces" << "with" << "#" << false;

    QTest::newRow("RegExp: ?")
        << QLatin1String("$") << "Filename"
        << "AAAAAAAAA" << "[a-zA-z]?" << "A" << true;
    QTest::newRow("RegExp: {1}")
        << QLatin1String("$") << "Filename"
        << "AAAAAAAA" << "[a-zA-z]{1}"  << "A" << true;
    QTest::newRow("RegExp: +")
        << QLatin1String("$") << "Filename"
        << "A" << "[a-zA-z]+" << "A" << true;
    QTest::newRow("RegExp: \\d")
        << QLatin1String("$") << "Filename 123"
        << "Filename NumberNumberNumber" << "\\d"  << "Number" << true;
    QTest::newRow("RegExp: \\d+")
        << QLatin1String("$") << "Filename 123"
        << "Filename Number" << "\\d+" << "Number" << true;
    QTest::newRow("RegExp: Match")
        << QLatin1String("$") << "Filename 123"
        << "MATCHING" << "[a-zA-z]* \\d{3}" << "MATCHING" << true;
}

void TokenTest::testReplacing()
{
    QFETCH(QLatin1String, token);
    QFETCH(QString, filename);
    QFETCH(QString, expected);
    QFETCH(QString, replace);
    QFETCH(QString, with);
    QFETCH(bool, regularExpression);

    QString directory("/home/krename/");
    KRenameFile::List list;
    KRenameFile file(QUrl(directory + filename), filename.isEmpty(), eSplitMode_FirstDot, 1);
    list.push_back(file);

    QList<TReplaceItem> replaceList;

    TReplaceItem strings;
    strings.find = replace;
    strings.replace = with;
    strings.reg = regularExpression;
    strings.doProcessTokens = false;

    replaceList.append(strings);

    BatchRenamer b;
    b.setFilenameTemplate(token);
    b.setFiles(&list);
    b.setReplaceList(replaceList);
    b.processFilenames();

    QCOMPARE(list[0].dstFilename(), expected);
}

#include "moc_tokentest.cpp"
