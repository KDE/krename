// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#include "insertpartfilenamedlg.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>

InsertPartFilenameDlg::InsertPartFilenameDlg(const QString &filename, QWidget *parent)
    : QDialog(parent), m_start(-1), m_end(0)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QWidget     *widget = new QWidget(this);

    m_widget.setupUi(widget);
    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close, Qt::Horizontal, this);

    m_widget.textFilename->setText(filename);
    m_widget.textFilename->setCursorPosition(0);

    layout->addWidget(widget);
    layout->addWidget(m_buttons);

    connect(m_buttons, &QDialogButtonBox::accepted,
            this, &InsertPartFilenameDlg::accept);
    connect(m_buttons, &QDialogButtonBox::rejected,
            this, &InsertPartFilenameDlg::reject);

    connect(m_widget.checkInvert, &QCheckBox::clicked,
            this, &InsertPartFilenameDlg::slotUpdateKRenameCommand);
    connect(m_widget.textFilename, &SelectionSafeLineEdit::selectionChanged,
            this, &InsertPartFilenameDlg::slotSelectionChanged);
    connect(m_widget.comboConvert, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &InsertPartFilenameDlg::slotSelectionChanged);

    this->slotUpdateKRenameCommand();
}

void InsertPartFilenameDlg::slotSelectionChanged()
{
    m_start      = m_widget.textFilename->selectionStart();
    QString text = m_widget.textFilename->selectedText();
    m_end        = m_start + text.length();

    slotUpdateKRenameCommand();
}

void InsertPartFilenameDlg::slotUpdateKRenameCommand()
{
    m_command        = "";
    bool hasSelected = m_start != -1;
    int  cursorPos   = m_widget.textFilename->cursorPosition();
    int  end         = m_end;
    int  start       = m_start;

    char conversionflag = '$';
    if (m_widget.comboConvert->currentIndex() == 1) {
        conversionflag = '%';
    } else if (m_widget.comboConvert->currentIndex() == 2) {
        conversionflag = '&';
    } else if (m_widget.comboConvert->currentIndex() == 3) {
        conversionflag = '*';
    }

    if (!m_widget.textFilename->text().isEmpty()) {
        if (m_widget.checkInvert->isChecked() && hasSelected) {
            // inverted
            if (end) {
                start++;
                end++;
                if (start > 1) {
                    m_command = QString("[%1;%2]").arg(conversionflag).arg(start - 1);
                }

                if (end <= (signed int)m_widget.textFilename->text().length()) {
                    m_command.append(QString("[%1%2-[length]]").arg(conversionflag).arg(end));
                }
            }
        } else if (m_widget.checkInvert->isChecked() && !hasSelected) {
            m_command = QString("[%1").arg(conversionflag) + QString("1;%1][%3%2-[length]]").arg(cursorPos).arg(cursorPos + 1);
        } else if (!m_widget.checkInvert->isChecked() && hasSelected) {
            if (end) {
                start++;
                end++;
                if (end <= (signed int)m_widget.textFilename->text().length()) {
                    m_command = QString("[%1%2;%3]").arg(conversionflag).arg(start).arg(end - start);
                } else {
                    m_command = QString("[%1%2-[length]]").arg(conversionflag).arg(start);
                }
            }
        } else if (!m_widget.checkInvert->isChecked() && !hasSelected) {
            m_command = QString("[%1%2-[length]]").arg(conversionflag).arg(cursorPos);
        }

    }

    m_widget.labelPreview->setText(m_command);
}

#include "moc_insertpartfilenamedlg.cpp"
