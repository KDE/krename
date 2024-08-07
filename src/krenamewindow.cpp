// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#include "krenamewindow.h"

#include "krenamemodel.h"
#include "plugin.h"
#include "pluginloader.h"
#include "richtextitemdelegate.h"
#include "startupinfo.h"
#include "tokensortmodedialog.h"

#include "ui_krenamefiles.h"
#include "ui_krenamedestination.h"
//#include "ui_krenamesimple.h"
#include "ui_krenameplugins.h"
#include "ui_krenamefilename.h"

#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QPushButton>
#include <kseparator.h>

#include <QDialogButtonBox>
#include <QStackedWidget>
#include <QTabBar>
#include <QVBoxLayout>

#include <KHelpMenu>
#include <KLazyLocalizedString>

static const KRenameWindow::TGuiMode tAdvancedMode = {
    4,
    {
        kli18n("&1. Files"),
        kli18n("&2. Destination"),
        kli18n("&3. Plugins"),
        kli18n("&4. Filename")
    },
    {
        0, 1, 2, 3
    },
    {
        "document-open-folder",
        "document-save",
        "configure",
        "edit-rename"
    }
};

KRenameWindow::KRenameWindow(QWidget *parent)
    : KMainWindow(parent),
      m_curPage(0), m_guiMode(nullptr),
      m_fileCount(0)
{
    QWidget     *center = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(center);

    m_delegate = new RichTextItemDelegate(this);
    m_tabBar  = new QTabBar(center);
    m_stack   = new QStackedWidget(center);
    m_buttons = new QDialogButtonBox(center);

    layout->addWidget(m_tabBar);
    layout->addWidget(m_stack);
    layout->addWidget(new KSeparator(Qt::Horizontal, center));
    layout->addWidget(m_buttons);
    layout->setStretchFactor(m_stack, 2);

    this->setCentralWidget(center);

    for (int i = 0; i < tAdvancedMode.numPages; i++) {
        const QIcon &icon = QIcon::fromTheme(tAdvancedMode.pageIcons[i]);
        m_tabBar->addTab(icon, tAdvancedMode.pageTitles[i].toString());
    }

    m_pageFiles    = new Ui::KRenameFiles();
    m_pageDests    = new Ui::KRenameDestination();
    m_pagePlugins  = new Ui::KRenamePlugins();
    m_pageFilename = new Ui::KRenameFilename();

    // add files page
    QWidget *page = new QWidget(m_stack);
    m_pageFiles->setupUi(page);
    m_stack->addWidget(page);

    // add destination page
    page = new QWidget(m_stack);
    m_pageDests->setupUi(page);
    m_stack->addWidget(page);

    // add plugin page
    page = new QWidget(m_stack);
    m_pagePlugins->setupUi(page);
    m_stack->addWidget(page);

    // add filename page
    page = new QWidget(m_stack);
    m_pageFilename->setupUi(page);
    m_stack->addWidget(page);

    setupGui();
    setupPlugins();
    setupIcons();

    StartUpInfo *startUp = new StartUpInfo();
    connect(startUp, &StartUpInfo::addFiles, this, &KRenameWindow::addFiles);
    connect(startUp, &StartUpInfo::enterTemplate,
            this, &KRenameWindow::slotGotoTemplatesPage);

    m_pageDests->urlrequester->setMode(KFile::Directory | KFile::ExistingOnly);
    m_pageFiles->fileList->setItemDelegate(m_delegate);
    m_pageFiles->fileList->setInfoWidget(startUp);

    // Make sure that now signal occurs before setupGui was called
    connect(m_tabBar, &QTabBar::currentChanged,
            this, &KRenameWindow::showPage);
    connect(m_buttonClose, &QPushButton::clicked,
            this, &KRenameWindow::close);
    connect(m_buttons, &QDialogButtonBox::accepted,
            this, &KRenameWindow::slotFinish);

    this->setAutoSaveSettings("KRenameWindowSettings", true);

    // If there is a huge entry in the history of filenameTemplate that size
    // apparently gets used as minimal size of the window. Setting it to some
    // value here works around that.
    // https://bugs.kde.org/show_bug.cgi?id=398980
    // TODO: Find a solution without a magic number
    setMinimumWidth(200);

    // Show the first page in any mode
    showPage(0);
}

KRenameWindow::~KRenameWindow()
{
}

void KRenameWindow::loadConfig()
{
    // ExtHistoryCombo needs an object name
    m_pageFilename->filenameTemplate->setObjectName("FILENAME_TEMPLATE");
    m_pageFilename->extensionTemplate->setObjectName("EXTENSION_TEMPLATE");
    m_pageFilename->comboFilenameCustom->setObjectName("FILENAME_CUSTOM_TEMPLATE");
    m_pageFilename->comboExtensionCustom->setObjectName("EXTENSION_CUSTOM_TEMPLATE");
    m_pageFilename->comboPrefixCustom->setObjectName("PREVIX_CUSTOM_TEMPLATE");
    m_pageFilename->comboSuffixCustom->setObjectName("SUFFIX_CUSTOM_TEMPLATE");

    m_pageFilename->filenameTemplate->loadConfig();
    m_pageFilename->extensionTemplate->loadConfig();
    m_pageFilename->comboFilenameCustom->loadConfig();
    m_pageFilename->comboExtensionCustom->loadConfig();
    m_pageFilename->comboPrefixCustom->loadConfig();
    m_pageFilename->comboSuffixCustom->loadConfig();

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup groupGui = config->group(QString("GUISettings"));

    int width = groupGui.readEntry("Column0", QVariant(this->previewColumnWidth(0))).toInt();
    if (width > 0) {
        this->setPreviewColumnWidth(0, width);
    }

    width = groupGui.readEntry("Column1", QVariant(this->previewColumnWidth(1))).toInt();
    if (width > 0) {
        this->setPreviewColumnWidth(1, width);
    }
}

