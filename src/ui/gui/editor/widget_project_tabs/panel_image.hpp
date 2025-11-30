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
#include <qwidget.h>
#include <QtWidgets>
#include <filesystem>
#include <utility>
#include "backend/helper/ome_parser/ome_info.hpp"
#include "ui/gui/helper/table_widget.hpp"

namespace joda::ui::gui {

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
  [[nodiscard]] auto getSelectedImage() const -> std::tuple<std::filesystem::path, int32_t, joda::ome::OmeInfo>;
  [[nodiscard]] auto getSelectedImageOrFirst() const -> std::tuple<std::filesystem::path, int32_t, joda::ome::OmeInfo>;
  void deselectImages();

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

  mutable std::filesystem::path mPathOfFirst;
  mutable joda::ome::OmeInfo mOmeOfFirstImage;

private slots:
  void filterImages();
};
}    // namespace joda::ui::gui
