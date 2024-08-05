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
#include "ui/helper/waitingspinnerwidget.hpp"
#include "ui/panel_preview.hpp"

namespace joda::ui::qt {

class WindowMain;
class ContainerChannel;

class PanelChannelEdit : public QWidget
{
  Q_OBJECT

signals:
  void updatePreviewStarted();
  void updatePreviewFinished();

public:
  PanelChannelEdit(WindowMain *wm, ContainerChannel *);
  ~PanelChannelEdit();
  void init();

  void setActive(bool setActive)
  {
    if(!mIsActiveShown && setActive) {
      mIsActiveShown = true;
      updatePreview();
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

  QHBoxLayout *createLayout();
  std::tuple<QVBoxLayout *, QWidget *> addVerticalPanel(QLayout *horizontalLayout, const QString &bgColor,
                                                        int margin = 16, bool enableScrolling = false,
                                                        int maxWidth = 250, int spacing = 4) const;

  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  ContainerChannel *mParentContainer;
  QLabel *createTitle(const QString &);
  PanelPreview *mPreviewImage = nullptr;
  std::mutex mPreviewMutex;
  int mPreviewCounter                         = 0;
  std::unique_ptr<std::thread> mPreviewThread = nullptr;
  bool mIsActiveShown                         = false;
  /////////////////////////////////////////////////////
  int32_t mSelectedTileX = 0;
  int32_t mSelectedTileY = 0;

private slots:
  /////////////////////////////////////////////////////
  void onChannelTypeChanged();
  void onDetectionModechanged();
  void updatePreview();
  void onTileClicked(int32_t tileX, int32_t tileY);
  void onPreviewStarted();
  void onPreviewFinished();
};

}    // namespace joda::ui::qt
