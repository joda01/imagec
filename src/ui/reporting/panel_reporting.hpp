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
/// \brief     A short description what happens here.
///

#pragma once

#include <qboxlayout.h>
#include <qcolormap.h>
#include <qmainwindow.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <memory>
#include "backend/results/analyzer/analyzer.hpp"
#include "backend/results/db_column_ids.hpp"
#include "backend/results/table/table.hpp"
#include "heatmap/panel_heatmap.hpp"
#include "ui/container/container_function.hpp"
#include "ui/container/container_label.hpp"
#include "ui/container/panel_edit_base.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/panel_preview.hpp"

namespace joda::ui::qt {
class WindowMain;
}

namespace joda::ui::qt {

///
/// \class      PanelReporting
/// \author     Joachim Danmayr
/// \brief      Heatmap panel
///
class PanelReporting : public PanelEdit
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
    std::string analyzeId;
    uint32_t plateRows = 0;
    uint32_t plateCols = 0;
    uint32_t plateId   = 1;
    joda::results::ChannelIndex channelIdx;
    joda::results::MeasureChannelId measureChannel;
    std::vector<std::vector<int32_t>> wellImageOrder;
    joda::results::Stats stats;
    uint32_t densityMapAreaSize = 200;
  };

  /////////////////////////////////////////////////////
  PanelReporting(WindowMain *win);
  void openFromFile(const QString &pathToDbFile);
  void setData(const SelectedFilter &);
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

  [[nodiscard]] const results::Table &getData() const
  {
    return mHeatmap01->getData();
  }

  void setAnalyzer();

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t PREVIEW_BASE_SIZE = 450;
  /////////////////////////////////////////////////////
  void valueChangedEvent() override;

  WindowMain *mWindowMain;
  std::shared_ptr<joda::results::Analyzer> mAnalyzer;

  // Breadcrumb///////////////////////////////////////////////////
  void createBreadCrump(joda::ui::qt::helper::LayoutGenerator *);
  QPushButton *mBackButton;
  PanelPreview *mPreviewImage;
  QComboBox *mChannelSelector;
  QComboBox *mMeasurementSelector;
  QComboBox *mStatsSelector;
  std::vector<results::db::ChannelMeta> mChannelInfos;
  std::string mAnalyzeId;
  uint32_t mDenesityMapSize = 200;

  /////////////////////////////////////////////////////
  ChartHeatMap *mHeatmap01;
  SelectedFilter mFilter;
  Navigation mNavigation = Navigation::PLATE;

  // WELL///////////////////////////////////////////////////
  ContainerLabel *mWellName;
  ContainerLabel *mWellValue;
  ContainerLabel *mWellMeta;

  // Image///////////////////////////////////////////////////
  QWidget *mImageInfoWidget;
  ContainerLabel *mImageName;
  ContainerLabel *mImageValue;
  ContainerLabel *mImageMeta;
  std::shared_ptr<ContainerFunction<bool, bool>> mMarkAsInvalid;

  // Area///////////////////////////////////////////////////
  QWidget *mAreaInfoWidget;
  ContainerLabel *mAreaName;
  ContainerLabel *mAreaValue;
  ContainerLabel *mAreaMeta;

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
  void onElementSelected(int cellX, int cellY, results::TableCell value);
  void onOpenNextLevel(int cellX, int cellY, results::TableCell value);
  void onBackClicked();
  void repaintHeatmap();
  void paintPlate();
  void paintWell();
  void paintImage();
  void onExportImageClicked();
  void onChannelChanged();
  void onMeasurementChanged();
};

}    // namespace joda::ui::qt
