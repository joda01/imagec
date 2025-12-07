///
/// \file      widget_video_control_button_group.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qactiongroup.h>
#include <qmenu.h>
#include <qobject.h>
#include <qspinbox.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qtoolbar.h>
#include <qwidget.h>
#include <functional>
#include <mutex>
#include "ui/gui/helper/debugging.hpp"
#include "ui/gui/helper/icon_generator.hpp"

namespace joda::ui::gui {

class VideoControlButtonGroup : public QObject
{
  Q_OBJECT

public:
  VideoControlButtonGroup(const std::function<void(void)> &onSettingsChanged, QToolBar *parentToolbar);
  void setMaxTimeStacks(int32_t maxTStacks);

public slots:
  void disableUiForPlayback(bool enabled);

  [[nodiscard]] int32_t getMaxTimeStacks() const;
  void setEnabled(bool enabled);
  void setVisible(bool visible);
  void setValue(int32_t val);
  auto value() -> int32_t
  {
    return mSpinnerActTimeStack->value();
  }
  [[nodiscard]] bool isVideoRunning() const
  {
    return mActionPlay->isChecked();
  }

private:
  // T-STACK //////////////////////////////////////////////////
  std::optional<int32_t> mMaxTimeStacks = std::nullopt;
  int32_t mPlaybackSpeed                = 1000;
  QActionGroup *mPlaybackspeedGroup     = nullptr;
  QMenu *mPlaybackSpeedSelector         = nullptr;
  QTimer *mPlayTimer                    = nullptr;
  QAction *mActionPlay                  = nullptr;
  QAction *mSeekBack                    = nullptr;
  QAction *mSeekForward                 = nullptr;
  QSpinBox *mSpinnerActTimeStack        = nullptr;
  QAction *mSpinnerActTimeStackAction   = nullptr;
  bool mPlaybackToolbarVisible          = false;
  int32_t mTempMaxTimeStacks            = 0;

  std::function<void(void)> mCallback;
  std::mutex mSetMutex;
  QToolBar *mParentToolbar = nullptr;
};

}    // namespace joda::ui::gui