void KRenameWindow::saveConfig()
{
    m_pageFilename->filenameTemplate->saveConfig();
    m_pageFilename->extensionTemplate->saveConfig();
    m_pageFilename->comboFilenameCustom->saveConfig();
    m_pageFilename->comboExtensionCustom->saveConfig();
    m_pageFilename->comboPrefixCustom->saveConfig();
    m_pageFilename->comboSuffixCustom->saveConfig();

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup groupGui = config->group(QString("GUISettings"));

    groupGui.writeEntry("Column0", this->previewColumnWidth(0));
    groupGui.writeEntry("Column1", this->previewColumnWidth(1));
}

void KRenameWindow::setupGui()
{
    m_buttons->clear();

    m_guiMode    = &tAdvancedMode;

    m_buttonFinish = new QPushButton(QIcon::fromTheme(QStringLiteral("dialog-ok")),    i18n("&Finish"));

    m_buttons->setStandardButtons(QDialogButtonBox::Help | QDialogButtonBox::Close);
    m_buttons->addButton(m_buttonFinish, QDialogButtonBox::AcceptRole);
    m_buttonClose = m_buttons->button(QDialogButtonBox::Close);
    m_buttonClose->setShortcut(QKeySequence(QKeySequence::Quit));

    KHelpMenu *helpMenu = new KHelpMenu(this, QString(), false);
    helpMenu->menu()->removeAction(helpMenu->action(KHelpMenu::menuHelpContents));
    m_buttons->button(QDialogButtonBox::Help)->setMenu(helpMenu->menu());

    setupSlots();
    slotEnableControls();
}

void KRenameWindow::setupPlugins()
{
    PluginLoader *loader = PluginLoader::Instance();

    const QList<Plugin *> &list = loader->plugins();
    QList<Plugin *>::const_iterator it = list.begin();

    m_pluginsWidgetHash.reserve(list.count());
    m_pluginsHash.reserve(list.count());

    m_pagePlugins->searchPlugins->searchLine()->setTreeWidget(m_pagePlugins->listPlugins);

    while (it != list.end()) {
        // create plugin gui
        QWidget *widget = new QWidget(m_pagePlugins->stackPlugins);
        (*it)->createUI(widget);
        int idx = m_pagePlugins->stackPlugins->addWidget(widget);
        m_pagePlugins->stackPlugins->setCurrentIndex(idx);

        m_pluginsHash[(*it)->name()] = (*it);
        m_pluginsWidgetHash[(*it)->name()] = widget;

        // add to list of all plugins
        QTreeWidgetItem *item = new QTreeWidgetItem(m_pagePlugins->listPlugins);
        item->setText(0, (*it)->name());
        item->setIcon(0, (*it)->icon());

        slotPluginChanged(item);

        ++it;
    }

    m_pagePlugins->splitter->setStretchFactor(0, 0);
    m_pagePlugins->splitter->setStretchFactor(1, 8);
    m_pagePlugins->listPlugins->sortColumn();
}

void KRenameWindow::setupIcons()
{
    QIcon upIcon = QIcon::fromTheme("arrow-up");
    QIcon downIcon = QIcon::fromTheme("arrow-down");

    // Page 1 icons

    QIcon openIcon = QIcon::fromTheme("document-open");
    QIcon removeIcon = QIcon::fromTheme("list-remove");

    m_pageFiles->buttonAdd->setIcon(openIcon);
    m_pageFiles->buttonRemove->setIcon(removeIcon);
    m_pageFiles->buttonUp->setIcon(upIcon);
    m_pageFiles->buttonDown->setIcon(downIcon);

    // Page 4 icons

    QIcon helpIcon = QIcon::fromTheme("help-hint");
    QIcon findIcon = QIcon::fromTheme("edit-find");

    m_pageFilename->buttonHelp1->setIcon(helpIcon);
    m_pageFilename->buttonHelp2->setIcon(helpIcon);
    m_pageFilename->buttonHelp3->setIcon(helpIcon);
    m_pageFilename->buttonHelp4->setIcon(helpIcon);
    m_pageFilename->buttonFunctions->setIcon(helpIcon);

    m_pageFilename->buttonFind->setIcon(findIcon);
    m_pageFilename->buttonFindSimple->setIcon(findIcon);

    m_pageFilename->buttonUp->setIcon(upIcon);
    m_pageFilename->buttonDown->setIcon(downIcon);

}

