// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Dominik Seichter <domseichter@web.de>

#include "tokenhelpdialog.h"

#include "batchrenamer.h"
#include "krenamemodel.h"

#include <KSharedConfig>
#include <KTreeWidgetSearchLine>
#include <QPushButton>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLineEdit>

#define COLUMN_PREVIEW 2

/**
 * A wrapper class for KRenameModel that
 * replaces display role with customer Qt::UserRole
 * so that the KRenameModel will always return unformatted
 * data, because QComboBox cannot display richtext.
 */
class KRenameUnformattedWrappedModel : public QAbstractListModel
{
    Q_OBJECT
public:
    KRenameUnformattedWrappedModel(KRenameModel *model)
        : m_model(model)
    {
    }

    /** Get the file at position index.
     *
     *  @param a valid index in the internal vector
     *
     *  @returns a KRenameFile object
     */
    const KRenameFile &file(int index) const
    {
        return m_model->file(index);
    }

    /** Get the file at position index.
     *
     *  @param a valid index in the internal vector
     *
     *  @returns a KRenameFile object
     */
    KRenameFile &file(int index)
    {
        return m_model->file(index);
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (role == Qt::DisplayRole) {
            return static_cast<KRenameModel *>(m_model)->data(index, Qt::UserRole);
        } else {
            return static_cast<KRenameModel *>(m_model)->data(index, role);
        }
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return m_model->rowCount(parent);
    }

private:
    KRenameModel *m_model;
};

const int TokenHelpDialog::S_MAX_RECENT = 10;

TokenHelpDialog::TokenHelpDialog(KRenameModel *model, BatchRenamer *renamer,
                                 QLineEdit *edit, QWidget *parent)
    : QDialog(parent), m_edit(edit), m_renamer(renamer)
{
    m_model = new KRenameUnformattedWrappedModel(model);

    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);
    m_widget.setupUi(mainWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    QPushButton *insert = new QPushButton;
    buttonBox->addButton(insert, QDialogButtonBox::ActionRole);
    this->connect(buttonBox, &QDialogButtonBox::accepted,
                  this, &TokenHelpDialog::accept);
    this->connect(buttonBox, &QDialogButtonBox::rejected,
                  this, &TokenHelpDialog::reject);
    mainLayout->addWidget(buttonBox);
    insert->setText(i18n("&Insert"));

    QPushButton *close = buttonBox->button(QDialogButtonBox::Close);

    m_widget.searchCategory->searchLine()->setTreeWidget(m_widget.listCategories);
    m_widget.searchToken   ->searchLine()->setTreeWidget(m_widget.listTokens);
    m_widget.comboPreview->setModel(m_model);
    m_widget.listTokens->sortItems(0, Qt::AscendingOrder);

    connect(insert, &QPushButton::clicked,
            this, &TokenHelpDialog::slotInsert);
    connect(close, &QPushButton::clicked,
            this, &TokenHelpDialog::saveConfig);

    connect(m_widget.listCategories, &QTreeWidget::itemClicked,
            this, &TokenHelpDialog::slotCategoryChanged);
    connect(m_widget.listTokens, &QTreeWidget::itemClicked,
            this, &TokenHelpDialog::slotInsert);
    connect(m_widget.checkPreview, &QCheckBox::clicked,
            this, &TokenHelpDialog::slotPreviewClicked);
    connect(m_widget.comboPreview, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
            this, &TokenHelpDialog::slotUpdatePreview);

    slotEnableControls();
    loadConfig();
}

TokenHelpDialog::~TokenHelpDialog()
{
    delete m_model;
}

const QString TokenHelpDialog::getTokenSeparator()
{
    return QString::fromLatin1(";;");
}

void TokenHelpDialog::add(const QString &headline, const QStringList &commands, const QIcon &icon, bool first)
{
    m_map[headline] = commands;

    QTreeWidgetItem *item = new QTreeWidgetItem(m_widget.listCategories);
    item->setText(0, headline);
    item->setIcon(0, icon);

    if (first) {
        m_first = headline;
    }
}

int TokenHelpDialog::exec()
{
    addRecentTokens();

    m_widget.listCategories->sortItems(0, Qt::AscendingOrder);

    if (!m_lastSelected.isEmpty()) {
        selectCategory(m_lastSelected);
    } else if (!m_first.isEmpty()) {
        selectCategory(m_first);
    }

    return QDialog::exec();
}

void TokenHelpDialog::selectCategory(const QString &category)
{
    for (int i = 0; i < m_widget.listCategories->topLevelItemCount(); i++)
        if (m_widget.listCategories->topLevelItem(i)->text(0) == category) {
            m_widget.listCategories->topLevelItem(i)->setSelected(true);
            this->slotCategoryChanged(m_widget.listCategories->topLevelItem(i));
            break;
        }
}

void TokenHelpDialog::slotCategoryChanged(QTreeWidgetItem *item)
{
    m_widget.listTokens->clear();

    const QStringList &commands = m_map[item->text(0)];
    for (int i = 0; i < commands.count(); i++) {
        QTreeWidgetItem *item = new QTreeWidgetItem(m_widget.listTokens);
        item->setText(0, commands[i].section(getTokenSeparator(), 0, 0));
        item->setText(1, commands[i].section(getTokenSeparator(), 1, 1));
    }

    slotUpdatePreview();
}

