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

private:
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

private slots:
  void onChannelTypeChanged();
  void onCellApproximationChanged();
  void onDetectionModechanged();
  void updatePreview();
};

}    // namespace joda::ui::qt
