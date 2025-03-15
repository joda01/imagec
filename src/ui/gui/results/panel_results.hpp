///
/// \file      panel_heatmap.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-21
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <qaction.h>
#include <qboxlayout.h>
#include <qcolormap.h>
#include <qcombobox.h>
#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qwidget.h>
#include <memory>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/database/plugins/filter.hpp"
#include "backend/helper/table/table.hpp"
#include "controller/controller.hpp"
#include "heatmap/panel_heatmap.hpp"
#include "ui/gui/container/panel_edit_base.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/panel_preview.hpp"
#include "ui/gui/window_main/panel_results_info.hpp"

namespace joda::ui::gui {
class WindowMain;
}

namespace joda::ui::gui {

class DialogColumnSettings;

///
/// \class      PanelResults
/// \author     Joachim Danmayr
/// \brief      Heatmap panel
///
class PanelResults : public PanelEdit
{
  Q_OBJECT

public:
  enum class Navigation
  {
    PLATE = 0,
    WELL  = 1,
    IMAGE = 2
  };

  /////////////////////////////////////////////////////
  PanelResults(WindowMain *win);
  ~PanelResults();
  void openFromFile(const QString &pathToDbFile);
  void setActive(bool);
  [[nodiscard]] Navigation getActualNavigation() const
  {
    return mNavigation;
  }

  [[nodiscard]] uint16_t getSelectedGroup() const
  {
    return mActGroupId;
  }

  [[nodiscard]] uint64_t getSelectedImage() const
  {
    return mSelectedImageId;
  }

  [[nodiscard]] const table::Table &getData() const
  {
    return mHeatmapChart->getData();
  }

signals:
  void finishedLoading();

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t PREVIEW_BASE_SIZE = 450;

  struct DataSet
  {
    struct Value
    {
      double value = 0;
    };

    std::optional<db::AnalyzeMeta> analyzeMeta;
    std::optional<db::GroupInfo> groupMeta;
    std::optional<db::ImageInfo> imageMeta;
    std::optional<Value> value;
    //  std::optional<results::db::PlateMeta> plateMeta;
    //  std::optional<results::db::GroupMeta> groupMeta;
    //  std::optional<results::db::ImageMeta> imageMeta;
    //  std::optional<results::db::ChannelMeta> channelMeta;
    //  std::optional<results::db::ImageChannelMeta> imageChannelMeta;
  };

  /////////////////////////////////////////////////////
  void valueChangedEvent() override;
  void tableToQWidgetTable(const joda::table::Table &table);
  void tableToHeatmap(const joda::table::Table &table);
  void paintEmptyHeatmap();
  void goHome();
  void refreshView();
  void refreshBreadCrump();
  void copyTableToClipboard(QTableWidget *table);
  void refreshActSelection();
  void resetSettings();

  /////////////////////////////////////////////////////
  void storeResultsTableSettingsToDatabase();
  void showFileSaveDialog(const QString &filter = "Excel 2007-365 (*.xlsx);;Text CSV (*.csv);;R-Script (*.r);;ImageC export template (*" +
                                                  QString(joda::fs::EXT_EXPORT_TEMPLATE.data()) + ")");
  void saveData(const std::string &fileName, joda::ctrl::ExportSettings::ExportType);
  void saveTemplate(const std::string &fileName);
  void showOpenFileDialog();
  void loadTemplate(const std::string &pathToOpenFileFrom);
  void backTo(Navigation backTo);

  auto getWellOrder() const -> std::vector<std::vector<int32_t>>;
  void setWellOrder(const std::vector<std::vector<int32_t>> &wellOrder);
  void setPlateSize(const QSize &size);
  auto getPlateSize() const -> QSize;
  void setDensityMapSize(uint32_t densityMapSize);
  auto getDensityMapSize() const -> uint32_t;

  WindowMain *mWindowMain;
  std::unique_ptr<joda::db::Database> mAnalyzer;
  std::filesystem::path mDbFilePath;

  // Breadcrumb///////////////////////////////////////////////////

  QLabel *mSelectedRowInfo;
  QLineEdit *mSelectedValue;

  QPushButton *mBreadCrumpPlate;
  QPushButton *mBreadCrumpWell;
  QPushButton *mBreadCrumpImage;

  // Toolbar///////////////////////////////////////////////////
  void createToolBar(joda::ui::gui::helper::LayoutGenerator *);
  auto getClasssFromCombo() const -> std::pair<std::string, std::string>;
  QPushButton *mTableButton   = nullptr;
  QPushButton *mHeatmapButton = nullptr;

  PanelPreview *mPreviewImage;
  // uint32_t mDensityMapSize = 200;
  QComboBox *mColumn;

  /// COLUMN EDIT //////////////////////////////////////////////////
  void createEditColumnDialog();
  void columnEdit(int32_t colIdx);
  DialogColumnSettings *mColumnEditDialog;

  /////////////////////////////////////////////////////
  settings::ResultsSettings mFilter;
  PlaceholderTableWidget *mTable;
  std::map<int32_t, joda::table::Table> mActListData;
  std::map<int32_t, joda::table::Table> mActHeatmapData;
  table::Table mSelectedTable;
  int32_t mSelectedTableColumn = -1;
  int32_t mSelectedTableRow    = -1;

  std::mutex mSelectMutex;

  // TOOLBARS///////////////////////////////////////////////////

  /////////////////////////////////////////////////////
  void setHeatmapVisible(bool);
  QHBoxLayout *mHeatmapContainer;
  ChartHeatMap *mHeatmapChart;
  Navigation mNavigation = Navigation::PLATE;
  QAction *mMarkAsInvalid;
  DataSet mSelectedDataSet;
  QAction *mExportSvg = nullptr;
  QAction *mExportPng = nullptr;

  /// HEATMAP SIDEBAR//////////////////////////////////////////////////
  QLineEdit *mWellOrderMatrix;
  QComboBox *mPlateSize;
  QComboBox *mDensityMapSize;

  /////////////////////////////////////////////////////
  uint16_t mActGroupId = 0;
  uint64_t mActImageId = 0;

  uint32_t mSelectedWellId;
  uint64_t mSelectedImageId;
  uint32_t mSelectedTileId;
  Point mSelectedAreaPos;

  struct Selection
  {
    int32_t row = 0;
    int32_t col = 0;
  };

  std::map<Navigation, Selection> mSelection;

  bool mIsLoading = false;
  bool mIsActive  = false;

public slots:
  void onFinishedLoading();
  void onMarkAsInvalidClicked(bool);
  void onElementSelected(int cellX, int cellY, table::TableCell value);
  void onOpenNextLevel(int cellX, int cellY, table::TableCell value);
  void onTableCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
  void onExportImageClicked();
  void onShowTable();
  void onShowHeatmap();
  void onCellClicked(int row, int column);
  void onColumnComboChanged();
};

}    // namespace joda::ui::gui
