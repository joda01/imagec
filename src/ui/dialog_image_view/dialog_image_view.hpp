///
/// \file      dialog_image_view.hpp
/// \author    Joachim Danmayr
/// \date      2024-07-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <qdialog.h>
#include "panel_image_view.hpp"

namespace joda::ui::qt {

///
/// \class      DialogImageViewer
/// \author     Joachim Danmayr
/// \brief
///
class DialogImageViewer : public QDialog
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogImageViewer(QWidget *parent);

private:
  PanelImageView *mImageViewLeft;
  PanelImageView *mImageViewRight;

private slots:
  void onLeftViewChanged();
  void onRightViewChanged();
};

}    // namespace joda::ui::qt