void KRenameWindow::setupSlots()
{
    connect(m_pageFiles->buttonAdd, &QPushButton::clicked,
            this, &KRenameWindow::addFiles);
    connect(m_pageFiles->buttonRemove, &QPushButton::clicked,
            this, &KRenameWindow::removeFiles);
    connect(m_pageFiles->buttonRemoveAll, &QPushButton::clicked,
            this, &KRenameWindow::removeAllFiles);
    connect(m_pageFiles->checkPreview, &QCheckBox::clicked,
            this, &KRenameWindow::slotPreviewChanged);
    connect(m_pageFiles->checkName, &QCheckBox::clicked,
            this, &KRenameWindow::slotPreviewChanged);
    connect(m_pageFiles->comboSort, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &KRenameWindow::slotSortChanged);
    connect(m_pageFiles->fileList, &KRenameListView::doubleClicked,
            this, &KRenameWindow::slotOpenFile);
    connect(m_pageFiles->buttonUp, &QPushButton::clicked,
            this, &KRenameWindow::slotMoveUp);
    connect(m_pageFiles->buttonDown, &QPushButton::clicked,
            this, &KRenameWindow::slotMoveDown);

    connect(m_pageDests->optionRename, &QRadioButton::clicked,
            this, &KRenameWindow::slotRenameModeChanged);
    connect(m_pageDests->optionCopy, &QRadioButton::clicked,
            this, &KRenameWindow::slotRenameModeChanged);
    connect(m_pageDests->optionMove, &QRadioButton::clicked,
            this, &KRenameWindow::slotRenameModeChanged);
    connect(m_pageDests->optionLink, &QRadioButton::clicked,
            this, &KRenameWindow::slotRenameModeChanged);
    connect(m_pageDests->checkOverwrite, &QCheckBox::clicked,
            this, &KRenameWindow::overwriteFilesChanged);

    connect(m_pagePlugins->listPlugins, &QTreeWidget::currentItemChanged,
            this, &KRenameWindow::slotPluginChanged);
    connect(m_pagePlugins->checkEnablePlugin, &QCheckBox::clicked,
            this, &KRenameWindow::slotPluginEnabled);

    connect(m_pageFilename->checkExtension, &QCheckBox::clicked,
            this, &KRenameWindow::slotEnableControls);
    connect(m_pageFilename->buttonNumbering, &QPushButton::clicked,
            this, &KRenameWindow::showAdvancedNumberingDialog);
    connect(m_pageFilename->buttonInsert, &QPushButton::clicked,
            this, &KRenameWindow::showInsertPartFilenameDialog);
    connect(m_pageFilename->buttonFind, &QPushButton::clicked,
            this, &KRenameWindow::showFindReplaceDialog);

    connect(m_pageFilename->filenameTemplate, &ExtHistoryCombo::delayedTextChanged,
            this, &KRenameWindow::slotTemplateChanged);
    connect(m_pageFilename->extensionTemplate, &ExtHistoryCombo::delayedTextChanged,
            this, &KRenameWindow::slotTemplateChanged);
    connect(m_pageFilename->checkExtension, &QCheckBox::clicked,
            this, &KRenameWindow::slotTemplateChanged);
    connect(m_pageFilename->buttonFunctions, &QPushButton::clicked,
            this, &KRenameWindow::slotTokenHelpRequested);
    connect(m_pageFilename->comboExtension, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &KRenameWindow::slotExtensionSplitModeChanged);

    connect(m_pageFilename->buttonUp, &QPushButton::clicked,
            this, &KRenameWindow::slotMoveUpPreview);
    connect(m_pageFilename->buttonDown, &QPushButton::clicked,
            this, &KRenameWindow::slotMoveDownPreview);

    connect(m_pageFilename->listPreview, &PreviewList::addFiles,
            this, &KRenameWindow::addFiles);
    connect(m_pageFilename->listPreview, &PreviewList::updateCount,
            this, &KRenameWindow::updateCount);

    connect(m_pageFilename->comboFilenameCustom, &ExtHistoryCombo::delayedTextChanged,
            this, &KRenameWindow::slotSimpleTemplateChanged);
    connect(m_pageFilename->comboSuffixCustom, &ExtHistoryCombo::delayedTextChanged,
            this, &KRenameWindow::slotSimpleTemplateChanged);
    connect(m_pageFilename->comboPrefixCustom, &ExtHistoryCombo::delayedTextChanged,
            this, &KRenameWindow::slotSimpleTemplateChanged);
    connect(m_pageFilename->comboExtensionCustom, &ExtHistoryCombo::delayedTextChanged,
            this, &KRenameWindow::slotSimpleTemplateChanged);

    connect(m_pageFilename->comboPrefix, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &KRenameWindow::slotSimpleTemplateChanged);
    connect(m_pageFilename->comboSuffix, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &KRenameWindow::slotSimpleTemplateChanged);
    connect(m_pageFilename->comboFilenameSimple, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &KRenameWindow::slotSimpleTemplateChanged);
    connect(m_pageFilename->comboExtensionSimple, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &KRenameWindow::slotSimpleTemplateChanged);

    connect(m_pageFilename->comboExtensionSimple, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &KRenameWindow::slotEnableControls);
    connect(m_pageFilename->comboFilenameSimple, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &KRenameWindow::slotEnableControls);

    connect(m_pageFilename->buttonHelp1, &QPushButton::clicked, this, &KRenameWindow::slotTokenHelpRequestedWizard1);
    connect(m_pageFilename->buttonHelp2, &QPushButton::clicked, this, &KRenameWindow::slotTokenHelpRequestedWizard2);
    connect(m_pageFilename->buttonHelp3, &QPushButton::clicked, this, &KRenameWindow::slotTokenHelpRequestedWizard3);
    connect(m_pageFilename->buttonHelp4, &QPushButton::clicked, this, &KRenameWindow::slotTokenHelpRequestedWizard4);
    connect(m_pageFilename->buttonFindSimple, &QPushButton::clicked,
            this, &KRenameWindow::showFindReplaceDialog);

    connect(m_pageFilename->spinDigits, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &KRenameWindow::slotSimpleTemplateChanged);
    connect(m_pageFilename->spinIndex, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &KRenameWindow::slotSimpleStartIndexChanged);
}

