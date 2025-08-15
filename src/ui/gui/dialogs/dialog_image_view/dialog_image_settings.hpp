///
/// \file      histo_toolbar.hpp
/// \author    Joachim Danmayr
/// \date      2025-03-21
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <qcombobox.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qscrollbar.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qtmetamacros.h>
#include <qtoolbar.h>
#include <qwindow.h>
#include <tuple>
#include "backend/enums/enums_units.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/image/image.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"

namespace joda::ui::gui {

class DialogImageSettings : public QDialog
{
  Q_OBJECT

public:
  struct Settings
  {
    enums::ZProjection zProjection = enums::ZProjection::MAX_INTENSITY;
    int32_t imageSeries            = 0;
    int32_t tileWidth              = 4096;

    float pixelWidth  = 0;
    float pixelHeight = 0;
    enums::Units unit = enums::Units::Pixels;
  };

  /////////////////////////////////////////////////////
  DialogImageSettings(Settings *settings, QWidget *parent, const ome::OmeInfo &omeInfo);

private:
  /////////////////////////////////////////////////////
  void accept() override;
  void fromSettings(const ome::OmeInfo &omeInfo);

  /////////////////////////////////////////////////////
  QComboBox *mZprojection;
  QComboBox *mSeries;
  QComboBox *mTileSize;

  QLineEdit *mPixelWidth;
  QComboBox *mUnitWidth;

  QLineEdit *mPixelHeight;
  QComboBox *mUnitHeight;

  Settings *mSettings;
};

}    // namespace joda::ui::gui
