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
#include <qscrollbar.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qtmetamacros.h>
#include <qtoolbar.h>
#include <qwindow.h>
#include <tuple>
#include "backend/enums/types.hpp"
#include "backend/helper/image/image.hpp"

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
  };

  /////////////////////////////////////////////////////
  DialogImageSettings(Settings *settings, QWidget *parent);

private:
  /////////////////////////////////////////////////////
  void accept() override;
  void fromSettings();

  /////////////////////////////////////////////////////
  QComboBox *mZprojection;
  QComboBox *mSeries;
  QComboBox *mTileSize;

  Settings *mSettings;
};

}    // namespace joda::ui::gui
