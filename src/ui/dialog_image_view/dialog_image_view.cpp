///
/// \file      dialog_image_view.cpp
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

#include "dialog_image_view.hpp"
#include <qdialog.h>
#include <qgridlayout.h>

namespace joda::ui::qt {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogImageViewer::DialogImageViewer(QWidget *parent) : QDialog(parent)
{
  setModal(false);
  setBaseSize(800, 800);

  QGridLayout *centralLayout = new QGridLayout(this);

  mImageViewLeft = new PanelImageView(this);
  mImageViewLeft->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  centralLayout->addWidget(mImageViewLeft, 0, 0);
  mImageViewLeft->resetImage();

  mImageViewRight = new PanelImageView(this);
  mImageViewRight->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  centralLayout->addWidget(mImageViewRight, 0, 1);
  mImageViewRight->resetImage();
}

}    // namespace joda::ui::qt
