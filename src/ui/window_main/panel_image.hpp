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
class PanelImages : public QWidget
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  explicit PanelImages(WindowMain *windowMain);
  [[nodiscard]] auto getSelectedImage() const -> std::tuple<int32_t, int32_t>;

signals:
  void imageSelectionChanged(int32_t newImgIdex, int32_t selectedSeries);

private:
  /////////////////////////////////////////////////////
  void updateImagesList();
  void updateImageMeta();

  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  PlaceholderTableWidget *mImages;
  PlaceholderTableWidget *mImageMeta;
  QLineEdit *mSearchField;

private slots:
  void filterImages();
};
}    // namespace joda::ui::qt
