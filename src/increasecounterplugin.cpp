// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2008 Dominik Seichter <domseichter@web.de>

#include "increasecounterplugin.h"

#include "ui_increasecounterpluginwidget.h"
#include "pluginloader.h"

#include <QIcon>
#include <QRegExp>

IncreaseCounterPlugin::IncreaseCounterPlugin(PluginLoader *loader)
    : Plugin(loader), m_offset(0)
{
    m_widget = new Ui::IncreaseCounterPluginWidget();
}

IncreaseCounterPlugin::~IncreaseCounterPlugin()
{
    delete m_widget;
}

const QString IncreaseCounterPlugin::name() const
{
    return i18n("Increase Counter");
}

const QIcon IncreaseCounterPlugin::icon() const
{
    return QIcon::fromTheme("document-properties");
}

QString IncreaseCounterPlugin::processFile(BatchRenamer *, int, const QString &filenameOrToken, EPluginType)
{
    // Split string into prenum, number and postnum parts
    QRegExp splitit("(\\D*)(\\d+)(.*)");

    // Is there anything to increment ?
    if (splitit.exactMatch(filenameOrToken)) {
        QString prenum  = splitit.cap(1);
        long    tmp     = splitit.cap(2).toLong();
        QString postnum = splitit.cap(3);

        tmp += m_offset;

        return (prenum + QString::asprintf("%0*li", (int)splitit.cap(2).length(), tmp) + postnum);
    }
    return QString();
}

void IncreaseCounterPlugin::createUI(QWidget *parent) const
{
    m_widget->setupUi(parent);

    connect(m_widget->spinOffset, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &IncreaseCounterPlugin::slotOffsetChanged);
}

void IncreaseCounterPlugin::slotOffsetChanged(int offset)
{
    m_offset = offset;
    m_pluginLoader->sendUpdatePreview();
}

#include "moc_increasecounterplugin.cpp"
