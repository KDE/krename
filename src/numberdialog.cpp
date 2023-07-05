// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#include "numberdialog.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>

NumberDialog::NumberDialog(int start, int step, bool reset, const QList<int> &skip, QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QWidget     *widget = new QWidget(this);

    m_widget.setupUi(widget);
    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);

    layout->addWidget(widget);
    layout->addWidget(m_buttons);

    connect(m_buttons, &QDialogButtonBox::accepted,
            this, &NumberDialog::accept);
    connect(m_buttons, &QDialogButtonBox::rejected,
            this, &NumberDialog::reject);

    connect(m_widget.buttonAdd, &QPushButton::clicked,
            this, &NumberDialog::slotAddNumber);
    connect(m_widget.buttonRemove, &QPushButton::clicked, this,
            &NumberDialog::slotRemoveNumber);
    connect(m_widget.listSkip, &QListWidget::itemSelectionChanged,
            this, &NumberDialog::slotEnableControls);
    m_widget.spinStart->setValue(start);
    m_widget.spinStep->setValue(step);
    m_widget.checkReset->setChecked(reset);
    m_widget.listSkip->setSortingEnabled(true);   // TODO: sort numeric

    QList<int>::ConstIterator it = skip.constBegin();
    while (it != skip.constEnd()) {
        m_widget.listSkip->addItem(QString::number(*it));
        ++it;
    }
}

QList<int> NumberDialog::skipNumbers() const
{
    QList<int> list;

    for (int i = 0; i < m_widget.listSkip->count(); i++) {
        QListWidgetItem *item = m_widget.listSkip->item(i);
        list.append(item->data(Qt::DisplayRole).toInt());

    }
    return list;
}

void NumberDialog::slotEnableControls()
{
    QList<QListWidgetItem *> selected = m_widget.listSkip->selectedItems();
    m_widget.buttonRemove->setEnabled(selected.size());
}

void NumberDialog::slotAddNumber()
{
    m_widget.listSkip->addItem(QString::number(m_widget.spinNumber->value()));
}

void NumberDialog::slotRemoveNumber()
{
    delete m_widget.listSkip->takeItem(m_widget.listSkip->currentRow());
}

#include "moc_numberdialog.cpp"
