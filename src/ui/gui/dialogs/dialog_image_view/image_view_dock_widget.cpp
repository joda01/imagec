///
/// \file      image_view_dock_widget.cpp
/// \author    Joachim Danmayr
/// \date      2025-07-12
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "image_view_dock_widget.hpp"
#include <QCloseEvent>
#include "dialog_image_view.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
ImageViewDockWidget::ImageViewDockWidget(QMainWindow *windowMMain) : mWindowMain(windowMMain)
{
  setWindowTitle("Preview");
  setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  setFeatures(features() & ~QDockWidget::DockWidgetClosable);
  mImageViewWidget = new DialogImageViewer(windowMMain, nullptr, nullptr);
  setWidget(mImageViewWidget);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ImageViewDockWidget::closeEvent(QCloseEvent *event)
{
  event->ignore();    // Block the default close behavior

  // Optionally re-dock if floating
  if(isFloating()) {
    setFloating(false);
  }

  // Optionally just ensure it's visible again
  show();
}

}    // namespace joda::ui::gui
