// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2002 Dominik Seichter <domseichter@web.de>

#include "fileplugin.h"

#include <KIconLoader>
#include <KLocalizedString>
#include <kservice.h>

#include <QListWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QRegExp>

FilePlugin::FilePlugin(PluginLoader *loader, KService *service)
    : Plugin(loader),
      m_name(service->name()),
      m_comment(QString()),
      m_icon(service->icon())
{
}

FilePlugin::FilePlugin(PluginLoader *loader)
    : Plugin(loader),
      m_name("FilePlugin")
{
}

FilePlugin::~FilePlugin()
{

}

bool FilePlugin::supports(const QString &token)
{
    QString lower = token.toLower();

    for (int i = 0; i < m_keys.count(); i++)
        // TODO: Maybe we can optimize by putting all tokens
        //       already converted to lowercase into m_keys
        if (QRegExp(m_keys[i].toLower()).exactMatch(lower)) {
            return true;
        }

    return false;
}

const QIcon FilePlugin::icon() const
{
    return QIcon::fromTheme(m_icon);
}

void FilePlugin::createUI(QWidget *parent) const
{
    QSpacerItem *spacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *l    = new QVBoxLayout(parent);
    QHBoxLayout *hbox = new QHBoxLayout;

    QLabel *pix = new QLabel(parent);
    pix->setPixmap(QIcon::fromTheme(m_icon).pixmap(KIconLoader::SizeMedium));

    hbox->addWidget(pix);
    hbox->addWidget(new QLabel("<qt><b>" + name() + "</b></qt>", parent));
    hbox->addItem(spacer);

    QLabel *comment = new QLabel(m_comment, parent);
    comment->setWordWrap(true);
    l->addLayout(hbox);
    l->addWidget(comment);
    l->addWidget(new QLabel(i18n("Supported tokens:"), parent));

    QListWidget *list = new QListWidget(parent);

    const QStringList &keys = supportedTokens();

    for (int i = 0; i < keys.count(); i++) {
        list->insertItem(0, '[' + keys[i] + ']');
    }

    l->addWidget(list);
    l->setStretchFactor(list, 2);
}

