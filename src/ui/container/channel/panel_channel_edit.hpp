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
#include "controller/controller.hpp"
#include "ui/container/panel_edit_base.hpp"
#include "ui/helper/waitingspinnerwidget.hpp"
#include "ui/panel_preview.hpp"

namespace joda::ui::qt {

class WindowMain;
class ContainerChannel;

class PanelChannelEdit : public PanelEdit
{
  Q_OBJECT

signals:
  void updatePreviewStarted();
  void updatePreviewFinished();

public:
  PanelChannelEdit(WindowMain *wm, ContainerChannel *);
  ~PanelChannelEdit();

  void setActive(bool setActive)
  {
    if(!mIsActiveShown && setActive) {
      mIsActiveShown = true;
      updatePreview(-1, -1);
    }
    if(!setActive) {
      mIsActiveShown = false;
      std::lock_guard<std::mutex> lock(mPreviewMutex);
      mPreviewCounter = 0;
      mPreviewImage->resetImage("");
    }
  }

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t PREVIEW_BASE_SIZE = 450;

  /////////////////////////////////////////////////////
  ContainerChannel *mParentContainer;
  PanelPreview *mPreviewImage = nullptr;
  std::mutex mPreviewMutex;
  int mPreviewCounter                         = 0;
  std::unique_ptr<std::thread> mPreviewThread = nullptr;
  bool mIsActiveShown                         = false;
  /////////////////////////////////////////////////////
  int32_t mSelectedTileX = 0;
  int32_t mSelectedTileY = 0;

  void valueChangedEvent() override;

private slots:
  /////////////////////////////////////////////////////
  void updatePreview(int32_t newImgIdex, int32_t selectedSeries);
  void onTileClicked(int32_t tileX, int32_t tileY);
  void onPreviewStarted();
  void onPreviewFinished();
};

}    // namespace joda::ui::qt