void KRenameWindow::showPage(int index)
{
    if (index >= 0 && index < m_guiMode->numPages) {
        m_curPage = index;
        m_stack->setCurrentIndex(m_guiMode->mapIndex[index]);

        slotEnableControls();
    }
}

void KRenameWindow::slotEnableControls()
{
    // TODO:
    // Disable all extension widgets
    // if splitMode is NOEXTENSION

    if (m_buttonFinish) {
        bool enable = (m_curPage == m_guiMode->numPages - 1)
                      && m_fileCount > 0;
        m_buttonFinish->setEnabled(enable);
    }

    // enable gui controls
    m_pageFiles->buttonRemove->setEnabled(m_fileCount);
    m_pageFiles->buttonRemoveAll->setEnabled(m_fileCount);
    m_pageFiles->checkName->setEnabled(m_pageFiles->checkPreview->isChecked());

    m_pageFiles->buttonUp->setEnabled(m_fileCount);
    m_pageFiles->buttonDown->setEnabled(m_fileCount);

    m_pageDests->urlrequester->setEnabled(!m_pageDests->optionRename->isChecked());

    m_pageFilename->extensionTemplate->setEnabled(!m_pageFilename->checkExtension->isChecked());
    m_pageFilename->buttonFind->setEnabled(m_fileCount != 0);
    m_pageFilename->buttonNumbering->setEnabled(m_fileCount != 0);
    m_pageFilename->buttonInsert->setEnabled(m_fileCount != 0);

    m_pageFilename->buttonFindSimple->setEnabled(m_fileCount != 0);
    m_pageFilename->buttonHelp3->setEnabled(m_pageFilename->comboFilenameSimple->currentIndex() ==
                                            m_pageFilename->comboFilenameSimple->count() - 1);
    m_pageFilename->comboFilenameCustom->setEnabled(m_pageFilename->comboFilenameSimple->currentIndex() ==
            m_pageFilename->comboFilenameSimple->count() - 1);

    m_pageFilename->buttonHelp4->setEnabled(m_pageFilename->comboExtensionSimple->currentIndex() ==
                                            m_pageFilename->comboExtensionSimple->count() - 1);
    m_pageFilename->comboExtensionCustom->setEnabled(m_pageFilename->comboExtensionSimple->currentIndex() ==
            m_pageFilename->comboExtensionSimple->count() - 1);

    m_pageFilename->buttonUp->setEnabled(m_fileCount);
    m_pageFilename->buttonDown->setEnabled(m_fileCount);
}

void KRenameWindow::setCount(unsigned int count)
{
    m_fileCount = count;
    m_pageFiles->labelCount->setText(i18n("<b>Files:</b> %1", m_fileCount));

    m_pageFiles->fileList->slotUpdateCount();
    this->slotEnableControls();
}

void KRenameWindow::setFilenameTemplate(const QString &templ, bool insert)
{
    if (insert) {
        m_pageFilename->filenameTemplate->lineEdit()->insert(templ);
    } else {
        m_pageFilename->filenameTemplate->lineEdit()->setText(templ);
    }
}

void KRenameWindow::setExtensionTemplate(const QString &templ, bool insert)
{
    m_pageFilename->checkExtension->setChecked(true);
    m_pageFilename->comboExtensionSimple->setCurrentIndex(
        m_pageFilename->comboExtensionSimple->count() - 1);

    if (insert) {
        m_pageFilename->extensionTemplate->lineEdit()->insert(templ);
    } else {
        m_pageFilename->extensionTemplate->lineEdit()->setText(templ);
    }
}

void KRenameWindow::setRenameMode(ERenameMode eMode)
{
    m_pageDests->optionRename->setChecked(false);
    m_pageDests->optionCopy->setChecked(false);
    m_pageDests->optionMove->setChecked(false);
    m_pageDests->optionLink->setChecked(false);

    switch (eMode) {
    case eRenameMode_Rename:
        m_pageDests->optionRename->setChecked(true);
        break;
    case eRenameMode_Copy:
        m_pageDests->optionCopy->setChecked(true);
        break;
    case eRenameMode_Move:
        m_pageDests->optionMove->setChecked(true);
        break;
    case eRenameMode_Link:
        m_pageDests->optionLink->setChecked(true);
        break;
    default:
        // Default
        m_pageDests->optionRename->setChecked(true);
        break;
    }

    this->slotEnableControls();
    Q_EMIT renameModeChanged(eMode);
}

void KRenameWindow::setDestinationUrl(const QUrl &url)
{
    m_pageDests->urlrequester->setUrl(url);
}

void KRenameWindow::resetFileList()
{
    m_pageFiles->fileList->reset();
}

void KRenameWindow::setModel(KRenameModel *model)
{
    m_pageFiles->fileList->setModel(model);

    m_pageFilename->listPreview->setKRenameModel(model);

    connect(model, &KRenameModel::maxDotsChanged,
            this, &KRenameWindow::slotMaxDotsChanged);
}

void KRenameWindow::setPreviewModel(KRenamePreviewModel *model)
{
    m_pageFilename->listPreview->setModel(model);
}

