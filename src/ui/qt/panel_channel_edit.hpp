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
#include "helper/waitingspinnerwidget.hpp"
#include "ui/qt/panel_preview.hpp"

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
  void stopPreviewGeneration()
  {
    std::lock_guard<std::mutex> lock(mPreviewMutex);
    mPreviewCounter = 0;
  }

private:
  static constexpr int32_t PREVIEW_BASE_SIZE     = 450;
  static constexpr int32_t PLACEHOLDER_BASE_SIZE = 450;

  static inline const QString PLACEHOLDER{":/icons/outlined/icons8-picture-1000-lightgray.png"};

  QHBoxLayout *createLayout();
  std::tuple<QVBoxLayout *, QWidget *> addVerticalPanel(QLayout *horizontalLayout, const QString &bgColor,
                                                        int margin = 16, bool enableScrolling = false,
                                                        int maxWidth = 250) const;
  WindowMain *mWindowMain;
  ContainerChannel *mParentContainer;
  QLabel *createTitle(const QString &);
  QWidget *mScrollAreaCellApprox = nullptr;
  PreviewLabel *mPreviewImage    = nullptr;
  QLabel *mPreviewInfo           = nullptr;
  WaitingSpinnerWidget *mSpinner = nullptr;
  std::mutex mPreviewMutex;
  int mPreviewCounter;
  std::unique_ptr<std::thread> mPreviewThread = nullptr;

private slots:
  void onChannelTypeChanged();
  void onCellApproximationChanged();
  void onDetectionModechanged();
  void updatePreview();
};

}    // namespace joda::ui::qt
