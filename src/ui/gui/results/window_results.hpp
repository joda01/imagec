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
#include "backend/database/database.hpp"
#include "backend/database/query/filter.hpp"
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/table/table.hpp"
#include "backend/helper/thread_safe_queue.hpp"
#include "controller/controller.hpp"
#include "ui/gui/helper/layout_generator.hpp"

class PlaceholderTableWidget;

namespace joda::ui::gui {
class WindowMain;
}

namespace joda::ui::gui {

class ImageViewDockWidget;
class HeatmapWidget;
class PanelClassificationList;
class PanelGraphSettings;
class Dashboard;
class VideoControlButtonGroup;

///
/// \class      WindowResults
/// \author     Joachim Danmayr
/// \brief      Heatmap panel
///
class WindowResults : public QMainWindow
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
  WindowResults(WindowMain *win);
  ~WindowResults();
  void openFromFile(const QString &pathToDbFile);
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

  void setSelectedElement(table::TableCell value);
  void openNextLevel(const std::vector<table::TableCell> &selectedRows);

signals:
  void finishedLoading();
  void finishedExport();

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

  void closeEvent(QCloseEvent *event) override;

  /////////////////////////////////////////////////////
  void valueChangedEvent();
  void goHome();
  void refreshView();
  void refreshBreadCrump();
  void refreshActSelection();
  void resetSettings();
  void loadLastOpened();

  /////////////////////////////////////////////////////
  void storeResultsTableSettingsToDatabase();
  void showFileSaveDialog(const QString &filter = "Excel 2007-365 (*.xlsx);;Text CSV (*.csv);;R-Script (*.r)");
  void saveData(const std::string &fileName, joda::exporter::xlsx::ExportSettings::ExportFormat);
  void showOpenFileDialog();
  void backTo(Navigation backTo);
  void setWindowTitlePrefix(const QString &txt);

  WindowMain *mWindowMain;
  std::unique_ptr<joda::db::Database> mAnalyzer;
  std::filesystem::path mDbFilePath;
  QMenu *mOpenProjectMenu = nullptr;

  // Breadcrumb///////////////////////////////////////////////////

  QLabel *mSelectedRowInfo;
  QLineEdit *mSelectedValue;

  QToolBar *mBreadCrumpToolBar;
  QAction *mBreadCrumpPlate;
  QAction *mBreadCrumpWell;
  QAction *mBreadCrumpImage;
  QAction *mOpenNextLevel;
  QLabel *mBreadCrumpInfoText;

  // Toolbar///////////////////////////////////////////////////
  auto createToolBar() -> QToolBar *;
  auto getClasssFromCombo() const -> std::pair<std::string, std::string>;
  QAction *mTableButton   = nullptr;
  QAction *mHeatmapButton = nullptr;

  /// COLUMN EDIT //////////////////////////////////////////////////
  void columnEdit(int32_t colIdx);

  // FILTER ///////////////////////////////////////////////////
  settings::ResultsSettings mFilter;
  settings::ResultsSettings mActFilter;
  std::shared_ptr<db::QueryResult> mActListData;
  std::mutex mSelectMutex;

  // TOOLBARS///////////////////////////////////////////////////
  QAction *mShowPreview;
  QAction *mDeleteCol;
  QAction *mEditCol;

  /// NAVIGATION//////////////////////////////////////////////////
  Navigation mNavigation = Navigation::PLATE;
  QAction *mMarkAsInvalid;
  DataSet mSelectedDataSet;
  QAction *mExports   = nullptr;
  QAction *mExportSvg = nullptr;
  QAction *mExportPng = nullptr;

  /// WINDOW//////////////////////////////////////////////////
  QAction *mCascade;
  QAction *mTile;
  QAction *mMinimize;
  QAction *mRestore;

  /// DASHBOARD ///////////////////////////////////////
  Dashboard *mDashboard;
  std::set<std::set<enums::ClassId>> mTmpColocClasses;    // This is just for caching

  /// GRAPH //////////////////////////////////////////////////
  void setHeatmapVisible(bool);
  PanelGraphSettings *mDockWidgetGraphSettings;
  std::shared_ptr<HeatmapWidget> mGraphContainer;

  /// IMAGE DOCK //////////////////////////////////////////////
  void loadPreview();
  bool showSelectWorkingDir(const QString &path);
  std::filesystem::path mImageWorkingDirectory;
  ImageViewDockWidget *mDockWidgetImagePreview = nullptr;
  int32_t mSelectedTileX                       = 0;
  int32_t mSelectedTileY                       = 0;
  std::mutex mGeneratePreviewMutex;

  /// CLASSES DOCK ////////////////////////////////////////////
  PanelClassificationList *mDockWidgetClassList;

  /// PREVIEW /////////////////////////////////////////////////
  struct PreviewData
  {
    std::filesystem::path imagePath;
    db::AnalyzeMeta analyzeMeta;
    db::ImageInfo imageMeta;
    db::ObjectInfo objectInfo;
  };
  void previewThread();
  joda::TSQueue<PreviewData> mPreviewQue;
  std::unique_ptr<std::thread> mPreviewThread;

  // T-STACK //////////////////////////////////////////////////
  VideoControlButtonGroup *mVideoControlButton;

  /////////////////////////////////////////////////////
  uint64 mActGroupId = 0;
  std::set<uint64_t> mActImageId;
  bool mStopped = false;
  uint64_t mSelectedWellId;
  uint64_t mSelectedImageId;
  uint32_t mSelectedTileId;

  struct Selection
  {
    int32_t row = 0;
    int32_t col = 0;
  };

  std::map<Navigation, Selection> mSelection;
  std::mutex mLoadLock;

  bool mIsLoading = false;

public slots:
  void onFinishedExport();
  void onFinishedLoading();
  void onMarkAsInvalidClicked(bool);
  void onShowTable();
  void onShowHeatmap();
  void onColumnComboChanged();
};

}    // namespace joda::ui::gui