const QUrl KRenameWindow::destinationUrl() const
{
    return m_pageDests->urlrequester->url();
}

QList<int> KRenameWindow::selectedFileItems() const
{
    QList<int> selected;

    QItemSelectionModel *selection = m_pageFiles->fileList->selectionModel();
    QModelIndexList      indices = selection->selectedIndexes();
    QModelIndexList::const_iterator it = indices.constBegin();

    while (it != indices.constEnd()) {
        selected.append((*it).row());
        ++it;
    }

    return selected;
}

QList<int> KRenameWindow::selectedFileItemsPreview() const
{
    QList<int> selected;

    QItemSelectionModel *selection = m_pageFilename->listPreview->selectionModel();

    QModelIndexList      indices = selection->selectedIndexes();
    QModelIndexList::const_iterator it = indices.constBegin();

    while (it != indices.constEnd()) {
        selected.append((*it).row());
        ++it;
    }

    return selected;
}

bool KRenameWindow::isPreviewEnabled() const
{
    return m_pageFiles->checkPreview->isChecked();
}

void KRenameWindow::setPreviewEnabled(bool bPreview)
{
    m_pageFiles->checkPreview->setChecked(bPreview);

    slotPreviewChanged();
}

bool KRenameWindow::isPreviewNamesEnabled() const
{
    return m_pageFiles->checkName->isChecked();
}

void KRenameWindow::setPreviewNamesEnabled(bool bPreview)
{
    m_pageFiles->checkName->setChecked(bPreview);

    slotPreviewChanged();
}

int KRenameWindow::numberStartIndex() const
{
    return m_pageFilename->spinIndex->value();
}

void KRenameWindow::setNumberStartIndex(int index)
{
    m_pageFilename->spinIndex->setValue(index);
}

int KRenameWindow::sortMode() const
{
    return m_pageFiles->comboSort->currentIndex();
}

void KRenameWindow::setSortMode(int sortMode, const QString &customToken, int customSortMode)
{
    KRenameModel *model = static_cast<KRenameModel *>(m_pageFiles->fileList->model());
    bool bPrevious = m_pageFiles->comboSort->blockSignals(true);

    QString customTokenText;
    if (sortMode == eSortMode_Token) {
        customTokenText = customToken;
    }

    m_pageFiles->comboSort->setCurrentIndex(sortMode);
    m_pageFiles->labelCustomSort->setText(customTokenText);

    model->sortFiles(static_cast<ESortMode>(sortMode),
                     customToken,
                     static_cast<KRenameTokenSorter::ESimpleSortMode>(customSortMode));

    m_pageFiles->comboSort->blockSignals(bPrevious);
}

int KRenameWindow::previewColumnWidth(int index)
{
    return m_pageFilename->listPreview->columnWidth(index);
}

void KRenameWindow::setPreviewColumnWidth(int index, int width)
{
    m_pageFilename->listPreview->setColumnWidth(index, width);
}

bool KRenameWindow::isAdvancedMode() const
{
    return (m_pageFilename->tabWidget->currentIndex() == 0);
}

void KRenameWindow::setAdvancedMode(bool bAdvanced)
{
    m_pageFilename->tabWidget->setCurrentIndex(bAdvanced ? 0 : 1);
}

void KRenameWindow::showFilenameTab()
{
    m_tabBar->setCurrentIndex(MAX_PAGES - 1);
    if (this->isAdvancedMode()) {
        m_pageFilename->filenameTemplate->setFocus();
        m_pageFilename->filenameTemplate->selectAll();
    } else {
        m_pageFilename->comboPrefixCustom->setFocus();
        m_pageFilename->comboPrefixCustom->selectAll();
    }
}

void KRenameWindow::setPrefixSuffixSimple(QComboBox *combo, QComboBox *comboCustom, const QString &templ)
{
    if (templ.isEmpty()) {
        comboCustom->lineEdit()->setText(templ);
        combo->setCurrentIndex(0);
    } else {
        QString number('#');
        int c = m_pageFilename->spinDigits->value() - 1;

        while (c-- > 0) {
            number += '#';
        }

        number += QString("{%1}").arg(m_pageFilename->spinIndex->value());

        if (templ.startsWith(number)) {
            QString value = templ.right(templ.length() - number.length());
            combo->setCurrentIndex(1);
            comboCustom->lineEdit()->setText(value);
        } else if (templ.startsWith(QLatin1String("[date]"))) {
            QString value = templ.right(templ.length() - 6);
            combo->setCurrentIndex(2);
            comboCustom->lineEdit()->setText(value);
        } else {
            combo->setCurrentIndex(0);
            comboCustom->lineEdit()->setText(templ);
        }
    }
}

QString KRenameWindow::getPrefixSuffixSimple(QComboBox *combo, QComboBox *comboCustom)
{
    QString str;
    QString number('#');
    int c = m_pageFilename->spinDigits->value() - 1;

    while (c-- > 0) {
        number += '#';
    }

    number += QString("{%1}").arg(m_pageFilename->spinIndex->value());

    if (combo->currentIndex() == 1) {
        str = number;
    } else if (combo->currentIndex() == 2) {
        str = "[date]";    // TODO date
    }

    str += comboCustom->currentText();

    return str;
}

