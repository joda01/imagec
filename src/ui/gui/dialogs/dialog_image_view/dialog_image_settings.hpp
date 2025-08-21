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

    float pixelWidth                 = 0;
    float pixelHeight                = 0;
    enums::Units pixelSizeUnit       = enums::Units::um;
    enums::PhysicalSizeMode sizeMode = enums::PhysicalSizeMode::Automatic;
  };

  /////////////////////////////////////////////////////
  DialogImageSettings(Settings *settings, QWidget *parent, const ome::OmeInfo &omeInfo);

private:
  /////////////////////////////////////////////////////
  void accept() override;
  void fromSettings(const ome::OmeInfo &omeInfo);
  void setFromOme(const ome::OmeInfo &omeInfo, int32_t imgSeries, enums::Units unit);

  /////////////////////////////////////////////////////
  QComboBox *mZprojection;
  QComboBox *mSeries;
  QComboBox *mTileSize;

  QComboBox *mPixelSizeMode;
  QComboBox *mUnit;

  QLineEdit *mPixelWidth;

  QLineEdit *mPixelHeight;

  Settings *mSettings;

  const ome::OmeInfo &mOmeInfo;
};

}    // namespace joda::ui::gui
