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

#include <qboxlayout.h>
#include <qcolormap.h>
#include <qcombobox.h>
#include <qmainwindow.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <memory>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/table/table.hpp"
#include "heatmap/panel_heatmap.hpp"
#include "ui/container/panel_edit_base.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/panel_preview.hpp"
#include "ui/window_main/panel_results_info.hpp"

namespace joda::ui {
class WindowMain;
}

namespace joda::ui {

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

  struct SelectedFilter
  {
    uint32_t plateRows = 0;
    uint32_t plateCols = 0;
    uint32_t plateId   = 0;
    joda::enums::ClusterId clusterId;
    joda::enums::ClassId classId;
    joda::enums::Measurement measurementChannel;
    joda::enums::Stats stats;
    int32_t imageChannel;
    std::vector<std::vector<int32_t>> wellImageOrder;
    uint32_t densityMapAreaSize = 200;
  };

  /////////////////////////////////////////////////////
  PanelResults(WindowMain *win);
  void openFromFile(const QString &pathToDbFile);
  void setData(const SelectedFilter &);
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
    return mHeatmap01->getData();
  }

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t PREVIEW_BASE_SIZE = 450;
  /////////////////////////////////////////////////////
  void valueChangedEvent() override;
  void setAnalyzer();

  WindowMain *mWindowMain;
  std::unique_ptr<joda::db::Database> mAnalyzer;

  // Breadcrumb///////////////////////////////////////////////////
  void createBreadCrump(joda::ui::helper::LayoutGenerator *);
  QPushButton *mBackButton;
  PanelPreview *mPreviewImage;
  uint32_t mDensityMapSize = 200;

  QComboBox *mClusterSelector;
  QComboBox *mClassSelector;
  QComboBox *mMeasurementSelector;
  QComboBox *mImageChannelSelector;
  QComboBox *mStatsSelector;

  /////////////////////////////////////////////////////
  ChartHeatMap *mHeatmap01;
  SelectedFilter mFilter;
  Navigation mNavigation = Navigation::PLATE;
  QComboBox *mMarkAsInvalid;
  PanelResultsInfo::DataSet mSelectedDataSet;

  /////////////////////////////////////////////////////
  QAction *mMarkAsInvalidAction;

  /////////////////////////////////////////////////////
  uint16_t mActGroupId;
  uint64_t mActImageId;

  uint32_t mSelectedWellId;
  uint64_t mSelectedImageId;
  uint32_t mSelectedTileId;
  Point mSelectedAreaPos;

  bool mIsLoading = false;

public slots:
  void onExportClicked();
  void onMarkAsInvalidClicked();
  void onElementSelected(int cellX, int cellY, table::TableCell value);
  void onOpenNextLevel(int cellX, int cellY, table::TableCell value);
  void onBackClicked();
  void repaintHeatmap();
  void paintPlate();
  void paintWell();
  void paintImage();
  void onExportImageClicked();
  void onMeasurementChanged();
};

}    // namespace joda::ui
