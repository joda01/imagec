///
/// \file      panel_heatmap.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-21
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qaction.h>
#include <qboxlayout.h>
#include <qcolormap.h>
#include <qcombobox.h>
#include <qmainwindow.h>
#include <qpushbutton.h>
#include <qtoolbar.h>
#include <qwidget.h>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <mutex>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/database/plugins/filter.hpp"
#include "backend/helper/table/table.hpp"
#include "controller/controller.hpp"
#include "heatmap/panel_heatmap.hpp"
#include "ui/gui/container/panel_edit_base.hpp"
#include "ui/gui/helper/layout_generator.hpp"

class PlaceholderTableWidget;

namespace joda::ui::gui {
class WindowMain;
}

namespace joda::ui::gui {

class DialogColumnSettings;
class DialogImageViewer;
class QtBackend;

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

  [[nodiscard]] uint64_t getSelectedGroup() const
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
    std::optional<db::ObjectInfo> objectInfo;
    //  std::optional<results::db::PlateMeta> plateMeta;
    //  std::optional<results::db::GroupMeta> groupMeta;
    //  std::optional<results::db::ImageMeta> imageMeta;
    //  std::optional<results::db::ChannelMeta> channelMeta;
    //  std::optional<results::db::ImageChannelMeta> imageChannelMeta;
  };

  /////////////////////////////////////////////////////
  void valueChangedEvent() override;
  void tableToQWidgetTable(const db::QueryResult &table);
  void tableToHeatmap(const db::QueryResult &table);
  void paintEmptyHeatmap();
  void goHome();
  void refreshView();
  void refreshBreadCrump();
  void copyTableToClipboard(QTableWidget *table);
  void refreshActSelection();
  void resetSettings();

  /////////////////////////////////////////////////////
  void storeResultsTableSettingsToDatabase();
  void showFileSaveDialog(const QString &filter = "Excel 2007-365 (*.xlsx);;Text CSV (*.csv);;R-Script (*.r)");
  void saveData(const std::string &fileName, joda::ctrl::ExportSettings::ExportType);
  void showOpenFileDialog();
  void backTo(Navigation backTo);

  auto getWellOrder() const -> std::vector<std::vector<int32_t>>;
  void setWellOrder(const std::vector<std::vector<int32_t>> &wellOrder);
  void setPlateSize(const QSize &size);
  auto getPlateSize() const -> QSize;
  void setDensityMapSize(uint32_t densityMapSize);
  auto getDensityMapSize() const -> uint32_t;
  void openNextLevel(const std::vector<table::TableCell> &selectedRows);

  WindowMain *mWindowMain;
  std::unique_ptr<joda::db::Database> mAnalyzer;
  std::filesystem::path mDbFilePath;

  // Breadcrumb///////////////////////////////////////////////////

  QLabel *mSelectedRowInfo;
  QLineEdit *mSelectedValue;

  QPushButton *mBreadCrumpPlate;
  QPushButton *mBreadCrumpWell;
  QPushButton *mBreadCrumpImage;
  QPushButton *mOpenNextLevel;

  // Toolbar///////////////////////////////////////////////////
  void createToolBar(joda::ui::gui::helper::LayoutGenerator *);
  auto getClasssFromCombo() const -> std::pair<std::string, std::string>;
  QPushButton *mTableButton   = nullptr;
  QPushButton *mHeatmapButton = nullptr;

  /// COLUMN EDIT //////////////////////////////////////////////////
  void createEditColumnDialog();
  void columnEdit(int32_t colIdx);
  DialogColumnSettings *mColumnEditDialog;

  /////////////////////////////////////////////////////
  settings::ResultsSettings mFilter;
  settings::ResultsSettings mActFilter;
  PlaceholderTableWidget *mTable;
  db::QueryResult mActListData;
  int32_t mSelectedTableColumnIdx = -1;
  int32_t mSelectedTableRow       = -1;

  std::mutex mSelectMutex;

  // TOOLBARS///////////////////////////////////////////////////
  QAction *mAutoSort;
  QAction *mDeleteCol;
  QAction *mEditCol;

  /////////////////////////////////////////////////////
  void setHeatmapVisible(bool);
  std::shared_ptr<QtBackend> mGraphContainer;

  QHBoxLayout *mHeatmapContainer;
  ChartHeatMap *mHeatmapChart;
  Navigation mNavigation = Navigation::PLATE;
  QAction *mMarkAsInvalid;
  DataSet mSelectedDataSet;
  QAction *mExportSvg = nullptr;
  QAction *mExportPng = nullptr;

  /// HEATMAP SIDEBAR//////////////////////////////////////////////////
  QComboBox *mColumn;
  QLineEdit *mWellOrderMatrix;
  QComboBox *mPlateSize;
  QComboBox *mDensityMapSize;

  /// IMAGE DOCK //////////////////////////////////////////////
  void loadPreview();
  bool showSelectWorkingDir(const QString &path);
  std::filesystem::path mImageWorkingDirectory;
  DialogImageViewer *mPreviewImage = nullptr;
  joda::ome::OmeInfo mImgProps;
  int32_t mSelectedTileX = 0;
  int32_t mSelectedTileY = 0;
  std::mutex mGeneratePreviewMutex;

  /////////////////////////////////////////////////////
  uint64 mActGroupId = 0;
  std::set<uint64_t> mActImageId;

  uint64_t mSelectedWellId;
  uint64_t mSelectedImageId;
  uint32_t mSelectedTileId;
  QPoint mSelectedAreaPos;

  struct Selection
  {
    int32_t row = 0;
    int32_t col = 0;
  };

  std::map<Navigation, Selection> mSelection;
  std::mutex mLoadLock;

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
  void onTileClicked(int32_t tileX, int32_t tileY);
};

}    // namespace joda::ui::gui
