///
/// \file      panel_image.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qcombobox.h>
#include <qdockwidget.h>
#include <qwidget.h>
#include <QtWidgets>
#include <filesystem>
#include <optional>
#include <utility>
#include "backend/plot/plot_base.hpp"
#include "backend/settings/results_settings/results_settings.hpp"

namespace joda::db {
class Database;
namespace data {
enum class ColormapName;
enum class ColorMappingMode;
}    // namespace data
}    // namespace joda::db

namespace joda::settings {
class ResultsSettings;
}    // namespace joda::settings

namespace joda::ui::gui {
class WindowMain;

///
/// \class
/// \author
/// \brief
///
class PanelGraphSettings : public QDockWidget
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  explicit PanelGraphSettings(WindowMain *windowMain);
  int32_t getSelectedColumn() const
  {
    return mColumn->currentData().toInt();
  }
  void setColumns(const std::map<joda::settings::ResultsSettings::ColumnIdx, joda::settings::ResultsSettings::ColumnKey> &);

  auto getWellOrder() const -> std::vector<std::vector<int32_t>>;
  auto getPlateSize() const -> QSize;
  auto getDensityMapSize() const -> uint32_t;
  void fromSettings(const std::vector<std::vector<int32_t>> &wellOrder, const QSize &plateSize, uint32_t densityMapSize);
  joda::plot::ColormapName getSelectedColorMap() const;
  auto getColorMapRangeSetting() const -> joda::plot::ColorMappingMode;
  auto getColorMapRange() const -> joda::plot::ColorMappingRange;
  void setColorMapRange(const joda::plot::ColorMappingRange &);

signals:
  void settingsChanged();

private:
  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  QComboBox *mColumn;
  QLineEdit *mWellOrderMatrix;
  QComboBox *mPlateSize;
  QComboBox *mDensityMapSize;
  QComboBox *mColorMaps;
  QComboBox *mColormapRangeSettings;
  QLineEdit *mColorMapMinValue;
  QLineEdit *mColorMapMaxValue;
};
}    // namespace joda::ui::gui
