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
#include "panel_intersection_overview.hpp"

namespace joda::ui::qt {

class WindowMain;
class ContainerIntersection;

class PanelIntersectionEdit : public QWidget
{
  Q_OBJECT

signals:
  void updatePreviewStarted();
  void updatePreviewFinished();

public:
  PanelIntersectionEdit(WindowMain *wm, ContainerIntersection *);
  ~PanelIntersectionEdit();

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
    }
  }

private:
  static constexpr int32_t PREVIEW_BASE_SIZE = 450;

  QHBoxLayout *createLayout();
  std::tuple<QVBoxLayout *, QWidget *> addVerticalPanel(QLayout *horizontalLayout, const QString &bgColor,
                                                        int margin = 16, bool enableScrolling = false,
                                                        int maxWidth = 250) const;
  WindowMain *mWindowMain;
  ContainerIntersection *mParentContainer;
  QLabel *createTitle(const QString &);
  PanelPreview *mPreviewImage    = nullptr;
  WaitingSpinnerWidget *mSpinner = nullptr;
  std::mutex mPreviewMutex;
  int mPreviewCounter                         = 0;
  std::unique_ptr<std::thread> mPreviewThread = nullptr;
  bool mIsActiveShown                         = false;

private slots:
  void onChannelTypeChanged();
  void onCellApproximationChanged();
  void updatePreview();
  void onEditMeasurementClicked();
};

}    // namespace joda::ui::qt
