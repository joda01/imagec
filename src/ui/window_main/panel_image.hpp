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

///

#pragma once

#include <qcombobox.h>
#include <qwidget.h>
#include <QtWidgets>
#include <filesystem>
#include <utility>
#include "backend/helper/ome_parser/ome_info.hpp"
#include "ui/helper/table_widget.hpp"

namespace joda::ui {

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
  [[nodiscard]] auto getSelectedImage() const -> std::tuple<std::filesystem::path, uint32_t, joda::ome::OmeInfo>;
  [[nodiscard]] auto getSelectedImageOrFirst() const -> std::tuple<std::filesystem::path, uint32_t, joda::ome::OmeInfo>;

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
  joda::ome::OmeInfo mOmeFromActSelectedImage;

private slots:
  void filterImages();
};
}    // namespace joda::ui