QString KRenameWindow::getFilenameSimple(QComboBox *combo, QComboBox *comboCustom)
{
    enum modification { OriginalName, LowerCase, UpperCase, Capitalize, Custom };
    QString str;

    switch (combo->currentIndex()) {
    default:
    case OriginalName:
        str = '$'; break;
    case LowerCase:
        str = '%'; break;
    case UpperCase:
        str = '&'; break;
    case Capitalize:
        str = '*'; break;
    case Custom:
        str = comboCustom->currentText();
    }

    return str;
}

void KRenameWindow::slotBack()
{
    this->showPage(m_curPage - 1);
}

void KRenameWindow::slotNext()
{
    this->showPage(m_curPage + 1);
}

void KRenameWindow::slotFinish()
{
    Q_EMIT accepted();
}

void KRenameWindow::slotRenameModeChanged()
{
    ERenameMode mode = eRenameMode_Rename;

    if (m_pageDests->optionRename->isChecked()) {
        mode = eRenameMode_Rename;
    } else if (m_pageDests->optionCopy->isChecked()) {
        mode = eRenameMode_Copy;
    } else if (m_pageDests->optionMove->isChecked()) {
        mode = eRenameMode_Move;
    } else if (m_pageDests->optionLink->isChecked()) {
        mode = eRenameMode_Link;
    }

    Q_EMIT renameModeChanged(mode);

    this->slotEnableControls();
}

void KRenameWindow::slotGotoTemplatesPage()
{
    m_tabBar->setCurrentIndex(3);
}

void KRenameWindow::slotSimpleStartIndexChanged()
{
    Q_EMIT startIndexChanged(m_pageFilename->spinIndex->value());

    slotSimpleTemplateChanged();
}

void KRenameWindow::slotTemplateChanged()
{
    QString filename;
    QString extension;

    filename  = m_pageFilename->filenameTemplate->currentText();
    extension = m_pageFilename->checkExtension->isChecked() ? "$" :
                m_pageFilename->extensionTemplate->currentText();

    // set the new templates also for simple mode
    blockSignalsRecursive(this, true);
    this->setSimpleTemplate(filename, extension);
    blockSignalsRecursive(this, false);

    this->templatesChanged(filename, extension);
}

void KRenameWindow::slotSimpleTemplateChanged()
{
    QString filename  = getFilenameSimple(m_pageFilename->comboFilenameSimple, m_pageFilename->comboFilenameCustom);
    QString extension = getFilenameSimple(m_pageFilename->comboExtensionSimple, m_pageFilename->comboExtensionCustom);
    QString prefix    = getPrefixSuffixSimple(m_pageFilename->comboPrefix, m_pageFilename->comboPrefixCustom);
    QString suffix    = getPrefixSuffixSimple(m_pageFilename->comboSuffix, m_pageFilename->comboSuffixCustom);

    filename = prefix + filename + suffix;

    // set the new templates, but make sure signals
    // are blockes so that slotTemplateChanged emits updatePreview()
    // which is calculation intensive only once!
    blockSignalsRecursive(this, true);
    m_pageFilename->filenameTemplate->lineEdit()->setText(filename);
    m_pageFilename->extensionTemplate->lineEdit()->setText(extension);
    m_pageFilename->checkExtension->setChecked(false);
    blockSignalsRecursive(this, false);

    this->templatesChanged(filename, extension);
}

void KRenameWindow::templatesChanged(const QString &filename, const QString &extension)
{
    Q_EMIT filenameTemplateChanged(filename);
    Q_EMIT extensionTemplateChanged(extension);

    Q_EMIT updatePreview();

    m_pageFilename->buttonNumbering->setEnabled(filename.contains('#') || extension.contains('#'));
    this->slotEnableControls();
}

void KRenameWindow::setSimpleTemplate(const QString &filename, const QString &extension)
{
    // First set the simple extension from a template string
    if (extension == "$") {
        m_pageFilename->comboExtensionSimple->setCurrentIndex(0);
    } else if (extension == "%") {
        m_pageFilename->comboExtensionSimple->setCurrentIndex(1);
    } else if (extension == "&") {
        m_pageFilename->comboExtensionSimple->setCurrentIndex(2);
    } else if (extension == "*") {
        m_pageFilename->comboExtensionSimple->setCurrentIndex(3);
    } else {
        m_pageFilename->comboExtensionSimple->setCurrentIndex(4);
        m_pageFilename->comboExtensionCustom->lineEdit()->setText(extension);
    }

    // Now split the filename in prefix and suffix and set it as template
    // TODO: Make sure we do not find something like [*5-] or \$
    int index = 4;
    int pos   = filename.indexOf("$");
    if (pos == -1) {
        pos = filename.indexOf("%");
        if (pos == -1) {
            pos = filename.indexOf("&");
            if (pos == -1) {
                pos = filename.indexOf("*");
                if (pos != -1) {
                    index = 3;
                }
            } else {
                index = 2;
            }
        } else {
            index = 1;
        }
    } else {
        index = 0;
    }

    m_pageFilename->comboFilenameSimple->setCurrentIndex(index);
    if (pos == -1) {
        // No token found, so we have no prefix or suffix but
        // a custom name.
        m_pageFilename->comboPrefixCustom->lineEdit()->setText(QString());
        m_pageFilename->comboPrefix->setCurrentIndex(0);

        m_pageFilename->comboSuffixCustom->lineEdit()->setText(QString());
        m_pageFilename->comboSuffix->setCurrentIndex(0);

        m_pageFilename->comboFilenameCustom->lineEdit()->setText(filename);
    } else {
        QString prefix = (pos > 0 ? filename.left(pos) : QString());
        QString suffix = (pos < filename.length() ? filename.right(filename.length() - pos - 1) : QString());

        setPrefixSuffixSimple(m_pageFilename->comboPrefix, m_pageFilename->comboPrefixCustom, prefix);
        setPrefixSuffixSimple(m_pageFilename->comboSuffix, m_pageFilename->comboSuffixCustom, suffix);
    }
}

