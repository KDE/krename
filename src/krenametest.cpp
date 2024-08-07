// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#include "krenametest.h"

#include "krenamefile.h"
#include "batchrenamer.h"

#include <cstdio>

#include <QTextEdit>
#include <QVBoxLayout>

#include <QUrl>

// Macros to simplify the test code

#define PRINT_RESULT( name ) writeTestMessage(" %04i Running test: %20s = %s at line %i", \
        ++m_counter, name, m_result ? "[OK]" : "[FAILED]", __LINE__ ); \
if( !m_result ) { \
    ++m_failed; \
    writeTestMessage( "<b>ERROR: %04i Test %s failed!</b>\n", m_counter, name ); \
} else \
    ++m_success;

#define RUN_KRENAMEFILE_TEST( name, url, dir, file, ext, mode, dot, isDir )   \
    m_result = testKRenameFileInternal( url, dir, file, ext, mode, dot, isDir ); \
    PRINT_RESULT( name );

#define RUN_TOKEN_TEST( name, templ, filename, expect ) m_result = tokenTest( templ, filename, expect ); \
    PRINT_RESULT( name );

#define RUN_NUMBER_TEST( name, length, start, step, skip ) m_result = numberingTest( length, start, step, skip, 100 ); \
    PRINT_RESULT( name );

#define RUN_NUMBER_TESTS( name, start, step, skip ) m_result = numberingTest( 1, start, step, skip, 100 ); \
    PRINT_RESULT( name ); \
    m_result = numberingTest( 2, start, step, skip, 100 ); \
    PRINT_RESULT( name ); \
    m_result = numberingTest( 3, start, step, skip, 100 ); \
    PRINT_RESULT( name ); \
    m_result = numberingTest( 4, start, step, skip, 100 ); \
    PRINT_RESULT( name ); \
    m_result = numberingTest( 10, start, step, skip, 100 ); \
    PRINT_RESULT( name ); \

#define RUN_REPLACE_TEST( name, token, file, expect, replace, with, regExp ) \
    m_result = replaceTest( token, file, expect, replace, with, regExp); \
    PRINT_RESULT( name );