void TokenHelpDialog::slotInsert()
{
    QTreeWidgetItem *category = m_widget.listCategories->currentItem();
    if (category) {
        m_lastSelected = category->text(0);
    }

    QTreeWidgetItem *item = m_widget.listTokens->currentItem();
    if (item) {
        const QString &token = item->text(0);
        const QString &help = item->text(1);

        addToRecentTokens(token, help);
        saveConfig();

        m_edit->insert(token);
    }

    this->accept();
}

void TokenHelpDialog::loadConfig()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();

    KConfigGroup groupGui = config->group(QString("TokenHelpDialog"));

    m_lastSelected = groupGui.readEntry("LastSelectedCategory", m_lastSelected);
    m_recent = groupGui.readEntry("RecentTokens", m_recent);

    bool preview = groupGui.readEntry("Preview",  m_widget.checkPreview->isChecked());
    m_widget.checkPreview->setChecked(preview);

    int width = groupGui.readEntry("Column0", QVariant(m_widget.listTokens->columnWidth(0))).toInt();
    if (width > 0) {
        m_widget.listTokens->setColumnWidth(0, width);
    }

    width = groupGui.readEntry("Column1", QVariant(m_widget.listTokens->columnWidth(1))).toInt();
    if (width > 0) {
        m_widget.listTokens->setColumnWidth(1, width);
    }

    width = groupGui.readEntry("Column2", QVariant(m_widget.listTokens->columnWidth(2))).toInt();
    if (width > 0) {
        m_widget.listTokens->setColumnWidth(2, width);
    }

    restoreGeometry(groupGui.readEntry<QByteArray>("Geometry", QByteArray()));

    QList<int> sizes = groupGui.readEntry("Splitter", QList<int>());
    if (sizes.size() == 2) {
        m_widget.splitter->setSizes(sizes);
    }
}

void TokenHelpDialog::saveConfig()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();

    KConfigGroup groupGui = config->group(QString("TokenHelpDialog"));

    groupGui.writeEntry("Column0",  m_widget.listTokens->columnWidth(0));
    groupGui.writeEntry("Column1",  m_widget.listTokens->columnWidth(1));
    groupGui.writeEntry("Column2",  m_widget.listTokens->columnWidth(2));
    groupGui.writeEntry("Splitter", m_widget.splitter->sizes());
    groupGui.writeEntry("Preview",  m_widget.checkPreview->isChecked());
    groupGui.writeEntry("LastSelectedCategory", m_lastSelected);
    groupGui.writeEntry("RecentTokens", m_recent);

    groupGui.writeEntry("Geometry", saveGeometry());
}

void TokenHelpDialog::slotEnableControls()
{
    m_widget.comboPreview->setEnabled(m_widget.checkPreview->isChecked());
    m_widget.labelPreview->setEnabled(m_widget.checkPreview->isChecked());
}

void TokenHelpDialog::slotPreviewClicked(bool bPreview)
{
    slotEnableControls();

    if (bPreview) {
        m_widget.listTokens->setColumnHidden(COLUMN_PREVIEW, false);
        slotUpdatePreview();

        // make sure preview column is visible
        m_widget.listTokens->resizeColumnToContents(0);
        m_widget.listTokens->resizeColumnToContents(1);
    } else {
        m_widget.listTokens->setColumnHidden(COLUMN_PREVIEW, true);
    }
}

void TokenHelpDialog::slotUpdatePreview()
{
    if (!m_widget.checkPreview->isChecked()) {
        return;
    }

    int index = m_widget.comboPreview->currentIndex();
    if (index >= 0 && m_widget.listCategories->currentItem() != nullptr) {
        const KRenameFile &file = m_model->file(index);
        QApplication::setOverrideCursor(Qt::WaitCursor);

        QString token;
        for (int i = 0; i < m_widget.listTokens->topLevelItemCount(); i++) {
            QTreeWidgetItem *item = m_widget.listTokens->topLevelItem(i);
            if (item) {
                token = m_renamer->processString(item->text(0), file.srcFilename(), index);
                item->setText(COLUMN_PREVIEW, token);
            }
        }
        QApplication::restoreOverrideCursor();
    }
}

void TokenHelpDialog::addRecentTokens()
{
    this->add(i18n("Recent"), m_recent, QIcon::fromTheme("document-open-recent"));
}

void TokenHelpDialog::addToRecentTokens(const QString &token, const QString &help)
{
    // 1. Check if m_recent contains token and remove it
    QStringList::iterator it = m_recent.begin();
    while (it != m_recent.end()) {
        if ((*it).startsWith(token + getTokenSeparator())) {
            m_recent.erase(it);
            break;
        }

        ++it;
    }

    // 2. remove first token if more than 10
    if (m_recent.count() >= S_MAX_RECENT) {
        m_recent.removeAt(0);
    }

    // 3. append token
    QString recent = token + getTokenSeparator() + help;
    m_recent << recent;
}

#include "tokenhelpdialog.moc"

#include "moc_tokenhelpdialog.cpp"
