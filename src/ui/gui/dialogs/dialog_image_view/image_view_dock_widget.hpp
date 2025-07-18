///
/// \file      image_view_dock_widget.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-12
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qdockwidget.h>
#include <qtmetamacros.h>

namespace joda::ui::gui {

class DialogImageViewer;

///
/// \class      ImageViewDockWidget
/// \author     Joachim Danmayr
/// \brief
///
class ImageViewDockWidget : public QDockWidget
{
  Q_OBJECT

public:
  ImageViewDockWidget(QMainWindow *windowMMain);
  auto getImageWidget() -> DialogImageViewer *
  {
    return mImageViewWidget;
  }

private:
  void closeEvent(QCloseEvent *event) override;

  DialogImageViewer *mImageViewWidget;
  QMainWindow *mWindowMain;
};

}    // namespace joda::ui::gui
