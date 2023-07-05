// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#include "exthistorycombo.h"

#include <kconfig.h>
#include <kconfiggroup.h>
#include <KSharedConfig>

#include <QLineEdit>

#define EXT_HISTORY_COMBO_MAX_COUNT 10
#define EXT_HISTORY_COMBO_TIMER_DELAY 500

ExtHistoryCombo::ExtHistoryCombo(QWidget *parent)
    : KHistoryComboBox(parent)
{
    connect(this, static_cast<void (ExtHistoryCombo::*)(const QString &)>(&ExtHistoryCombo::activated),
            this, &ExtHistoryCombo::addToHistory);
    connect(this, &ExtHistoryCombo::editTextChanged,
            this, &ExtHistoryCombo::slotTextChanged);
    connect(&m_timer, &QTimer::timeout,
            this, &ExtHistoryCombo::delayedTextChanged);

    this->setMaxCount(EXT_HISTORY_COMBO_MAX_COUNT);
    this->setDuplicatesEnabled(false);

    m_timer.setSingleShot(true);
}

void ExtHistoryCombo::slotTextChanged()
{
    m_timer.stop();
    m_timer.start(EXT_HISTORY_COMBO_TIMER_DELAY);
}

void ExtHistoryCombo::loadConfig()
{
    QString currentText = this->currentText();
    QStringList history;
    QStringList completion;

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup groupGui = config->group(QString("ExtHistoryCombo") + this->objectName());

    completion = groupGui.readEntry("CompletionList", QStringList());
    history = groupGui.readEntry("HistoryList", QStringList());

    this->completionObject()->setItems(completion);
    this->setHistoryItems(history);
    this->lineEdit()->setText(currentText); // Preserve current text
}

void ExtHistoryCombo::saveConfig()
{
    addToHistory(currentText());

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup groupGui = config->group(QString("ExtHistoryCombo") + this->objectName());

    groupGui.writeEntry("CompletionList", this->completionObject()->items());
    groupGui.writeEntry("HistoryList", this->historyItems());

    config->sync();
}

void ExtHistoryCombo::selectAll()
{
    this->lineEdit()->setSelection(0, this->lineEdit()->text().length());
}

#include "moc_exthistorycombo.cpp"