void KRenameWindow::slotTokenHelpRequested()
{
    Q_EMIT showTokenHelpDialog(m_pageFilename->filenameTemplate->lineEdit());
}

void KRenameWindow::slotTokenHelpRequestedWizard1()
{
    Q_EMIT showTokenHelpDialog(m_pageFilename->comboPrefixCustom->lineEdit());
}

void KRenameWindow::slotTokenHelpRequestedWizard2()
{
    Q_EMIT showTokenHelpDialog(m_pageFilename->comboSuffixCustom->lineEdit());
}

void KRenameWindow::slotTokenHelpRequestedWizard3()
{
    Q_EMIT showTokenHelpDialog(m_pageFilename->comboFilenameCustom->lineEdit());
}

void KRenameWindow::slotTokenHelpRequestedWizard4()
{
    Q_EMIT showTokenHelpDialog(m_pageFilename->comboExtensionCustom->lineEdit());
}

void KRenameWindow::setExtensionSplitMode(ESplitMode splitMode, int dot)
{
    int index;
    switch (splitMode) {
    default:
    case eSplitMode_FirstDot:
        index = 0;
        break;
    case eSplitMode_LastDot:
        index = 1;
        break;
    case eSplitMode_NoExtension:
        index = 2;
        break;
    case eSplitMode_CustomDot:
        index = dot + 1;
        break;
    }

    m_pageFilename->comboExtension->setCurrentIndex(index);
}

void KRenameWindow::slotExtensionSplitModeChanged(int index)
{
    ESplitMode splitMode;
    switch (index) {
    case 0:
        splitMode = eSplitMode_FirstDot;
        break;
    case 1:
        splitMode = eSplitMode_LastDot;
        break;
    case 2:
        splitMode = eSplitMode_NoExtension;
        break;
    default:
        splitMode = eSplitMode_CustomDot;
        break;
    }

    Q_EMIT extensionSplitModeChanged(splitMode, index - 1);
}

void KRenameWindow::slotPreviewChanged()
{
    KRenameModel *model = static_cast<KRenameModel *>(m_pageFiles->fileList->model());

    if (m_pageFiles->checkPreview->isChecked() && !m_pageFiles->checkName->isChecked()) {
        m_pageFiles->fileList->setViewMode(QListView::IconMode);
    } else {
        m_pageFiles->fileList->setViewMode(QListView::ListMode);
    }

    model->setEnablePreview(m_pageFiles->checkPreview->isChecked(), m_pageFiles->checkName->isChecked());
    Q_EMIT filePreviewChanged(m_pageFiles->checkPreview->isChecked(), m_pageFiles->checkName->isChecked());

    m_pageFiles->fileList->setAcceptDrops(true);
    m_pageFiles->fileList->repaint();
    this->slotEnableControls();
}

void KRenameWindow::slotSortChanged(int index)
{
    ESortMode eMode;
    KRenameModel *model = static_cast<KRenameModel *>(m_pageFiles->fileList->model());
    KRenameTokenSorter::ESimpleSortMode eCustomSortMode = model->getSortModeCustomMode();
    QString customToken;

    switch (index) {
    default:
    case 0:
        eMode = eSortMode_Unsorted;  break;
    case 1:
        eMode = eSortMode_Ascending; break;
    case 2:
        eMode = eSortMode_Descending; break;
    case 3:
        eMode = eSortMode_Numeric; break;
    case 4:
        eMode = eSortMode_Random; break;
    case 5:
        eMode = eSortMode_AscendingDate; break;
    case 6:
        eMode = eSortMode_DescendingDate; break;
    case 7: {
        eMode = eSortMode_Token;
        QPointer<TokenSortModeDialog> dlg = new TokenSortModeDialog(eCustomSortMode, this);
        if (dlg->exec() == QDialog::Accepted) {
            customToken = dlg->getToken();
            eCustomSortMode = dlg->getSortMode();
        } else {
            eMode = model->getSortMode();
            // Do not change anything
            // Reset combo box
            m_pageFiles->comboSort->setCurrentIndex(eMode);
            return;
        }
        delete dlg;
        break;
    }
    }

    m_pageFiles->labelCustomSort->setText(customToken);
    model->sortFiles(eMode, customToken, eCustomSortMode);
}

void KRenameWindow::slotMaxDotsChanged(int dots)
{
    int i;
    const int FILE_EXTENSION_VARIABLE_ITEMS_START = 3;

    for (i = FILE_EXTENSION_VARIABLE_ITEMS_START;
            i < m_pageFilename->comboExtension->count();
            i++) {
        m_pageFilename->comboExtension->removeItem(i);
    }

    for (i = 1; i <= dots; ++i) {
        m_pageFilename->comboExtension->addItem(QString::number(i));
    }
}

void KRenameWindow::slotOpenFile(const QModelIndex &index)
{
    KRenameModel *model = static_cast<KRenameModel *>(m_pageFiles->fileList->model());
    model->run(index, this);
}

