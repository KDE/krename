// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2021 Heiko Becker <heiko.becker@kde.org>

#ifndef TOKEN_TEST_H
#define TOKEN_TEST_H

#include <QObject>

/** This class contains KRename's selftest.
 *  The selftest consists of several unit tests for
 *  important classes.
 *
 *  Currently covered classes:
 *
 *  - KRenameFile
 */
class TokenTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testTokens_data();
    void testTokens();
    void testReplacing_data();
    void testReplacing();
};

#endif // TOKEN_TEST_H
