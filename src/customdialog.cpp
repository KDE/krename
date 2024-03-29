// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#include "customdialog.h"

#include "krenamefile.h"

#include <KConfigGroup>

#include <QDialogButtonBox>

CustomDialog::CustomDialog(const KRenameFile &file, QWidget *parent)
    : QDialog(parent)
{
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);
    m_widget.setupUi(mainWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted,
            this, &CustomDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected,
            this, &CustomDialog::reject);
    mainLayout->addWidget(buttonBox);

    connect(m_widget.radioKRename, &QRadioButton::clicked,
            this, &CustomDialog::slotEnableControls);
    connect(m_widget.radioInput, &QRadioButton::clicked,
            this, &CustomDialog::slotEnableControls);
    connect(m_widget.radioCustom, &QRadioButton::clicked,
            this, &CustomDialog::slotEnableControls);

    // Set default vallues
    m_widget.radioCustom->setChecked(true);
    m_widget.radioKRename->setChecked(false);
    m_widget.radioInput->setChecked(false);

    QString srcFilename = file.srcFilename();
    if (!file.srcExtension().isEmpty()) {
        srcFilename += '.';
        srcFilename += file.srcExtension();
    }

    QString krenameFilename = file.dstFilename();
    if (!file.dstExtension().isEmpty()) {
        krenameFilename += '.';
        krenameFilename += file.dstExtension();
    }

    if (!file.manualChanges().isNull()) {
        switch (file.manualChangeMode()) {
        case eManualChangeMode_Custom:
            krenameFilename = file.manualChanges();
            break;
        case eManualChangeMode_Input:
            m_widget.radioInput->setChecked(true);
            m_widget.radioKRename->setChecked(false);
            m_widget.radioCustom->setChecked(false);
            srcFilename = file.manualChanges();
            break;
        case eManualChangeMode_None:
        default:
            break;
        }
    }

    m_widget.labelPreview->setPixmap(file.icon());
    m_widget.lineEditInput->setText(srcFilename);
    m_widget.lineEditCustom->setText(krenameFilename);
    slotEnableControls();

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(QString("CustomDialogGroup"));
    restoreGeometry(group.readEntry<QByteArray>("Geometry", QByteArray()));
}

CustomDialog::~CustomDialog()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(QString("CustomDialogGroup"));
    group.writeEntry("Geometry", saveGeometry());
}

void CustomDialog::slotEnableControls()
{
    m_widget.lineEditCustom->setEnabled(m_widget.radioCustom->isChecked());
    m_widget.lineEditInput->setEnabled(m_widget.radioInput->isChecked());
}

bool CustomDialog::hasManualChanges() const
{
    return !(m_widget.radioKRename->isChecked());
}

const QString CustomDialog::manualChanges() const
{
    if (m_widget.radioCustom->isChecked()) {
        return m_widget.lineEditCustom->text();
    } else if (m_widget.radioInput->isChecked()) {
        return m_widget.lineEditInput->text();
    } else {
        return QString();
    }
}

EManualChangeMode CustomDialog::manualChangeMode() const
{
    if (m_widget.radioCustom->isChecked()) {
        return eManualChangeMode_Custom;
    } else if (m_widget.radioInput->isChecked()) {
        return eManualChangeMode_Input;
    } else {
        return eManualChangeMode_None;
    }

}

#include "moc_customdialog.cpp"
