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
#include "backend/results/analyzer/analyzer.hpp"
#include "backend/results/db_column_ids.hpp"
#include "backend/results/table/table.hpp"
#include "ui/container/container_function.hpp"
#include "ui/container/container_label.hpp"
#include "ui/panel_preview.hpp"
#include "ui/reporting/plugins/heatmap_color_generator.hpp"

namespace joda::ui::qt::reporting::plugin {

class PanelHeatmap;

struct Point
{
  uint32_t x = 0;
  uint32_t y = 0;
};

///
/// \class      ChartHeatMap
/// \author     Joachim Danmayr
/// \brief      Chart plotting a heatmap
///
class ChartHeatMap : public QWidget
{
  Q_OBJECT

public:
  enum class MatrixForm
  {
    CIRCLE,
    RECTANGLE
  };

  enum class PaintControlImage
  {
    NO,
    YES
  };

  /////////////////////////////////////////////////////
  ChartHeatMap(PanelHeatmap *parent);
  void setData(std::weak_ptr<joda::results::Analyzer> analyzer, const joda::results::Table &, MatrixForm form,
               PaintControlImage paint, int32_t newHierarchy);

  [[nodiscard]] const results::Table &getData() const
  {
    return mData;
  }

signals:
  void onElementClick(int cellX, int cellY, results::TableCell value);
  void onDoubleClicked(int cellX, int cellY, results::TableCell value);

private:
  /////////////////////////////////////////////////////
  QString formatDoubleScientific(double value, int precision = 3);
  static void drawGaussianCurve(QPainter &painter, int startX, int startY, int height, int length);
  static double calcValueOnGaussianCurve(double x, double avg, double sttdev);
  void paintEvent(QPaintEvent *ev) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;

  // Function to find the nearest element in the map to a given value x
  static std::pair<float, QColor> findNearest(std::map<float, QColor> &myMap, double target);
  std::tuple<int32_t, Point> getWellUnderMouse(QMouseEvent *event);

  joda::results::Table mData;
  /*std::map<float, QColor> mColorMap{{0.1, QColor{32, 102, 168}},  {0.2, QColor{142, 193, 218}},
                                    {0.3, QColor{205, 225, 236}}, {0.4, QColor{237, 237, 237}},
                                    {0.5, QColor{246, 214, 194}}, {0.6, QColor{246, 214, 194}},
                                    {0.7, QColor{246, 214, 194}}, {0.8, QColor{212, 114, 100}},
                                    {0.9, QColor{174, 40, 44}},   {1, QColor{174, 40, 44}}};*/

  std::map<float, QColor> mColorMap{generateColorMap()};

  /////////////////////////////////////////////////////
  static inline const uint32_t spacing                 = 4;
  static inline const uint32_t Y_TOP_MARING            = 16;
  static inline const uint32_t X_LEFT_MARGIN           = 10;
  static inline const uint32_t LEGEND_HEIGHT           = 30;
  static inline const uint32_t LEGEND_COLOR_ROW_HEIGHT = 15;
  static inline const uint32_t HEATMAP_FONT_SIZE       = 12;

  std::weak_ptr<joda::results::Analyzer> mAnalyzer;
  PanelHeatmap *mParent;
  MatrixForm mForm = MatrixForm::CIRCLE;
  PaintControlImage mPaintCtrlImage;

  uint32_t mRows        = 0;
  uint32_t mCols        = 0;
  int32_t mHoveredWell  = -1;
  int32_t mActHierarchy = 0;

  struct Selection
  {
    int32_t mSelectedWell = -1;
    Point mSelectedPoint;
  };

  std::map<int32_t, Selection> mSelection;

  QString mActControlImagePath;
  QImage mActControlImage;
  bool mIsHovering = false;
};

///
/// \class      PanelHeatmap
/// \author     Joachim Danmayr
/// \brief      Heatmap panel
///
class PanelHeatmap : public QWidget
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
  PanelHeatmap(QMainWindow *win, QWidget *parent);
  void setData(std::weak_ptr<joda::results::Analyzer> analyzer, const SelectedFilter &);
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

signals:
  void loadingStarted();
  void loadingFinished();

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t PREVIEW_BASE_SIZE = 450;

  /////////////////////////////////////////////////////
  QWidget *createBreadCrump(QWidget *);
  QAction *mBackButton;
  PanelPreview *mPreviewImage;

  /////////////////////////////////////////////////////
  ChartHeatMap *mHeatmap01;
  std::weak_ptr<joda::results::Analyzer> mAnalyzer;
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
  uint32_t mSelectedImageId;
  uint64_t mSelectedTileId;
  Point mSelectedAreaPos;

  bool mIsLoading = false;

public slots:
  void onMarkAsInvalidClicked();
  void onElementSelected(int cellX, int cellY, results::TableCell value);
  void onOpenNextLevel(int cellX, int cellY, results::TableCell value);
  void onBackClicked();
  void repaintHeatmap();
  void paintPlate();
  void paintWell();
  void paintImage();
  void onExportImageClicked();
};

}    // namespace joda::ui::qt::reporting::plugin
