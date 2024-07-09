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

  connect(mImageViewLeft, &PanelImageView::onImageRepainted, this, &DialogImageViewer::onLeftViewChanged);
  connect(mImageViewRight, &PanelImageView::onImageRepainted, this, &DialogImageViewer::onRightViewChanged);
}

void DialogImageViewer::onLeftViewChanged()
{
  if(nullptr != mImageViewRight && nullptr != mImageViewLeft) {
    mImageViewRight->blockSignals(true);
    mImageViewLeft->blockSignals(true);

    mImageViewRight->horizontalScrollBar()->setValue(mImageViewLeft->horizontalScrollBar()->value());
    mImageViewRight->verticalScrollBar()->setValue(mImageViewLeft->verticalScrollBar()->value());
    mImageViewRight->setTransform(mImageViewLeft->transform());

    mImageViewRight->blockSignals(false);
    mImageViewLeft->blockSignals(false);
  }
}

void DialogImageViewer::onRightViewChanged()
{
  if(nullptr != mImageViewRight && nullptr != mImageViewLeft) {
    mImageViewRight->blockSignals(true);
    mImageViewLeft->blockSignals(true);

    mImageViewLeft->horizontalScrollBar()->setValue(mImageViewRight->horizontalScrollBar()->value());
    mImageViewLeft->verticalScrollBar()->setValue(mImageViewRight->verticalScrollBar()->value());
    mImageViewLeft->setTransform(mImageViewRight->transform());

    mImageViewRight->blockSignals(false);
    mImageViewLeft->blockSignals(false);
  }
}

}    // namespace joda::ui::qt
