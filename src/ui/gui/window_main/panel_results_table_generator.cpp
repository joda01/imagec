///
/// \file      dialog_results_template_generator.cpp
/// \author    Joachim Danmayr
/// \date      2025-03-04
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "panel_results_table_generator.hpp"
#include <qboxlayout.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <qtoolbar.h>
#include <qwidget.h>
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/database/plugins/filter.hpp"
#include "backend/settings/results_settings/results_template.hpp"
#include "ui/gui/helper/table_widget.hpp"
#include "ui/gui/results/dialog_column_settings.hpp"
#include "ui/gui/results/dialog_column_template.hpp"
#include "ui/gui/window_main/window_main.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
PanelResultsTableGenerator::PanelResultsTableGenerator(WindowMain *mainWindow, joda::settings::AnalyzeSettings *analyzeSettings) :
    QWidget(mainWindow), mMainWindow(mainWindow), mAnalyzeSettings(analyzeSettings)
{
  auto *mainLayout = new QVBoxLayout();
  mainLayout->setContentsMargins(0, 0, 0, 0);
  setContentsMargins(0, 0, 0, 0);
  this->setLayout(mainLayout);

  //
  // Action button
  //
  auto *toolBar = new QToolBar();
  toolBar->setContentsMargins(0, 0, 0, 0);

  //
  //
  //
  mTemplateMenu           = new QMenu();
  auto *templateGenerator = new QAction(generateSvgIcon("document-new"), "", this);
  templateGenerator->setStatusTip("Open table column generation wizard");
  connect(templateGenerator, &QAction::triggered, [this]() {
    if(mColumnTemplate->exec() == 0) {
      mAnalyzeSettings->resultsSettings = mAnalyzeSettings->resultsTemplate.toSettings(*mAnalyzeSettings);
      refreshView();
    }
    loadTemplates();
    mMainWindow->checkForSettingsChanged();
  });
  templateGenerator->setMenu(mTemplateMenu);
  toolBar->addAction(templateGenerator);

  //
  // Open template
  //
  auto *openTemplate = new QAction(generateSvgIcon("folder-stash"), "Open template");
  openTemplate->setStatusTip("Open table template from file");
  connect(openTemplate, &QAction::triggered, [this]() {
    QString folderToOpen           = joda::templates::TemplateParser::getUsersTemplateDirectory().string().data();
    QString filePathOfSettingsFile = QFileDialog::getOpenFileName(
        this, "Open template", folderToOpen, "ImageC template files (*" + QString(joda::fs::EXT_RESULTS_TABLE_TEMPLATE.data()) + ")");
    if(filePathOfSettingsFile.isEmpty()) {
      return;
    }
    this->openTemplate(filePathOfSettingsFile);
  });
  toolBar->addAction(openTemplate);

  //
  //
  //
  auto *autoGenerateTable = new QAction(generateSvgIcon("quickopen-file"), "", this);
  autoGenerateTable->setStatusTip("Auto generate table column settings from template. If no template is set, a template is generated.");
  connect(autoGenerateTable, &QAction::triggered, [this]() {
    if(askForChangeTemplate()) {
      if(mAnalyzeSettings->resultsTemplate.columns.empty()) {
        mAnalyzeSettings->resultsTemplate.columns = {
            {.measureChannels = {enums::Measurement::COUNT, enums::Measurement::INTERSECTING}, .stats = {enums::Stats::SUM, enums::Stats::AVG}},
            {.measureChannels = {enums::Measurement::AREA_SIZE}, .stats = {enums::Stats::SUM, enums::Stats::AVG}},
            {.measureChannels = {enums::Measurement::INTENSITY_AVG, enums::Measurement::INTENSITY_SUM},
             .stats           = {enums::Stats::SUM, enums::Stats::AVG}}};
      }
      mAnalyzeSettings->resultsSettings = mAnalyzeSettings->resultsTemplate.toSettings(*mAnalyzeSettings);
      refreshView();
      mMainWindow->checkForSettingsChanged();
    }
  });
  toolBar->addAction(autoGenerateTable);

  toolBar->addSeparator();

  //
  //
  //
  auto *addColumn = new QAction(generateSvgIcon("edit-table-insert-column-right"), "", this);
  addColumn->setToolTip("Add column");
  connect(addColumn, &QAction::triggered, [this]() {
    mColumnEditDialog->updateClassesAndClasses(*mAnalyzeSettings);
    mColumnEditDialog->exec(mColumns->rowCount());
    if(mAutoSort->isChecked()) {
      mAnalyzeSettings->resultsSettings.sortColumns();
    }
    refreshView();
    mMainWindow->checkForSettingsChanged();
  });
  toolBar->addAction(addColumn);

  auto *deleteColumn = new QAction(generateSvgIcon("edit-table-delete-column"), "", this);
  deleteColumn->setToolTip("Delete column");
  toolBar->addAction(deleteColumn);
  connect(deleteColumn, &QAction::triggered, [this]() {
    if(mSelectedTableRow >= 0) {
      mAnalyzeSettings->resultsSettings.eraseColumn({.tabIdx = 0, .colIdx = mSelectedTableRow});
      if(mAutoSort->isChecked()) {
        mAnalyzeSettings->resultsSettings.sortColumns();
      }
      refreshView();
      mMainWindow->checkForSettingsChanged();
    }
  });

  auto *editColumn = new QAction(generateSvgIcon("document-edit"), "", this);
  editColumn->setToolTip("Edit column");
  toolBar->addAction(editColumn);
  connect(editColumn, &QAction::triggered, [this]() {
    if(mSelectedTableRow >= 0) {
      mColumnEditDialog->updateClassesAndClasses(*mAnalyzeSettings);
      mColumnEditDialog->exec(mSelectedTableRow);
      if(mAutoSort->isChecked()) {
        mAnalyzeSettings->resultsSettings.sortColumns();
      }
      refreshView();
      mMainWindow->checkForSettingsChanged();
    }
  });

  toolBar->addSeparator();

  mAutoSort = new QAction(generateSvgIcon("view-sort-ascending-name"), "", this);
  mAutoSort->setCheckable(true);
  mAutoSort->setChecked(true);
  mAutoSort->setStatusTip("Sort columns");
  toolBar->addAction(mAutoSort);
  connect(mAutoSort, &QAction::triggered, [this]() {
    if(mAutoSort->isChecked()) {
      mAnalyzeSettings->resultsSettings.sortColumns();
      refreshView();
      mMainWindow->checkForSettingsChanged();
    }
  });

  //
  // Table
  //
  mColumns = new PlaceholderTableWidget();
  mColumns->setColumnCount(1);
  mColumns->setPlaceholderText("Add a column");
  mColumns->setHorizontalHeaderLabels({"Results column template"});
  mColumns->setAlternatingRowColors(true);
  mColumns->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  mColumns->verticalHeader()->setVisible(true);
  mColumns->horizontalHeader()->setVisible(false);
  mColumns->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
  mColumns->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
  mColumns->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  connect(mColumns, &PlaceholderTableWidget::currentCellChanged,
          [this](int currentRow, int currentColumn, int previousRow, int previousColumn) { mSelectedTableRow = currentRow; });

  connect(mColumns, &PlaceholderTableWidget::cellDoubleClicked, [this](int row, int column) {
    mSelectedTableRow = row;
    if(mSelectedTableRow >= 0) {
      mColumnEditDialog->updateClassesAndClasses(*mAnalyzeSettings);
      mColumnEditDialog->exec(mSelectedTableRow);
      refreshView();
      mMainWindow->checkForSettingsChanged();
    }
  });

  mainLayout->addWidget(toolBar);
  mainLayout->addWidget(mColumns);

  //
  // Add command dialog
  //
  mColumnEditDialog = new DialogColumnSettings(&mAnalyzeSettings->resultsSettings, this);
  mColumnTemplate   = new DialogColumnTemplate(&mAnalyzeSettings->resultsTemplate, this);

  loadTemplates();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
// int32_t PanelResultsTableGenerator::exec()
//{
//   refreshView();
//   mColumnEditDialog->updateClassesAndClasses(*mAnalyzeSettings);
//   return QDialog::exec();
// }

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResultsTableGenerator::refreshView()
{
  auto getNameForClass = [this](const enums::ClassId &classId) -> std::string {
    for(const auto &classs : mAnalyzeSettings->projectSettings.classification.classes) {
      if(classs.classId == classId) {
        return classs.name;
      }
    }
    return "";
  };

  auto &columns = mAnalyzeSettings->resultsSettings.mutableColumns();
  mColumns->setRowCount(columns.size());
  std::set<int32_t> toRemove;
  for(auto &[index, key] : columns) {
    auto *item          = mColumns->item(index.colIdx, 0);
    std::string nameTmp = getNameForClass(key.classId);
    if(nameTmp.empty()) {
      toRemove.emplace(index.colIdx);
    }
    key.names.className        = nameTmp;
    key.names.intersectingName = getNameForClass(key.intersectingChannel);

    if(item == nullptr) {
      item = new QTableWidgetItem();
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      item->setText(key.createHeader().data());
      mColumns->setItem(index.colIdx, 0, item);
    } else {
      item->setText(key.createHeader().data());
    }
  }

  // Remove not existing classes
  if(!toRemove.empty()) {
    for(const auto idx : toRemove) {
      mAnalyzeSettings->resultsSettings.eraseColumn({.tabIdx = 0, .colIdx = idx});
    }
    refreshView();
    mMainWindow->checkForSettingsChanged();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
bool PanelResultsTableGenerator::askForChangeTemplate()
{
  QMessageBox messageBox(mMainWindow);
  messageBox.setIconPixmap(generateSvgIcon("data-information").pixmap(48, 48));
  messageBox.setWindowTitle("Proceed?");
  messageBox.setText("Actual taken settings will get lost!");
  QPushButton *noButton  = messageBox.addButton(tr("No"), QMessageBox::NoRole);
  QPushButton *yesButton = messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
  messageBox.setDefaultButton(noButton);
  auto reply = messageBox.exec();
  return messageBox.clickedButton() != noButton;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResultsTableGenerator::loadTemplates()
{
  auto foundTemplates = joda::templates::TemplateParser::findTemplates(
      {"templates/results", joda::templates::TemplateParser::getUsersTemplateDirectory().string()}, joda::fs::EXT_RESULTS_TABLE_TEMPLATE);

  mTemplateMenu->clear();
  std::string actCategory = "basic";
  size_t addedPerCategory = 0;
  for(const auto &[category, dataInCategory] : foundTemplates) {
    for(const auto &[_, data] : dataInCategory) {
      // Now the user templates start, add an addition separator
      if(category != actCategory) {
        actCategory = category;
        if(addedPerCategory > 0) {
          mTemplateMenu->addSeparator();
        }
      }
      QAction *action;
      if(!data.icon.isNull()) {
        action = mTemplateMenu->addAction(QIcon(data.icon.scaled(28, 28)), data.title.data());
      } else {
        action = mTemplateMenu->addAction(generateSvgIcon("favorite"), data.title.data());
      }
      connect(action, &QAction::triggered, [this, path = data.path]() { openTemplate(path.data()); });
    }
    addedPerCategory = dataInCategory.size();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResultsTableGenerator::openTemplate(const QString &path)
{
  try {
    joda::settings::ResultsTemplate settings = joda::templates::TemplateParser::loadTemplate(std::filesystem::path(path.toStdString()));
    mAnalyzeSettings->resultsTemplate        = settings;
    mAnalyzeSettings->resultsSettings        = settings.toSettings(*mAnalyzeSettings);
    refreshView();
    mMainWindow->checkForSettingsChanged();
  } catch(const std::exception &ex) {
    joda::log::logWarning("Could not load template >" + path.toStdString() + "<. What: " + std::string(ex.what()));
  }
}

}    // namespace joda::ui::gui
