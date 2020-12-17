// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2001 Dominik Seichter <domseichter@web.de>

#include "plugin.h"

#include "tokenhelpdialog.h"

Plugin::Plugin(PluginLoader *loader)
    : m_pluginLoader(loader), m_enabled(false)
{
}

Plugin::~Plugin()
{

}

void Plugin::loadConfig(KConfigGroup &)
{
    /* Do nothing by default */
}

void Plugin::saveConfig(KConfigGroup &) const
{
    /* Do nothing by default */
}

QString Plugin::createHelpEntry(const QString &token, const QString &help)
{
    QString cmd('[');
    cmd = cmd + token + ']' + TokenHelpDialog::getTokenSeparator();
    cmd = cmd + help;

    return cmd;
}