void KRenameWindow::slotMoveUp()
{
    QList<int> sel = this->selectedFileItems();

    KRenameModel *model = static_cast<KRenameModel *>(m_pageFiles->fileList->model());
    model->moveFilesUp(sel);

    QItemSelectionModel *selection = m_pageFiles->fileList->selectionModel();
    QList<int>::const_iterator it  = sel.constBegin();
    while (it != sel.constEnd()) {
        if (*it - 1 > 0) {
            selection->select(model->createIndex(*it - 1), QItemSelectionModel::Select);
        }

        ++it;
    }

    // make sure that the first item is visible
    // TODO: Maybe it is better to calculate the minimum index here
    if (sel.size()) {
        m_pageFiles->fileList->scrollTo(model->createIndex(sel.front() - 1), QAbstractItemView::EnsureVisible);
    }
}

void KRenameWindow::slotMoveDown()
{
    QList<int> sel = this->selectedFileItems();

    KRenameModel *model = static_cast<KRenameModel *>(m_pageFiles->fileList->model());
    model->moveFilesDown(sel);

    QItemSelectionModel *selection = m_pageFiles->fileList->selectionModel();
    QList<int>::const_iterator it  = sel.constBegin();
    while (it != sel.constEnd()) {
        if (*it + 1 < model->rowCount()) {
            selection->select(model->createIndex(*it + 1), QItemSelectionModel::Select);
        }

        ++it;
    }

    // make sure that the last item is visible
    // TODO: Maybe it is better to calculate the maximum index here
    if (sel.size()) {
        m_pageFiles->fileList->scrollTo(model->createIndex(sel.back() + 1), QAbstractItemView::EnsureVisible);
    }
}

void KRenameWindow::slotMoveUpPreview()
{
    QList<int>         sel  = this->selectedFileItemsPreview();
    QAbstractItemView *view = m_pageFilename->listPreview;

    moveUp(sel, view);
}

void KRenameWindow::slotMoveDownPreview()
{
    QList<int>         sel  = this->selectedFileItemsPreview();
    QAbstractItemView *view = m_pageFilename->listPreview;

    moveDown(sel, view);
}

void KRenameWindow::moveUp(const QList<int> &selected, QAbstractItemView *view)
{
    KRenameModel *model = static_cast<KRenameModel *>(m_pageFiles->fileList->model());
    model->moveFilesUp(selected);

    Q_EMIT updatePreview();

    QItemSelectionModel *selection = view->selectionModel();
    QList<int>::const_iterator it  = selected.begin();
    while (it != selected.end()) {
        if (*it - 1 > 0) {
            selection->select(model->createIndex(*it - 1), QItemSelectionModel::Select);
        }

        ++it;
    }

    // make sure that the first item is visible
    // TODO: Maybe it is better to calculate the minimum index here
    if (selected.size()) {
        view->scrollTo(model->createIndex(selected.front() - 1), QAbstractItemView::EnsureVisible);
    }
}

void KRenameWindow::moveDown(const QList<int> &selected, QAbstractItemView *view)
{
    KRenameModel *model = static_cast<KRenameModel *>(m_pageFiles->fileList->model());
    model->moveFilesDown(selected);

    Q_EMIT updatePreview();

    QItemSelectionModel *selection = view->selectionModel();
    QList<int>::const_iterator it  = selected.begin();
    while (it != selected.end()) {
        if (*it + 1 < model->rowCount()) {
            selection->select(model->createIndex(*it + 1), QItemSelectionModel::Select);
        }

        ++it;
    }

    // make sure that the last item is visible
    // TODO: Maybe it is better to calculate the maximum index here
    if (selected.size()) {
        view->scrollTo(model->createIndex(selected.back() + 1), QAbstractItemView::EnsureVisible);
    }
}

void KRenameWindow::slotPluginChanged(QTreeWidgetItem *selected)
{
    QWidget *w = m_pluginsWidgetHash[selected->text(0)];
    Plugin  *p = m_pluginsHash[selected->text(0)];

    if (p->enabledByDefault()) {
        m_pagePlugins->checkEnablePlugin->setChecked(true);
    } else {
        m_pagePlugins->checkEnablePlugin->setChecked(p->isEnabled());
    }

    //m_pagePlugins->checkEnablePlugin->setEnabled(!p->enabledByDefault());
    m_pagePlugins->stackPlugins->setCurrentWidget(w);

    slotPluginEnabled();
}

void KRenameWindow::slotPluginEnabled()
{
    QTreeWidgetItem *selected = m_pagePlugins->listPlugins->currentItem();
    if (selected) {
        QWidget *w = m_pluginsWidgetHash[selected->text(0)];
        Plugin  *p = m_pluginsHash[selected->text(0)];

        p->setEnabled(m_pagePlugins->checkEnablePlugin->isChecked());
        w->setEnabled(p->enabledByDefault() || m_pagePlugins->checkEnablePlugin->isChecked());
    }
}

void KRenameWindow::blockSignalsRecursive(QObject *obj, bool b)
{
    if (obj) {
        obj->blockSignals(b);

        QList<QObject *> list = obj->children();
        QList<QObject *>::iterator it = list.begin();
        QObject *o = nullptr;

        while (it != list.end()) {
            o = *it;
            if (o && o != obj) {
                blockSignalsRecursive(o, b);
            }

            ++it;
        }
    }
}

#include "moc_krenamewindow.cpp"
