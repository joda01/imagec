///
/// \file      panel_heatmap.cpp
/// \author    Joachim Danmayr
/// \date      2024-05-21
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "panel_results.hpp"
#include <qaction.h>
#include <qactiongroup.h>
#include <qboxlayout.h>
#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qevent.h>
#include <qgridlayout.h>
#include <qlayout.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qsize.h>
#include <qtablewidget.h>
#include <qtoolbar.h>
#include <qwidget.h>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <cmath>
#include <exception>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/database/exporter/xlsx/exporter.hpp"
#include "backend/helper/database/plugins/control_image.hpp"
#include "backend/helper/database/plugins/filter.hpp"
#include "backend/helper/database/plugins/stats_for_image.hpp"
#include "backend/helper/database/plugins/stats_for_well.hpp"
#include "ui/container/container_button.hpp"
#include "ui/container/container_label.hpp"
#include "ui/container/panel_edit_base.hpp"
#include "ui/container/setting/setting_combobox_classification_unmanaged.hpp"
#include "ui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/helper/icon_generator.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/panel_preview.hpp"
#include "ui/window_main/panel_results_info.hpp"
#include "ui/window_main/window_main.hpp"
#include <nlohmann/json_fwd.hpp>
#include "dialog_column_settings.hpp"

namespace joda::ui {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
PanelResults::PanelResults(WindowMain *windowMain) : PanelEdit(windowMain, nullptr, false), mWindowMain(windowMain)
{
  // Drop downs
  createEditColumnDialog();
  createBreadCrump(&layout());

  //
  // Create Table
  //
  mTable = new PlaceholderTableWidget();
  mTable->setPlaceholderText("Click >Add column> to add your first column.");
  mTable->setRowCount(0);
  mTable->setColumnCount(0);
  mTable->verticalHeader()->setDefaultSectionSize(8);    // Set each row to 50 pixels height

  connect(mTable, &QTableWidget::currentCellChanged, this, &PanelResults::onTableCurrentCellChanged);
  connect(mTable->verticalHeader(), &QHeaderView::sectionDoubleClicked,
          [this](int logicalIndex) { onOpenNextLevel(logicalIndex, 0, mSelectedTable.data(logicalIndex, 0)); });

  connect(mTable, &QTableWidget::cellDoubleClicked, [this](int row, int column) { onOpenNextLevel(row, 0, mSelectedTable.data(row, 0)); });
  connect(mTable, &QTableWidget::cellClicked, this, &PanelResults::onCellClicked);

  // Middle layout
  auto *tab  = layout().addTab("", [] {});
  auto *col  = tab->addVerticalPanel();
  mHeatmap01 = new ChartHeatMap(this);
  mHeatmap01->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  connect(mHeatmap01, &ChartHeatMap::onElementClick, this, &PanelResults::onElementSelected);
  connect(mHeatmap01, &ChartHeatMap::onDoubleClicked, this, &PanelResults::onOpenNextLevel);
  connect(layout().getBackButton(), &QAction::triggered, [this] { mWindowMain->showPanelStartPage(); });
  connect(getWindowMain()->getPanelResultsInfo(), &joda::ui::PanelResultsInfo::settingsChanged, [this]() { refreshView(); });

  col->setContentsMargins(0, 0, 0, 0);
  col->setSpacing(0);
  col->addWidget(mHeatmap01);
  col->addWidget(mTable);

  onShowTable();
  refreshView();
}

void PanelResults::valueChangedEvent()
{
}

///
/// \brief
/// \author     Joachim Danmayr
///
void PanelResults::setActive(bool active)
{
  if(!active) {
    getWindowMain()->getPanelResultsInfo()->setData({});
    mAnalyzer.reset();
  }
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelResults::createBreadCrump(joda::ui::helper::LayoutGenerator *toolbar)
{
  //
  //
  // Back button
  mBackButton = new QPushButton(generateIcon("arrow-left"), "");
  mBackButton->setEnabled(false);
  connect(mBackButton, &QPushButton::clicked, this, &PanelResults::onBackClicked);
  toolbar->addItemToTopToolbar(mBackButton);

  // Column select
  mColumn = new QComboBox();
  mColumn->setMinimumWidth(150);
  connect(mColumn, &QComboBox::currentIndexChanged, this, &PanelResults::onColumnComboChanged);
  mColumnAction = toolbar->addItemToTopToolbar(mColumn);
  mColumnAction->setVisible(false);

  toolbar->addSeparatorToTopToolbar();

  //
  // Open and save
  //
  auto *openSheet = new QAction(generateIcon("opened-folder"), "Open sheet settings", toolbar);
  connect(openSheet, &QAction::triggered, [this]() { loadTemplate(); });
  toolbar->addItemToTopToolbar(openSheet);

  auto *saveSheet = new QAction(generateIcon("save"), "Save sheet settings", toolbar);
  connect(saveSheet, &QAction::triggered, [this]() { saveTemplate(); });
  toolbar->addItemToTopToolbar(saveSheet);

  toolbar->addSeparatorToTopToolbar();

  //
  //
  //
  auto *grp          = new QActionGroup(toolbar);
  auto *mTableButton = new QAction(generateIcon("table"), "");
  mTableButton->setCheckable(true);
  mTableButton->setChecked(true);
  grp->addAction(mTableButton);
  toolbar->addItemToTopToolbar(mTableButton);

  auto *mHeatmapButton = new QAction(generateIcon("heat-map"), "");
  mHeatmapButton->setCheckable(true);
  grp->addAction(mHeatmapButton);
  toolbar->addItemToTopToolbar(mHeatmapButton);

  connect(mHeatmapButton, &QAction::toggled, [this](bool checked) {
    if(checked) {
      onShowHeatmap();
    }
  });
  connect(mTableButton, &QAction::toggled, [this](bool checked) {
    if(checked) {
      onShowTable();
    }
  });

  toolbar->addSeparatorToTopToolbar();

  //
  // Copy Button
  //
  auto *copy = new QAction(generateIcon("copy"), "");
  copy->setToolTip("Copy table");
  connect(copy, &QAction::triggered, [this]() { copyTableToClipboard(mTable); });
  toolbar->addItemToTopToolbar(copy);

  //
  // Export button
  //
  auto *exportData = new QAction(generateIcon("excel"), "");
  exportData->setToolTip("Export XLSX");
  connect(exportData, &QAction::triggered, [this]() { onExportClicked(ExportFormat::XLSX); });

  toolbar->addItemToTopToolbar(exportData);

  auto *exportR = new QAction(generateIcon("r-studio"), "");
  exportR->setToolTip("Export R");
  toolbar->addItemToTopToolbar(exportR);

  toolbar->addSeparatorToTopToolbar();

  //
  //
  //
  auto *addColumn = new QAction(generateIcon("add-column"), "");
  addColumn->setToolTip("Add column");
  connect(addColumn, &QAction::triggered, [this]() { columnEdit(mTable->columnCount()); });

  toolbar->addItemToTopToolbar(addColumn);

  auto *editColumn = new QAction(generateIcon("edit-column"), "");
  editColumn->setToolTip("Edit column");
  connect(editColumn, &QAction::triggered, [this]() {
    if(mSelectedTableColumn >= 0) {
      columnEdit(mSelectedTableColumn);
    }
  });

  toolbar->addItemToTopToolbar(editColumn);

  auto *deleteColumn = new QAction(generateIcon("delete-column"), "");
  deleteColumn->setToolTip("Delete column");
  connect(deleteColumn, &QAction::triggered, [this]() {
    if(mSelectedTableColumn >= 0) {
      mFilter.eraseColumn({.tabIdx = 0, .colIdx = mSelectedTableColumn});
      refreshView();
    }
  });

  toolbar->addItemToTopToolbar(deleteColumn);

  //
  //
  //
  mMarkAsInvalid = new QComboBox();
  mMarkAsInvalid->addItem("Valid", false);
  mMarkAsInvalid->addItem("Invalid", true);
  mMarkAsInvalidAction = toolbar->addItemToTopToolbar(mMarkAsInvalid);
  mMarkAsInvalidAction->setVisible(false);
  connect(mMarkAsInvalid, &QComboBox::currentIndexChanged, this, &PanelResults::onMarkAsInvalidClicked);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::setAnalyzer()
{
  mSelectedDataSet.analyzeMeta = mAnalyzer->selectExperiment();
  mColumnEditDialog->updateClustersAndClasses(mAnalyzer.get());
  getWindowMain()->getPanelResultsInfo()->setData(mSelectedDataSet);
  refreshView();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::refreshView()
{
  const auto &size      = mWindowMain->getPanelResultsInfo()->getPlateSize();
  const auto &wellOrder = mWindowMain->getPanelResultsInfo()->getWellOrder();

  mFilter.setFilter(mAnalyzer.get(), {.plateId         = 0,
                                      .groupId         = mActGroupId,
                                      .imageId         = mActImageId,
                                      .plateRows       = static_cast<uint16_t>(size.height()),
                                      .plateCols       = static_cast<uint16_t>(size.width()),
                                      .heatmapAreaSize = mWindowMain->getPanelResultsInfo()->getDensityMapSize(),
                                      .wellImageOrder  = wellOrder});

  //
  //
  //
  if(mAnalyzer && !mIsLoading) {
    mIsLoading = true;
    std::thread([this] {
      switch(mNavigation) {
        case Navigation::PLATE:
          mBackButton->setEnabled(false);
          {
            mActListData = joda::db::StatsPerGroup::toTable(mFilter, db::StatsPerGroup::Grouping::BY_PLATE);
            if(!mActListData.empty()) {
              tableToQWidgetTable(mActListData[0]);
            } else {
              mTable->setRowCount(0);
              mTable->setColumnCount(0);
            }
          }
          {
            mActHeatmapData = joda::db::StatsPerGroup::toHeatmap(mFilter, db::StatsPerGroup::Grouping::BY_PLATE);
            if(!mActHeatmapData.empty() && mActHeatmapData.contains(mColumn->currentData().toInt())) {
              tableToHeatmap(mActHeatmapData[mColumn->currentData().toInt()]);
            } else {
              paintEmptyHeatmap();
            }
          }
          break;
        case Navigation::WELL:
          mBackButton->setEnabled(true);
          {
            mActListData = joda::db::StatsPerGroup::toTable(mFilter, db::StatsPerGroup::Grouping::BY_WELL);
            if(!mActListData.empty()) {
              tableToQWidgetTable(mActListData[0]);
            } else {
              mTable->setRowCount(0);
              mTable->setColumnCount(0);
            }
          }
          {
            mActHeatmapData = joda::db::StatsPerGroup::toHeatmap(mFilter, db::StatsPerGroup::Grouping::BY_WELL);
            if(!mActHeatmapData.empty() && mActHeatmapData.contains(mColumn->currentData().toInt())) {
              tableToHeatmap(mActHeatmapData[mColumn->currentData().toInt()]);
            } else {
              paintEmptyHeatmap();
            }
          }
          break;
        case Navigation::IMAGE:
          mBackButton->setEnabled(true);
          {
            mActListData = joda::db::StatsPerImage::toTable(mFilter);
            if(!mActListData.empty()) {
              tableToQWidgetTable(mActListData[0]);
            } else {
              mTable->setRowCount(0);
              mTable->setColumnCount(0);
            }
          }
          {
            mActHeatmapData = joda::db::StatsPerImage::toHeatmap(mFilter);
            if(!mActHeatmapData.empty() && mActHeatmapData.contains(mColumn->currentData().toInt())) {
              tableToHeatmap(mActHeatmapData[mColumn->currentData().toInt()]);
            } else {
              paintEmptyHeatmap();
            }
          }
          break;
      }
      update();
      mIsLoading = false;
    }).detach();
  }
}

///
/// \brief      Export image
/// \author     Joachim Danmayr
///
void PanelResults::onExportImageClicked()
{
  /*
  cv::Rect rectangle;
  rectangle.x      = mSelectedAreaPos.y * mFilter.densityMapAreaSize;    // Images are mirrored in the coordinates
  rectangle.y      = mSelectedAreaPos.x * mFilter.densityMapAreaSize;    // Images are mirrored in the coordinates
  rectangle.width  = mFilter.densityMapAreaSize;
  rectangle.height = mFilter.densityMapAreaSize;

  auto retImage = joda::results::analyze::plugins::ControlImage::getControlImage(
      *mAnalyzer, mActImageId, mFilter.channelIdx, mSelectedTileId, rectangle);

  QString filePath =
      QFileDialog::getSaveFileName(this, "Save File", mAnalyzer->getBasePath().string().data(), "PNG Files (*.png)");
  if(filePath.isEmpty()) {
    return;
  }
  if(!filePath.endsWith(".png")) {
    filePath += ".png";
  }
  bool isSuccess = cv::imwrite(filePath.toStdString(), retImage);

  QDesktopServices::openUrl(QUrl("file:///" + filePath));
  */
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelResults::onMarkAsInvalidClicked()
{
  if(mMarkAsInvalid->currentData().toBool()) {
    enums::ChannelValidity val;
    val.set(enums::ChannelValidityEnum::MANUAL_OUT_SORTED);
    mAnalyzer->setImageValidity(mSelectedImageId, val);
  } else {
    enums::ChannelValidity val;
    val.set(enums::ChannelValidityEnum::MANUAL_OUT_SORTED);
    mAnalyzer->unsetImageValidity(mSelectedImageId, val);
  }
  refreshView();
}

///
/// \brief      An element has been selected
/// \author     Joachim Danmayr
///
void PanelResults::onElementSelected(int cellX, int cellY, table::TableCell value)
{
  switch(mNavigation) {
    case Navigation::PLATE: {
      mSelectedWellId = value.getId();

      //  auto groupId = value.getId();
      //  auto [result, channel] =
      //      mAnalyzer->getGroupInformation(mFilter.analyzeId, mFilter.plateId, mFilter.channelIdx, groupId);
      //  mSelectedDataSet.groupMeta   = result;
      //  mSelectedDataSet.channelMeta = channel;
      mSelectedDataSet.imageMeta.reset();
      mMarkAsInvalidAction->setVisible(false);
    } break;
    case Navigation::WELL: {
      mSelectedImageId = value.getId();

      auto imageInfo             = mAnalyzer->selectImageInfo(value.getId());
      mSelectedDataSet.imageMeta = imageInfo;
      mMarkAsInvalid->blockSignals(true);

      if(imageInfo.validity.test(enums::ChannelValidityEnum::MANUAL_OUT_SORTED)) {
        mMarkAsInvalid->setCurrentIndex(1);
      } else {
        mMarkAsInvalid->setCurrentIndex(0);
      }
      mMarkAsInvalid->blockSignals(false);
      mMarkAsInvalidAction->setVisible(true);
    }

    break;
    case Navigation::IMAGE:
      mSelectedTileId = value.getId();
      mMarkAsInvalidAction->setVisible(false);
      mSelectedAreaPos.x = cellX;
      mSelectedAreaPos.y = cellY;
      break;
  }

  mSelectedDataSet.value = PanelResultsInfo::DataSet::Value{.value = value.getVal()};
  getWindowMain()->getPanelResultsInfo()->setData(mSelectedDataSet);
}

///
/// \brief      Open the next deeper level form the element with given id
/// \author     Joachim Danmayr
///
void PanelResults::onOpenNextLevel(int cellX, int cellY, table::TableCell value)
{
  int actMenu = static_cast<int>(mNavigation);
  actMenu++;
  if(actMenu <= 2) {
    mNavigation = static_cast<Navigation>(actMenu);
  } else {
    // An area has been selected within an image -> trigger an export
    onExportImageClicked();
    return;
  }
  switch(mNavigation) {
    case Navigation::PLATE:
      break;
    case Navigation::WELL:
      mActGroupId = static_cast<uint16_t>(value.getId());
      break;
    case Navigation::IMAGE:
      mActImageId = value.getId();
      break;
  }
  mTable->setCurrentCell(0, 0);
  refreshView();
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelResults::onBackClicked()
{
  int actMenu = static_cast<int>(mNavigation);
  actMenu--;
  if(actMenu >= 0) {
    mNavigation = static_cast<Navigation>(actMenu);
  }
  auto col = mSelection[mNavigation].col;
  auto row = mSelection[mNavigation].row;
  mTable->setCurrentCell(row, col);

  switch(mNavigation) {
    case Navigation::PLATE:
      mSelectedDataSet.imageMeta.reset();
      break;
    case Navigation::WELL:
      break;
    case Navigation::IMAGE:
      break;
  }

  refreshView();
  getWindowMain()->getPanelResultsInfo()->setData(mSelectedDataSet);
}

///
/// \brief
/// \author     Joachim Danmayr
///
void PanelResults::openFromFile(const QString &pathToDbFile)
{
  if(pathToDbFile.isEmpty()) {
    return;
  }
  mDbFilePath = std::filesystem::path(pathToDbFile.toStdString());
  try {
    mAnalyzer = std::make_unique<joda::db::Database>();
    mAnalyzer->openDatabase(std::filesystem::path(pathToDbFile.toStdString()));
    setAnalyzer();
    if(mSelectedDataSet.analyzeMeta.has_value()) {
      getWindowMain()->getPanelResultsInfo()->addResultsFileToHistory(
          std::filesystem::path(pathToDbFile.toStdString()), mSelectedDataSet.analyzeMeta->jobName, mSelectedDataSet.analyzeMeta->timestampStart);
    }

  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateIcon("warning-yellow").pixmap(48, 48));
    messageBox.setWindowTitle("Could not load database!");
    messageBox.setText("Could not load settings, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::onShowTable()
{
  mTable->setVisible(true);
  mColumnAction->setVisible(false);

  mHeatmap01->setVisible(false);
  refreshView();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::onShowHeatmap()
{
  mTable->setVisible(false);
  mColumnAction->setVisible(true);

  mHeatmap01->setVisible(true);
  refreshView();
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelResults::tableToHeatmap(const joda::table::Table &table)
{
  if(mAnalyzer) {
    if(mSelectedTableColumn >= 0) {
      mHeatmap01->setData(table, mNavigation == Navigation::PLATE ? ChartHeatMap::MatrixForm::CIRCLE : ChartHeatMap::MatrixForm::RECTANGLE,
                          ChartHeatMap::PaintControlImage::NO, static_cast<int32_t>(mNavigation));
      return;
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::paintEmptyHeatmap()
{
  joda::table::Table table;
  const auto &size      = mWindowMain->getPanelResultsInfo()->getPlateSize();
  const auto &wellOrder = mWindowMain->getPanelResultsInfo()->getWellOrder();
  uint16_t rows         = size.height();
  uint16_t cols         = size.width();
  for(int row = 0; row < rows; row++) {
    table.getMutableRowHeader()[row] = "";
    for(int col = 0; col < cols; col++) {
      table.getMutableColHeader()[col] = "";
      table::TableCell data;
      table.setData(row, col, data);
    }
  }
  mHeatmap01->setData(table, ChartHeatMap::MatrixForm::CIRCLE, ChartHeatMap::PaintControlImage::NO, static_cast<int32_t>(mNavigation));
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::createEditColumnDialog()
{
  mColumnEditDialog = new DialogColumnSettings(&mFilter, mWindowMain);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::columnEdit(int32_t colIdx)
{
  mColumnEditDialog->exec(colIdx);
  refreshView();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::tableToQWidgetTable(const joda::table::Table &tableIn)
{
  std::lock_guard<std::mutex> lock(mSelectMutex);
  mSelectedTable = tableIn;
  if(tableIn.getCols() > 0) {
    mTable->setColumnCount(tableIn.getCols());
    mTable->setRowCount(tableIn.getRows());

  } else {
    mTable->setColumnCount(0);
    mTable->setRowCount(0);
  }

  auto createTableWidget = [](const QString &data) {
    auto *widget = new QTableWidgetItem(data);
    widget->setFlags(widget->flags() & ~Qt::ItemIsEditable);
    return widget;
  };

  mColumn->blockSignals(true);
  auto actData = mColumn->currentData();
  mColumn->clear();
  // Header
  for(int col = 0; col < mTable->columnCount(); col++) {
    char txt      = col + 'A';
    auto colCount = QString(std::string(1, txt).data());

    if(tableIn.getCols() > col) {
      QString headerText = tableIn.getColHeader(col).data();
      mColumn->addItem(headerText, col);
      headerText = headerText.replace("[", "\n[");
      mTable->setHorizontalHeaderItem(col, createTableWidget(headerText));

    } else {
      mTable->setHorizontalHeaderItem(col, createTableWidget(colCount));
      mColumn->addItem(colCount, col);
    }
  }
  auto idx = mColumn->findData(actData);
  if(idx >= 0) {
    mColumn->setCurrentIndex(idx);
  }
  mColumn->blockSignals(false);

  // Row
  for(int row = 0; row < mTable->rowCount(); row++) {
    if(tableIn.getRows() > row) {
      mTable->setVerticalHeaderItem(row, createTableWidget(tableIn.getRowHeader(row).data()));
    } else {
      mTable->setVerticalHeaderItem(row, createTableWidget(QString(std::to_string(row).data())));
    }
  }

  for(int col = 0; col < mTable->columnCount(); col++) {
    for(int row = 0; row < mTable->rowCount(); row++) {
      QTableWidgetItem *item = mTable->item(row, col);
      if(item == nullptr) {
        item = createTableWidget(" ");
        mTable->setItem(row, col, item);
      }
      if(item) {
        if(tableIn.getRows() > row && tableIn.getCols() > col) {
          item->setText(QString::number((double) tableIn.data(row, col).getVal()));
        } else {
          item->setText(" ");
        }
      }
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::onColumnComboChanged()
{
  onTableCurrentCellChanged(mSelectedTableRow, mColumn->currentData().toInt(), mSelectedTableRow, mSelectedTableColumn);
  refreshView();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::onTableCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
  mSelectedTableColumn = currentColumn;
  mSelectedTableRow    = currentRow;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::copyTableToClipboard(QTableWidget *table)
{
  QStringList data;
  for(int row = 0; row < table->rowCount(); ++row) {
    QStringList rowData;
    for(int col = 0; col < table->columnCount(); ++col) {
      rowData << table->item(row, col)->text();
    }
    data << rowData.join("\t");    // Join row data with tabs for better readability
  }

  QString text = data.join("\n");    // Join rows with newlines

  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(text);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::onCellClicked(int rowSelected, int columnSelcted)
{
  // Update table
  mSelection[mNavigation] = {rowSelected, columnSelcted};
  auto selectedData       = mActListData.at(0).data(rowSelected, columnSelcted);
  onElementSelected(columnSelcted, rowSelected, selectedData);
}

///
/// \brief      Export data
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::onExportClicked(ExportFormat format)
{
  QString filePathOfSettingsFile;
  switch(format) {
    case ExportFormat::XLSX:
      filePathOfSettingsFile = QFileDialog::getSaveFileName(this, "Save File", mDbFilePath.parent_path().string().data(), "Spreadsheet (*.xlsx)");
      break;
    case ExportFormat::R:
      filePathOfSettingsFile = QFileDialog::getSaveFileName(this, "Save File", mDbFilePath.parent_path().string().data(), "R-Script (*.r)");
      break;
  }

  if(filePathOfSettingsFile.isEmpty()) {
    return;
  }

  std::thread([this, filePathOfSettingsFile, format] {
    if(!mTable->isVisible()) {
      joda::db::BatchExporter::startExportHeatmap(mActHeatmapData, mWindowMain->getSettings(), mSelectedDataSet.analyzeMeta->jobName,
                                                  mSelectedDataSet.analyzeMeta->timestampStart, mSelectedDataSet.analyzeMeta->timestampFinish,
                                                  filePathOfSettingsFile.toStdString());
    } else {
      joda::db::BatchExporter::startExportList(mActListData, mWindowMain->getSettings(), mSelectedDataSet.analyzeMeta->jobName,
                                               mSelectedDataSet.analyzeMeta->timestampStart, mSelectedDataSet.analyzeMeta->timestampFinish,
                                               filePathOfSettingsFile.toStdString());
    }
  }).detach();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::saveTemplate()
{
  QString templatePath      = joda::templates::TemplateParser::getUsersTemplateDirectory().string().data();
  QString pathToStoreFileIn = QFileDialog::getSaveFileName(this, "Save File", templatePath,
                                                           "ImageC export template files (*" + QString(joda::fs::EXT_EXPORT_TEMPLATE.data()) + ")");

  if(pathToStoreFileIn.isEmpty()) {
    return;
  }
  if(!pathToStoreFileIn.startsWith(templatePath)) {
    joda::log::logError("Templates must be stored in >" + templatePath.toStdString() + "< directory.");
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateIcon("warning-yellow").pixmap(48, 48));
    messageBox.setWindowTitle("Could not save template!");
    messageBox.setText("Templates must be stored in >" + templatePath + "< directory.");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
    return;
  }
  nlohmann::json json = mFilter;
  joda::templates::TemplateParser::saveTemplate(json, std::filesystem::path(pathToStoreFileIn.toStdString()), joda::fs::EXT_EXPORT_TEMPLATE);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::loadTemplate()
{
  QString templatePath       = joda::templates::TemplateParser::getUsersTemplateDirectory().string().data();
  QString pathToOpenFileFrom = QFileDialog::getOpenFileName(this, "Open File", templatePath,
                                                            "ImageC export template files (*" + QString(joda::fs::EXT_EXPORT_TEMPLATE.data()) + ")");
  if(pathToOpenFileFrom.isEmpty()) {
    return;
  }
  try {
    auto json = joda::templates::TemplateParser::loadTemplate(std::filesystem::path(pathToOpenFileFrom.toStdString()));
    mFilter   = json;
    refreshView();
  } catch(const std::exception &ex) {
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateIcon("error-red").pixmap(48, 48));
    messageBox.setWindowTitle("Error...");
    messageBox.setText("Error in opening template got >" + QString(ex.what()) + "<.");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

}    // namespace joda::ui
