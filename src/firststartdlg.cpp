// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#include "firststartdlg.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>

FirstStartDlg::FirstStartDlg(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QWidget     *widget = new QWidget(this);

    m_selector.setupUi(widget);
    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);

    layout->addWidget(widget);
    layout->addWidget(m_buttons);

    connect(m_buttons, &QDialogButtonBox::accepted,
            this, &FirstStartDlg::accept);
}

