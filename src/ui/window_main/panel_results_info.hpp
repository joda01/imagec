///
/// \file      panel_image.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <qcombobox.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <QtWidgets>
#include <filesystem>
#include <optional>
#include <utility>
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/vchannel/vchannel_voronoi_settings.hpp"
#include "ui/container/channel/container_channel.hpp"
#include "ui/container/container_base.hpp"
#include "ui/container/intersection/container_intersection.hpp"
#include "ui/container/voronoi/container_voronoi.hpp"
#include "ui/helper/table_widget.hpp"

namespace joda::ui::qt {

class WindowMain;

///
/// \class
/// \author
/// \brief
///
class PanelResultsInfo : public QWidget
{
  Q_OBJECT

public:
  struct DataSet
  {
    std::optional<results::db::AnalyzeMeta> analyzeMeta;
    std::optional<results::db::PlateMeta> plateMeta;
    std::optional<results::db::GroupMeta> groupMeta;
    std::optional<results::db::ImageMeta> imageMeta;
    std::optional<results::db::ChannelMeta> channelMeta;
    std::optional<results::db::ImageChannelMeta> imageChannelMeta;
  };

  /////////////////////////////////////////////////////
  explicit PanelResultsInfo(WindowMain *windowMain);
  void setData(const DataSet &);
  [[nodiscard]] auto getWellOrder() const -> std::vector<std::vector<int32_t>>;
  [[nodiscard]] auto getPlateSize() const -> QSize;
  void addResultsFileToHistory(const std::filesystem::path &dbFile, const std::string &jobName,
                               const std::chrono::system_clock::time_point &time);
  void clearHistory();

signals:
  void settingsChanged();

private:
  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  PlaceholderTableWidget *mLastLoadedResults;
  PlaceholderTableWidget *mResultsProperties;
  std::set<std::string> mAddedPaths;

  /////////////////////////////////////////////////////
  QComboBox *mPlateSize;
  QLineEdit *mWellOrderMatrix;

private slots:
  void filterResults();
};
}    // namespace joda::ui::qt