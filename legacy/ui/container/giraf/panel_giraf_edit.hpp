///
/// \file      panel_channel.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <qtmetamacros.h>
#include <QtWidgets>
#include <memory>
#include <mutex>
#include "ui/helper/waitingspinnerwidget.hpp"
#include "ui/panel_preview.hpp"

namespace joda::ui::qt {

class WindowMain;
class ContainerGiraf;

class PanelGirafEdit : public QWidget
{
  Q_OBJECT

signals:
  void updatePreviewStarted();
  void updatePreviewFinished();

public:
  PanelGirafEdit(WindowMain *wm, ContainerGiraf *);
  ~PanelGirafEdit();
  void init();

  void setActive(bool setActive)
  {
    if(!mIsActiveShown && setActive) {
      mIsActiveShown = true;
    }
    if(!setActive) {
      mIsActiveShown = false;
      std::lock_guard<std::mutex> lock(mPreviewMutex);
      mPreviewCounter = 0;
    }
  }

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t PREVIEW_BASE_SIZE = 450;

  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  ContainerGiraf *mParentContainer;
  QLabel *createTitle(const QString &);
  PanelPreview *mPreviewImage    = nullptr;
  WaitingSpinnerWidget *mSpinner = nullptr;
  std::mutex mPreviewMutex;
  int mPreviewCounter                         = 0;
  std::unique_ptr<std::thread> mPreviewThread = nullptr;
  bool mIsActiveShown                         = false;
};

}    // namespace joda::ui::qt