#define RUN_KRENAME_MULTI_FILE_TEST( files, expected, expectedPaths, token, name ) \
    m_result = testMultipleFiles( files, expected, expectedPaths, token ); \
    PRINT_RESULT( name );

    KRenameTest::KRenameTest()
        : QWidget(),
          m_counter(0), m_verbose(false), m_success(0), m_failed(0)
    {
        QVBoxLayout *layout = new QVBoxLayout(this);

        m_text = new QTextEdit(this);
        m_text->setReadOnly(true);
        m_text->setFont(QFont("Courier"));

        layout->addWidget(m_text);

        this->setWindowTitle("KRename Test");
        this->resize(640, 480);
        this->show();
    }

    KRenameTest::~KRenameTest()
    {

    }

    void KRenameTest::startTest()
    {
        testKRenameFile();
        testBatchRenamer();

        this->writeTestMessage("<b>Results:</b>\n\nSuccessful: %i Failed %i", m_success, m_failed);
    }

    void KRenameTest::writeTestHeader(const QString &text)
    {
        QString str("<b>Testing class: <i>%1</i></b>");
        m_text->append(str.arg(text));
    }

    void KRenameTest::writeTestMessage(const char *format, ...)
    {
        va_list  args;

        va_start(args, format);
        vsnprintf(m_buffer, KRenameTest::BUFFER_LENGTH, format, args);
        va_end(args);

        m_text->append(m_buffer);
    }

    void KRenameTest::testKRenameFile()
    {
        writeTestHeader("KRenameFile");

        // Test a simple filename
        RUN_KRENAMEFILE_TEST("file:///home/test/simple.txt (first dot)", QUrl("file:///home/test/simple.txt"),
                             "/home/test", "simple", "txt", eSplitMode_FirstDot, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/simple.txt (last dot)", QUrl("file:///home/test/simple.txt"),
                             "/home/test", "simple", "txt", eSplitMode_LastDot, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/simple.txt (no extension)", QUrl("file:///home/test/simple.txt"),
                             "/home/test", "simple.txt", "", eSplitMode_NoExtension, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/simple.txt (no extension)", QUrl("file:///home/test/simple.txt"),
                             "/home/test", "simple.txt", "", eSplitMode_NoExtension, 1, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/simple.txt (custom dot 0)", QUrl("file:///home/test/simple.txt"),
                             "/home/test", "simple.txt", "", eSplitMode_CustomDot, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/simple.txt (custom dot 1)", QUrl("file:///home/test/simple.txt"),
                             "/home/test", "simple", "txt", eSplitMode_CustomDot, 1, false);

        // Test a simple filename and a directory with spaces
        RUN_KRENAMEFILE_TEST("file:///home/dir with space/simple.txt (first dot)", QUrl("file:///home/dir with space/simple.txt"),
                             "/home/dir with space", "simple", "txt", eSplitMode_FirstDot, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/dir with space/simple.txt (last dot)", QUrl("file:///home/dir with space/simple.txt"),
                             "/home/dir with space", "simple", "txt", eSplitMode_LastDot, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/dir with space/simple.txt (custom dot 0)", QUrl("file:///home/dir with space/simple.txt"),
                             "/home/dir with space", "simple.txt", "", eSplitMode_CustomDot, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/dir with space/simple.txt (custom dot 1)", QUrl("file:///home/dir with space/simple.txt"),
                             "/home/dir with space", "simple", "txt", eSplitMode_CustomDot, 1, false);

        // test a more complicated file extension
        RUN_KRENAMEFILE_TEST("file:///home/test/complicated.tar.gz (first dot)", QUrl("file:///home/test/complicated.tar.gz"),
                             "/home/test", "complicated", "tar.gz", eSplitMode_FirstDot, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/complicated.tar.gz (last dot)", QUrl("file:///home/test/complicated.tar.gz"),
                             "/home/test", "complicated.tar", "gz", eSplitMode_LastDot, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/complicated.tar.gz (custom dot 0)", QUrl("file:///home/test/complicated.tar.gz"),
                             "/home/test", "complicated.tar.gz", "", eSplitMode_CustomDot, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/complicated.tar.gz (custom dot 1)", QUrl("file:///home/test/complicated.tar.gz"),
                             "/home/test", "complicated", "tar.gz", eSplitMode_CustomDot, 1, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/complicated.tar.gz (custom dot 2)", QUrl("file:///home/test/complicated.tar.gz"),
                             "/home/test", "complicated.tar", "gz", eSplitMode_CustomDot, 2, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/complicated.tar.gz (custom dot 3)", QUrl("file:///home/test/complicated.tar.gz"),
                             "/home/test", "complicated.tar.gz", "", eSplitMode_CustomDot, 3, false);

        // test a directory with dot
        RUN_KRENAMEFILE_TEST("file:///home/dir.with.dot/simple.txt (first dot)", QUrl("file:///home/dir.with.dot/simple.txt"),
                             "/home/dir.with.dot", "simple", "txt", eSplitMode_FirstDot, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/dir.with.dot/simple.txt (last dot)", QUrl("file:///home/dir.with.dot/simple.txt"),
                             "/home/dir.with.dot", "simple", "txt", eSplitMode_LastDot, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/dir.with.dot/simple.txt (custom dot 0)", QUrl("file:///home/dir.with.dot/simple.txt"),
                             "/home/dir.with.dot", "simple.txt", "", eSplitMode_CustomDot, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/dir.with.dot/simple.txt (custom dot 1)", QUrl("file:///home/dir.with.dot/simple.txt"),
                             "/home/dir.with.dot", "simple", "txt", eSplitMode_CustomDot, 1, false);

        // test a directory which ends with a slash
        RUN_KRENAMEFILE_TEST("file:///home/test/ (first dot)", QUrl("file:///home/test/"),
                             "/home", "test", "", eSplitMode_FirstDot, 0, true);
        RUN_KRENAMEFILE_TEST("file:///home/test/ (last dot)", QUrl("file:///home/test/"),
                             "/home", "test", "", eSplitMode_LastDot, 0, true);
        RUN_KRENAMEFILE_TEST("file:///home/test/ (custom dot 0)", QUrl("file:///home/test/"),
                             "/home", "test", "", eSplitMode_CustomDot, 0, true);
        RUN_KRENAMEFILE_TEST("file:///home/test/ (custom dot 1)", QUrl("file:///home/test/"),
                             "/home", "test", "", eSplitMode_CustomDot, 1, true);

        // test a directory which doesn't end with a slash
        RUN_KRENAMEFILE_TEST("file:///home/test (first dot)", QUrl("file:///home/test"),
                             "/home", "test", "", eSplitMode_FirstDot, 0, true);
        RUN_KRENAMEFILE_TEST("file:///home/test (last dot)", QUrl("file:///home/test"),
                             "/home", "test", "", eSplitMode_LastDot, 0, true);
        RUN_KRENAMEFILE_TEST("file:///home/test (custom dot 0)", QUrl("file:///home/test"),
                             "/home", "test", "", eSplitMode_CustomDot, 0, true);
        RUN_KRENAMEFILE_TEST("file:///home/test (custom dot 1)", QUrl("file:///home/test"),
                             "/home", "test", "", eSplitMode_CustomDot, 1, true);

        // test no file extension
        RUN_KRENAMEFILE_TEST("file:///home/test/simple (first dot)", QUrl("file:///home/test/simple"),
                             "/home/test", "simple", "", eSplitMode_FirstDot, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/simple (last dot)", QUrl("file:///home/test/simple"),
                             "/home/test", "simple", "", eSplitMode_LastDot, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/simple (custom dot 0)", QUrl("file:///home/test/simple"),
                             "/home/test", "simple", "", eSplitMode_CustomDot, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/simple (custom dot 1)", QUrl("file:///home/test/simple"),
                             "/home/test", "simple", "", eSplitMode_CustomDot, 1, false);

        // test very long complex file extension
        RUN_KRENAMEFILE_TEST("file:///home/test/simple.txt.long.ext.zip (first dot)", QUrl("file:///home/test/simple.txt.long.ext.zip"),
                             "/home/test", "simple", "txt.long.ext.zip", eSplitMode_FirstDot, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/simple.txt.long.ext.zip (last dot)", QUrl("file:///home/test/simple.txt.long.ext.zip"),
                             "/home/test", "simple.txt.long.ext", "zip", eSplitMode_LastDot, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/simple.txt.long.ext.zip (custom dot 0)", QUrl("file:///home/test/simple.txt.long.ext.zip"),
                             "/home/test", "simple.txt.long.ext.zip", "", eSplitMode_CustomDot, 0, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/simple.txt.long.ext.zip (custom dot 1)", QUrl("file:///home/test/simple.txt.long.ext.zip"),
                             "/home/test", "simple", "txt.long.ext.zip", eSplitMode_CustomDot, 1, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/simple.txt.long.ext.zip (custom dot 2)", QUrl("file:///home/test/simple.txt.long.ext.zip"),
                             "/home/test", "simple.txt", "long.ext.zip", eSplitMode_CustomDot, 2, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/simple.txt.long.ext.zip (custom dot 3)", QUrl("file:///home/test/simple.txt.long.ext.zip"),
                             "/home/test", "simple.txt.long", "ext.zip", eSplitMode_CustomDot, 3, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/simple.txt.long.ext.zip (custom dot 4)", QUrl("file:///home/test/simple.txt.long.ext.zip"),
                             "/home/test", "simple.txt.long.ext", "zip", eSplitMode_CustomDot, 4, false);
        RUN_KRENAMEFILE_TEST("file:///home/test/simple.txt.long.ext.zip (custom dot 5)", QUrl("file:///home/test/simple.txt.long.ext.zip"),
                             "/home/test", "simple.txt.long.ext.zip", "", eSplitMode_CustomDot, 5, false);
    }

    bool KRenameTest::testKRenameFileInternal(const QUrl &url, const QString &directory,
            const QString &filename, const QString &extension,
            ESplitMode eSplitMode, int dot, bool isDir)
    {
        // if the expected filename and extension is empty expect a directory
        //bool        isDirectory = filename.isEmpty() && extension.isEmpty();
        KRenameFile file(url, isDir, eSplitMode, dot);

        if (file.srcDirectory() != directory) {
            writeTestMessage("Directory=(%s) expected (%s).\n",
                             file.srcDirectory().toLatin1().data(),
                             directory.toLatin1().data());
            return false;
        }

        if (file.srcFilename() != filename) {
            writeTestMessage("Filename=(%s) expected (%s).\n",
                             file.srcFilename().toLatin1().data(),
                             filename.toLatin1().data());
            return false;
        }

        if (file.srcExtension() != extension) {
            writeTestMessage("Extension=(%s) expected (%s).\n",
                             file.srcExtension().toLatin1().data(),
                             extension.toLatin1().data());
            return false;
        }

        return true;
    }

    void KRenameTest::testBatchRenamer()
    {
        writeTestHeader("BatchRenamer");

        // Testing the basic KRename tokens
        QString filename(" Test File name ");
        QString directory1("krename");
        QString directory2("home");

        RUN_TOKEN_TEST("$ Test", "$", filename, filename);
        RUN_TOKEN_TEST("& Test", "&", filename, filename.toUpper());
        RUN_TOKEN_TEST("% Test", "%", filename, filename.toLower());
        RUN_TOKEN_TEST("* Test", "*", filename, " Test File Name ");
        RUN_TOKEN_TEST("# Test", "#", filename, QString::number(0));
        RUN_TOKEN_TEST("## Test", "##", filename, QString::asprintf("%02i", 0));
        RUN_TOKEN_TEST("### Test", "###", filename, QString::asprintf("%03i", 0));
        RUN_TOKEN_TEST("#### Test", "####", filename, QString::asprintf("%04i", 0));
        RUN_TOKEN_TEST("##### Test", "#####", filename, QString::asprintf("%05i", 0));
        RUN_TOKEN_TEST("#{100;2} Test", "#{100;2}", filename, QString::number(100));
        RUN_TOKEN_TEST("####{100;2} Test", "####{100;2}", filename, QString::asprintf("%04i", 100));
        RUN_TOKEN_TEST("####{2;2}## Test", "####{2;2}##", filename,
                       QString::asprintf("%04i", 2) + QString::asprintf("%02i", 0));
        RUN_TOKEN_TEST("[1] Test", "[1]", filename, QString(filename[0]));
        RUN_TOKEN_TEST("[2] Test", "[2]", filename, QString(filename[1]));
        RUN_TOKEN_TEST("[3] Test", "[3]", filename, QString(filename[2]));
        RUN_TOKEN_TEST("[&4] Test", "[&4]", filename, QString(filename[3].toUpper()));
        RUN_TOKEN_TEST("[$4] Test", "[$4]", filename, QString(filename[3]));
        RUN_TOKEN_TEST("[%4] Test", "[%4]", filename, QString(filename[3].toLower()));
        RUN_TOKEN_TEST("[*4] Test", "[*4]", filename, QString(filename[3].toUpper()));
        RUN_TOKEN_TEST("[4-] Test", "[4-]", filename, filename.right(filename.length() - 3));
        RUN_TOKEN_TEST("[&4-] Test", "[&4-]", filename, filename.right(filename.length() - 3).toUpper());
        RUN_TOKEN_TEST("[$4-] Test", "[$4-]", filename, filename.right(filename.length() - 3));
        RUN_TOKEN_TEST("[%4-] Test", "[%4-]", filename, filename.right(filename.length() - 3).toLower());
        RUN_TOKEN_TEST("[*4-] Test", "[*4-]", filename, "St File Name ");
        RUN_TOKEN_TEST("[4-] Test", "[4-]", filename, filename.right(filename.length() - 3));
        RUN_TOKEN_TEST("[&4-[length]] Test", "[&4-[length]]", filename, filename.right(filename.length() - 3).toUpper());
        RUN_TOKEN_TEST("[$4-[length]] Test", "[$4-[length]]", filename, filename.right(filename.length() - 3));
        RUN_TOKEN_TEST("[%4-[length]] Test", "[%4-[length]]", filename, filename.right(filename.length() - 3).toLower());
        RUN_TOKEN_TEST("[*4-[length]] Test", "[*4-[length]]", filename, "St File Name ");
        RUN_TOKEN_TEST("[trimmed;[4-]] Test", "[trimmed;[4-]]", filename, filename.right(filename.length() - 3).trimmed());
        RUN_TOKEN_TEST("[trimmed] Test", "[trimmed]", filename, filename.trimmed());
        RUN_TOKEN_TEST("[length] Test", "[length]", filename, QString::number(filename.length()));
        RUN_TOKEN_TEST("[length-0] Test", "[length-0]", filename, QString::number(filename.length()));
        RUN_TOKEN_TEST("[length-1] Test", "[length-1]", filename, QString::number(filename.length() - 1));
        RUN_TOKEN_TEST("[length-2] Test", "[length-2]", filename, QString::number(filename.length() - 2));
        RUN_TOKEN_TEST("[#length] Test", "[#length]", filename, QString::number(filename.length()));
        RUN_TOKEN_TEST("[#length-0] Test", "[#length-0]", filename, QString::number(filename.length()));
        RUN_TOKEN_TEST("[#length-1] Test", "[#length-1]", filename, QString::number(filename.length() - 1));
        RUN_TOKEN_TEST("[#length-2] Test", "[#length-2]", filename, QString::number(filename.length() - 2));
        RUN_TOKEN_TEST("[####length] Test", "[####length]", filename, QString::asprintf("%04i", (int)filename.length()));
        RUN_TOKEN_TEST("[####length-0] Test", "[####length-0]", filename, QString::asprintf("%04i", (int)filename.length()));
        RUN_TOKEN_TEST("[####length-1] Test", "[####length-1]", filename, QString::asprintf("%04i", (int)filename.length() - 1));
        RUN_TOKEN_TEST("[####length-2] Test", "[####length-2]", filename, QString::asprintf("%04i", (int)filename.length() - 2));
        RUN_TOKEN_TEST("[6-9] Test", "[6-9]", filename, filename.mid(5, 4));
        RUN_TOKEN_TEST("[&6-9] Test", "[&6-9]", filename, filename.mid(5, 4).toUpper());
        RUN_TOKEN_TEST("[$6-9] Test", "[$6-9]", filename, filename.mid(5, 4));
        RUN_TOKEN_TEST("[%6-9] Test", "[%6-9]", filename, filename.mid(5, 4).toLower());
        RUN_TOKEN_TEST("[*6-9] Test", "[*6-9]", filename, filename.mid(5, 4));
        RUN_TOKEN_TEST("[trimmed;[6-9]] Test", "[trimmed;[6-9]]", filename, filename.mid(5, 4).trimmed());
        RUN_TOKEN_TEST("[6;4] Test", "[6;4]", filename, filename.mid(5, 4));
        RUN_TOKEN_TEST("[&6;4] Test", "[&6;4]", filename, filename.mid(5, 4).toUpper());
        RUN_TOKEN_TEST("[$6;4] Test", "[$6;4]", filename, filename.mid(5, 4));
        RUN_TOKEN_TEST("[%6;4] Test", "[%6;4]", filename, filename.mid(5, 4).toLower());
        RUN_TOKEN_TEST("[*6;4] Test", "[*6;4]", filename, filename.mid(5, 4));
        RUN_TOKEN_TEST("[1;1{[length]}] Test", "[1;1{[length]}]", filename,  "1");
        RUN_TOKEN_TEST("[trimmed;[6;4]] Test", "[trimmed;[6;4]]", filename, filename.mid(5, 4).trimmed());
        RUN_TOKEN_TEST("[trimmed; Hallo ] Test", "[trimmed; Hallo ]", filename, "Hallo");
        RUN_TOKEN_TEST("[dirname] Test", "[dirname]", filename, directory1);
        RUN_TOKEN_TEST("[&dirname] Test", "[&dirname]", filename, directory1.toUpper());
        RUN_TOKEN_TEST("[$dirname] Test", "[$dirname]", filename, directory1);
        RUN_TOKEN_TEST("[%dirname] Test", "[%dirname]", filename, directory1.toLower());
        // Don't change the capitalization below just to match the app name style,
        // it'll break the test
        RUN_TOKEN_TEST("[*dirname] Test", "[*dirname]", filename, "Krename");
        RUN_TOKEN_TEST("[trimmed;[dirname]] Test", "[trimmed;[dirname]]", filename, directory1);
        RUN_TOKEN_TEST("[dirname.] Test", "[dirname.]", filename, directory2);
        RUN_TOKEN_TEST("[&dirname.] Test", "[&dirname.]", filename, directory2.toUpper());
        RUN_TOKEN_TEST("[$dirname.] Test", "[$dirname.]", filename, directory2);
        RUN_TOKEN_TEST("[%dirname.] Test", "[%dirname.]", filename, directory2.toLower());
        RUN_TOKEN_TEST("[*dirname.] Test", "[*dirname.]", filename, "Home");
        RUN_TOKEN_TEST("[trimmed;[dirname.]] Test", "[trimmed;[dirname.]]", filename, directory2);
        RUN_TOKEN_TEST("[dirname..] Test", "[dirname..]", filename, "");
        RUN_TOKEN_TEST("[&dirname..] Test", "[&dirname..]", filename, "");
        RUN_TOKEN_TEST("[$dirname..] Test", "[$dirname..]", filename, "");
        RUN_TOKEN_TEST("[%dirname..] Test", "[%dirname..]", filename, "");
        RUN_TOKEN_TEST("[*dirname..] Test", "[*dirname..]", filename, "");
        // TODO: This test has strange effects that only occur if [dirname..] is an empty QString
        //RUN_TOKEN_TEST( "[trimmed;[dirname..]] Test", "[trimmed;[dirname..]]", filename, filename.trimmed() );
        RUN_TOKEN_TEST("Complex Test1", "&[2-5]", filename, filename.toUpper() + "Test");
        RUN_TOKEN_TEST("Complex Test2", "%[2-5]", filename, filename.toLower() + "Test");
        RUN_TOKEN_TEST("Complex Test3", "$[2-5]", filename, filename + "Test");
        RUN_TOKEN_TEST("Complex Test4", "*[2-5]", filename, " Test File Name Test");
        RUN_TOKEN_TEST("Complex Test5", "[trimmed][2-5]", filename, filename.trimmed() + "Test");
        RUN_TOKEN_TEST("Complex Test6", "[&2-5]\\&[length-2]\\&[1;1{Hallo}]", filename, "TEST&14&H");

        // Testing all special Characters in KRename
        RUN_TOKEN_TEST("\\/ Test", "\\/", filename, "%2f");   // this is displayed as a slash,
        // unix filenames are not allowed
        // to contain a slash
        RUN_TOKEN_TEST("\\[ Test", "\\[", filename, "[");
        RUN_TOKEN_TEST("\\] Test", "\\]", filename, "]");
        RUN_TOKEN_TEST("\\$ Test", "\\$", filename, "$");
        RUN_TOKEN_TEST("\\* Test", "\\*", filename, "*");
        RUN_TOKEN_TEST("\\\\ Test", "\\\\", filename, "\\");
        RUN_TOKEN_TEST("\\& Test", "\\&", filename, "&");
        RUN_TOKEN_TEST("\\% Test", "\\%", filename, "%");
        RUN_TOKEN_TEST("\\# Test", "\\#", filename, "#");

        // Testing filenames with special characters
        QString specialname("Test %1 File");
        RUN_TOKEN_TEST("File [ Test", "$", specialname.arg("["), specialname.arg("["));
        RUN_TOKEN_TEST("File ] Test", "$", specialname.arg("]"), specialname.arg("]"));
        RUN_TOKEN_TEST("File $ Test", "$", specialname.arg("$"), specialname.arg("$"));
        RUN_TOKEN_TEST("File * Test", "$", specialname.arg("*"), specialname.arg("*"));
        RUN_TOKEN_TEST("File \\ Test", "$", specialname.arg("\\"), specialname.arg("\\"));
        RUN_TOKEN_TEST("File & Test", "$", specialname.arg("&"), specialname.arg("&"));
        RUN_TOKEN_TEST("File % Test", "$", specialname.arg("%"), specialname.arg("%"));
        RUN_TOKEN_TEST("File # Test", "$", specialname.arg("#"), specialname.arg("#"));

        // load all plugins now
        //PluginLoader::instance()->loadPlugins( false ); // file plugins are not required

        // Testing system functions
        RUN_TOKEN_TEST("Date Test", "[date]", filename, QDateTime::currentDateTime().toString("dd-MM-yyyy"));
        RUN_TOKEN_TEST("dd-MM-yyyy Date Test", "[date;dd-MM-yyyy]",
                       filename, QDateTime::currentDateTime().toString("dd-MM-yyyy"));
        RUN_TOKEN_TEST("dd:MM:yyyy Date Test", "[date;dd:MM:yyyy]",
                       filename, QDateTime::currentDateTime().toString("dd:MM:yyyy"));
        RUN_TOKEN_TEST("yy.mm.dd Date Test", "[date;yy.mm.dd]",
                       filename, QDateTime::currentDateTime().toString("yy.mm.dd"));
        RUN_TOKEN_TEST("d Date Test", "[date;d]", filename, QDateTime::currentDateTime().toString("d"));
        RUN_TOKEN_TEST("dd Date Test", "[date;dd]", filename, QDateTime::currentDateTime().toString("dd"));
        RUN_TOKEN_TEST("ddd Date Test", "[date;ddd]", filename, QDateTime::currentDateTime().toString("ddd"));
        RUN_TOKEN_TEST("dddd Date Test", "[date;dddd]", filename, QDateTime::currentDateTime().toString("dddd"));
        RUN_TOKEN_TEST("M Date Test", "[date;M]", filename, QDateTime::currentDateTime().toString("M"));
        RUN_TOKEN_TEST("MM Date Test", "[date;MM]", filename, QDateTime::currentDateTime().toString("MM"));
        RUN_TOKEN_TEST("MMM Date Test", "[date;MMM]", filename, QDateTime::currentDateTime().toString("MMM"));
        RUN_TOKEN_TEST("MMMM Date Test", "[date;MMMM]", filename, QDateTime::currentDateTime().toString("MMMM"));
        RUN_TOKEN_TEST("yy Date Test", "[date;yy]", filename, QDateTime::currentDateTime().toString("yy"));
        RUN_TOKEN_TEST("yyyy Date Test", "[date;yyyy]", filename, QDateTime::currentDateTime().toString("yyyy"));

        RUN_TOKEN_TEST("h Date Test", "[date;h]", filename, QDateTime::currentDateTime().toString("h"));
        RUN_TOKEN_TEST("hh Date Test", "[date;hh]", filename, QDateTime::currentDateTime().toString("hh"));
        RUN_TOKEN_TEST("m Date Test", "[date;m]", filename, QDateTime::currentDateTime().toString("m"));
        RUN_TOKEN_TEST("mm Date Test", "[date;mm]", filename, QDateTime::currentDateTime().toString("mm"));
        RUN_TOKEN_TEST("s Date Test", "[date;s]", filename, QDateTime::currentDateTime().toString("s"));
        RUN_TOKEN_TEST("ss Date Test", "[date;ss]", filename, QDateTime::currentDateTime().toString("ss"));
        // Current computers are to slow to compare two milliseconds as the instruction is longer than
        // a millisecond.
        //
        // RUN_TOKEN_TEST( "z Date Test", "[date;z]", filename, QDateTime::currentDateTime().toString( "z") );
        // RUN_TOKEN_TEST( "zzz Date Test", "[date;zzz]", filename, QDateTime::currentDateTime().toString( "zzz") );
        RUN_TOKEN_TEST("ap Date Test", "[date;ap]", filename, QDateTime::currentDateTime().toString("ap"));
        RUN_TOKEN_TEST("AP Date Test", "[date;AP]", filename, QDateTime::currentDateTime().toString("AP"));
        RUN_TOKEN_TEST("Day Test", "[day]", filename, QDateTime::currentDateTime().toString("dd"));
        RUN_TOKEN_TEST("Month Test", "[month]", filename, QDateTime::currentDateTime().toString("MM"));
        RUN_TOKEN_TEST("Year Test", "[year]", filename, QDateTime::currentDateTime().toString("yyyy"));
        RUN_TOKEN_TEST("Hour Test", "[hour]", filename, QDateTime::currentDateTime().toString("hh"));
        RUN_TOKEN_TEST("Minute Test", "[minute]", filename, QDateTime::currentDateTime().toString("mm"));
        RUN_TOKEN_TEST("Second Test", "[second]", filename, QDateTime::currentDateTime().toString("ss"));
        RUN_TOKEN_TEST("Time Test", "[time]", filename, QDateTime::currentDateTime().toString("hh-mm-ss"));

        // Testing numbering name, start, step, skip
        RUN_NUMBER_TESTS("Numbers 0- Step 1", 0, 1, QList<int>());
        RUN_NUMBER_TESTS("Numbers 1- Step 1", 1, 1, QList<int>());
        RUN_NUMBER_TESTS("Numbers 2- Step 1", 2, 1, QList<int>());
        RUN_NUMBER_TESTS("Numbers -2- Step 1", -2, 1, QList<int>());

        RUN_NUMBER_TESTS("Numbers 0- Step 2", 0, 2, QList<int>());
        RUN_NUMBER_TESTS("Numbers 1- Step 2", 1, 2, QList<int>());
        RUN_NUMBER_TESTS("Numbers 2- Step 2", 2, 2, QList<int>());
        RUN_NUMBER_TESTS("Numbers -2- Step 2", -2, 2, QList<int>());

        RUN_NUMBER_TESTS("Numbers 0- Step 7", 0, 7, QList<int>());
        RUN_NUMBER_TESTS("Numbers 1- Step 7", 1, 7, QList<int>());
        RUN_NUMBER_TESTS("Numbers 2- Step 7", 2, 7, QList<int>());
        RUN_NUMBER_TESTS("Numbers -2- Step 7", -2, 7, QList<int>());

        RUN_NUMBER_TESTS("Numbers 0- Step -3", 0, -3, QList<int>());
        RUN_NUMBER_TESTS("Numbers 1- Step -3", 1, -3, QList<int>());
        RUN_NUMBER_TESTS("Numbers 2- Step -3", 2, -3, QList<int>());
        RUN_NUMBER_TESTS("Numbers -2- Step -3", -2, -3, QList<int>());

        RUN_NUMBER_TESTS("Skip 0- Step 1", 0, 1, QList<int>() << 1 << 2 << 3 << 4 << 89);
        RUN_NUMBER_TESTS("Skip 1- Step 1", 1, 1, QList<int>() << 1 << 2 << 3 << 4 << 89);
        RUN_NUMBER_TESTS("Skip 2- Step 1", 2, 1, QList<int>() << 1 << 2 << 3 << 4 << 89);
        RUN_NUMBER_TESTS("Skip -2- Step 1", -2, 1, QList<int>() << 1 << 2 << 3 << 4 << 89);

        RUN_NUMBER_TESTS("Skip 10- Step 79", 10, 79, QList<int>() << 1 << 2 << 3 << 4 << 89);
        RUN_NUMBER_TESTS("Skip 10- Step -2", 10, -2, QList<int>() << 1 << 2 << 3 << 4 << 89);
        RUN_NUMBER_TESTS("Skip 10- Step -1", 10, -1, QList<int>() << 1 << 2 << 3 << 4 << 89);

        // Test the find and replace feature of KRename
        RUN_REPLACE_TEST("Replace: Spaces", "$", "Filename with spaces", "Filename_with_spaces", " ", "_", false);
        RUN_REPLACE_TEST("Replace: Nothing", "$", "Filename", "Filename", " ", "_", false);
        RUN_REPLACE_TEST("Replace: Word", "$", "Filename with spaces", "Filename HAS spaces", "with", "HAS", false);
        RUN_REPLACE_TEST("Replace: $", "$", "Filename with $ and spaces", "Filename with ! and spaces", "$", "!", false);
        RUN_REPLACE_TEST("Replace: &", "$", "Filename with & and spaces", "Filename with ! and spaces", "&", "!", false);
        RUN_REPLACE_TEST("Replace: %", "$", "Filename with % and spaces", "Filename with ! and spaces", "%", "!", false);
        RUN_REPLACE_TEST("Replace: *", "$", "Filename with * and spaces", "Filename with ! and spaces", "*", "!", false);
        RUN_REPLACE_TEST("Replace: [", "$", "Filename with [ and spaces", "Filename with ! and spaces", "[", "!", false);
        RUN_REPLACE_TEST("Replace: ]", "$", "Filename with ] and spaces", "Filename with ! and spaces", "]", "!", false);
        RUN_REPLACE_TEST("Replace: #", "$", "Filename with # and spaces", "Filename with ! and spaces", "#", "!", false);
        RUN_REPLACE_TEST("Replace: to $", "$", "Filename with spaces", "Filename $ spaces", "with", "$", false);
        RUN_REPLACE_TEST("Replace: to &", "$", "Filename with spaces", "Filename & spaces", "with", "&", false);
        RUN_REPLACE_TEST("Replace: to %", "$", "Filename with spaces", "Filename % spaces", "with", "%", false);
        RUN_REPLACE_TEST("Replace: to *", "$", "Filename with spaces", "Filename * spaces", "with", "*", false);
        RUN_REPLACE_TEST("Replace: to [", "$", "Filename with spaces", "Filename [ spaces", "with", "[", false);
        RUN_REPLACE_TEST("Replace: to ]", "$", "Filename with spaces", "Filename ] spaces", "with", "]", false);
        RUN_REPLACE_TEST("Replace: to #", "$", "Filename with spaces", "Filename # spaces", "with", "#", false);

        RUN_REPLACE_TEST("RegExp: ?", "$", "Filename", "AAAAAAAAA", "[a-zA-z]?", "A", true);
        RUN_REPLACE_TEST("RegExp: {1}", "$", "Filename", "AAAAAAAA", "[a-zA-z]{1}", "A", true);
        RUN_REPLACE_TEST("RegExp: +", "$", "Filename", "A", "[a-zA-z]+", "A", true);
        RUN_REPLACE_TEST("RegExp: \\d", "$", "Filename 123", "Filename NumberNumberNumber", "\\d", "Number", true);
        RUN_REPLACE_TEST("RegExp: \\d+", "$", "Filename 123", "Filename Number", "\\d+", "Number", true);
        RUN_REPLACE_TEST("RegExp: Match", "$", "Filename 123", "MATCHING", "[a-zA-z]* \\d{3}", "MATCHING", true);

        // TODO:
        // TODO: Manual Change Test

        // Regression tests
        // Caused an infinite loop
        RUN_TOKEN_TEST("[1-2 [4-] Test", "[1-2 [4-]", filename, ' ' + filename.right(filename.length() - 3));
        const char *regTemplate = "###{329;13";
        RUN_TOKEN_TEST(regTemplate, regTemplate, filename, "329");
        RUN_TOKEN_TEST("ABC {1", "ABC {1", filename, "ABC {1");

        // Brackets appeared as \[ in the final result
        QString regressionName = "1-07 Take Flight (Wings) [Pocketman]";
        QString regressionExpect = "100-Take Flight (Wings) [Pocketman]";
        RUN_TOKEN_TEST("1##-[$6-] Test", "1##-[$6-]", regressionName, regressionExpect);

        KRenameFile::List files;
        QStringList expected;
        QStringList expectedPaths;

        files << KRenameFile(QUrl("/home/foo/bar/subdir"), true, eSplitMode_FirstDot, 1)
              << KRenameFile(QUrl("/home/foo"), true, eSplitMode_FirstDot, 1)
              << KRenameFile(QUrl("/home/foo/bar"), true, eSplitMode_FirstDot, 1)
              << KRenameFile(QUrl("/home/foo/bar/baz"), true, eSplitMode_FirstDot, 1);
        expected << "SUBDIR" << "FOO" << "BAR" << "BAZ";
        expectedPaths << "/home/foo/bar" << "/home" << "/home/FOO" << "/home/FOO/BAR";

        RUN_KRENAME_MULTI_FILE_TEST(files, expected, expectedPaths,
                                    "&", "Testing a directory hierarchy");
    }

    bool KRenameTest::tokenTest(const char *token, const QString &filename, const QString &expected)
    {
        QString directory("/home/krename/");
        KRenameFile::List list;
        KRenameFile file(QUrl(directory + filename), filename.isEmpty(), eSplitMode_FirstDot, 1);

        list.push_back(file);

        BatchRenamer b;
        b.setFilenameTemplate(token);
        b.setFiles(&list);
        b.processFilenames();

        QString str = list[0].dstFilename();

        bool result = (str == expected);
        if (m_verbose || !result)
            writeTestMessage(" ---> Expected: (%s) Got: (%s) Token: (%s)",
                             expected.toLatin1().data(),
                             str.toLatin1().data(), token);

        return result;
    }

    bool KRenameTest::numberingTest(int length, int start, int step, const QList<int> &skip, int num)
    {
        QString directory("/home/krename/");
        KRenameFile::List list;
        QString token;
        token.fill('#', length);

        for (int i = 0; i < num; i++) {
            QString filename = "any" + QString::number(i);
            KRenameFile file(QUrl(directory + filename), filename.isEmpty(), eSplitMode_FirstDot, 1);

            list.push_back(file);
        }

        BatchRenamer b;
        b.setFilenameTemplate(token);
        b.setFiles(&list);
        b.setNumberStepping(step);
        b.setNumberStartIndex(start);
        b.setNumberSkipList(skip);
        b.processFilenames();

        int cur = start;
        bool result = true;
        QString expected;

        while (skip.contains(cur)) {
            cur += step;
        }

        KRenameFile::List::ConstIterator it = list.constBegin();

        while (it != list.end()  && result) {
            expected = QString::asprintf("%0*i", length, cur);
            result = ((*it).dstFilename() == expected);
            if (m_verbose || !result)
                qDebug(" ---> Expected: (%s) Got: (%s) Start: %i Step: %i Token: (%s)",
                       expected.toLatin1().data(), (*it).dstFilename().toLatin1().data(), start, step, token.toLatin1().data());

            do {
                cur += step;
            } while (skip.contains(cur));

            ++it;
        }

        return result;
    }

    bool KRenameTest::replaceTest(const QString &token, const QString &filename, const QString &expected,
                                  const QString &replace, const QString &with, bool regExp)
    {
        QString directory("/home/krename/");
        KRenameFile::List list;
        KRenameFile file(QUrl(directory + filename), filename.isEmpty(), eSplitMode_FirstDot, 1);
        list.push_back(file);

        QList<TReplaceItem> replaceList;

        TReplaceItem strings;
        strings.find    = replace;
        strings.replace = with;
        strings.reg     = regExp;
        strings.doProcessTokens = false;

        replaceList.append(strings);

        BatchRenamer b;
        b.setFilenameTemplate(token);
        b.setFiles(&list);
        b.setReplaceList(replaceList);
        b.processFilenames();

        QString str = list[0].dstFilename();
        bool result = (str == expected);
        if (m_verbose || !result)
            writeTestMessage(" ---> Expected: (%s) Got: (%s) Token: (%s)",
                             expected.toLatin1().data(),
                             str.toLatin1().data(), token.toLatin1().data());

        return result;

    }

    bool KRenameTest::testMultipleFiles(KRenameFile::List &files, const QStringList &expected, const QStringList &expectedPath, const QString &token)
    {
        BatchRenamer b;
        b.setFilenameTemplate(token);
        b.setFiles(&files);
        b.processFilenames();

        bool result = true;
        for (int i = 0; i < files.count(); i++) {
            QString str = files[i].dstFilename();
            QString strPath = files[i].dstDirectory();
            bool strResult = (str == expected[i]);
            bool pathResult = (strPath == expectedPath[i]);
            if (m_verbose || !strResult) {
                writeTestMessage(" ---> Expected: (%s) Got: (%s) Token: (%s)",
                                 expected[i].toLatin1().data(),
                                 str.toLatin1().data(), token.toLatin1().data());
            }
            if (m_verbose || !pathResult) {
                writeTestMessage(" ---> Expected: (%s) Got: (%s) Token: (%s)",
                                 expectedPath[i].toLatin1().data(),
                                 strPath.toLatin1().data(), token.toLatin1().data());
            }

            result = result && strResult && pathResult;
        }

        return result;
    }

