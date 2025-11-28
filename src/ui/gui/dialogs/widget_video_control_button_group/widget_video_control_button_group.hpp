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

class VideoControlButtonGroup
{
public:
  VideoControlButtonGroup(const std::function<void(void)> &onSettingsChanged, QToolBar *parentToolbar) :
      mCallback(onSettingsChanged), mParentToolbar(parentToolbar)
  {
    // Video timer
    mPlayTimer = new QTimer();
    QObject::connect(mPlayTimer, &QTimer::timeout, [this] {
      if(mSpinnerActTimeStack->value() < getMaxTimeStacks()) {
        mSpinnerActTimeStack->blockSignals(true);
        mSpinnerActTimeStack->setValue(mSpinnerActTimeStack->value() + 1);
        mSpinnerActTimeStack->blockSignals(false);
      } else {
        mSpinnerActTimeStack->blockSignals(true);
        mSpinnerActTimeStack->setValue(0);
        mSpinnerActTimeStack->blockSignals(false);
      }
      mCallback();
    });

    mSpinnerActTimeStack = new QSpinBox();
    mSpinnerActTimeStack->setValue(0);
    mSpinnerActTimeStack->setMinimumWidth(65);
    QWidget::connect(mSpinnerActTimeStack, &QSpinBox::valueChanged, [this] { mCallback(); });

    mSeekBack = new QAction(generateSvgIcon<Style::REGULAR, Color::BLUE>("skip-back"), "Backward");
    QWidget::connect(mSeekBack, &QAction::triggered, [this] {
      if(mSpinnerActTimeStack->value() > 0) {
        mSpinnerActTimeStack->blockSignals(true);
        mSpinnerActTimeStack->setValue(mSpinnerActTimeStack->value() - 1);
        mSpinnerActTimeStack->blockSignals(false);
      }
      mCallback();
    });
    parentToolbar->addAction(mSeekBack);

    // ==========================================================

    mPlaybackSpeedSelector     = new QMenu();
    mPlaybackspeedGroup        = new QActionGroup(mPlaybackSpeedSelector);
    auto addPlaybackSpeedLamda = [this](QAction *action, int32_t timeMs, bool checked = false) {
      mPlaybackspeedGroup->addAction(action);
      action->setCheckable(true);
      action->setChecked(checked);
      QWidget::connect(action, &QAction::triggered, [timeMs, this]() {
        mPlaybackSpeed = timeMs;
        if(mActionPlay->isChecked()) {
          mPlayTimer->stop();
          mPlayTimer->start(mPlaybackSpeed);
        }
      });
    };

    addPlaybackSpeedLamda(mPlaybackSpeedSelector->addAction("25 Hz"), 40);
    addPlaybackSpeedLamda(mPlaybackSpeedSelector->addAction("20 Hz"), 50);
    addPlaybackSpeedLamda(mPlaybackSpeedSelector->addAction("10 Hz"), 100);
    addPlaybackSpeedLamda(mPlaybackSpeedSelector->addAction("5 Hz"), 200);
    addPlaybackSpeedLamda(mPlaybackSpeedSelector->addAction("4 Hz"), 250);
    addPlaybackSpeedLamda(mPlaybackSpeedSelector->addAction("3 Hz"), 333);
    addPlaybackSpeedLamda(mPlaybackSpeedSelector->addAction("2 Hz"), 500);
    addPlaybackSpeedLamda(mPlaybackSpeedSelector->addAction("1 Hz"), 1000, true);
    addPlaybackSpeedLamda(mPlaybackSpeedSelector->addAction("0.5 Hz"), 2000);

    mActionPlay = new QAction(generateSvgIcon<Style::REGULAR, Color::BLUE>("play"), "Play/Stop");
    mActionPlay->setMenu(mPlaybackSpeedSelector);
    mActionPlay->setCheckable(true);
    QWidget::connect(mActionPlay, &QAction::triggered, [this](bool selected) {
      if(selected) {
        mPlayTimer->start(mPlaybackSpeed);
      } else {
        mPlayTimer->stop();
      }
    });

    parentToolbar->addAction(mActionPlay);
    mSpinnerActTimeStackAction = parentToolbar->addWidget(mSpinnerActTimeStack);

    mSeekForward = new QAction(generateSvgIcon<Style::REGULAR, Color::BLUE>("skip-forward"), "Forward");
    QWidget::connect(mSeekForward, &QAction::triggered, [this] {
      if(mSpinnerActTimeStack->value() < getMaxTimeStacks()) {
        mSpinnerActTimeStack->blockSignals(true);
        mSpinnerActTimeStack->setValue(mSpinnerActTimeStack->value() + 1);
        mSpinnerActTimeStack->blockSignals(false);
      }
      mCallback();
    });
    parentToolbar->addAction(mSeekForward);
  }

  void setMaxTimeStacks(int32_t maxTStacks)
  {
    if(nullptr == mSeekForward) {
      return;
    }
    std::lock_guard<std::mutex> lock(mSetMutex);
    mTempMaxTimeStacks = maxTStacks;
    mSpinnerActTimeStack->setMaximum(maxTStacks);
    if(mTempMaxTimeStacks <= 1) {
      if(mSpinnerActTimeStack->value() > 0) {
        mSpinnerActTimeStack->setValue(0);
      }
      if(mSeekForward->isEnabled()) {
        QMetaObject::invokeMethod(
            mParentToolbar,    // or mActionPlay, or any QObject in the GUI thread
            [this]() {
              if(nullptr == mSeekForward) {
                return;
              }
              CHECK_GUI_THREAD(mSeekForward)
              mSeekForward->setEnabled(false);
              CHECK_GUI_THREAD(mSeekBack)
              mSeekBack->setEnabled(false);
              mActionPlay->setChecked(false);
              CHECK_GUI_THREAD(mActionPlay)
              mActionPlay->setEnabled(false);
              CHECK_GUI_THREAD(mSpinnerActTimeStack)
              mSpinnerActTimeStack->setEnabled(false);
              mPlayTimer->stop();
            },
            Qt::QueuedConnection);
      }
    } else {
      if(!mSeekForward->isEnabled()) {
        QMetaObject::invokeMethod(
            mParentToolbar,    // or mActionPlay, or any QObject in the GUI thread
            [this]() {
              if(nullptr == mSeekForward) {
                return;
              }
              CHECK_GUI_THREAD(mSeekForward)
              mSeekForward->setEnabled(true);
              CHECK_GUI_THREAD(mSeekBack)
              mSeekBack->setEnabled(true);
              CHECK_GUI_THREAD(mActionPlay)
              mActionPlay->setEnabled(true);
              CHECK_GUI_THREAD(mSpinnerActTimeStack)
              mSpinnerActTimeStack->setEnabled(true);
            },
            Qt::QueuedConnection);
      }
    }
  }
  [[nodiscard]] int32_t getMaxTimeStacks() const
  {
    return mTempMaxTimeStacks;
  }

  void setEnabled(bool enabled)
  {
    CHECK_GUI_THREAD(mActionPlay)
    mActionPlay->setEnabled(enabled);
    CHECK_GUI_THREAD(mSeekBack)
    mSeekBack->setEnabled(enabled);
    CHECK_GUI_THREAD(mSeekForward)
    mSeekForward->setEnabled(enabled);
    CHECK_GUI_THREAD(mSpinnerActTimeStackAction)
    mSpinnerActTimeStackAction->setEnabled(enabled);
  }

  void setVisible(bool visible)
  {
    CHECK_GUI_THREAD(mActionPlay)
    mActionPlay->setVisible(visible);
    CHECK_GUI_THREAD(mSeekBack)
    mSeekBack->setVisible(visible);
    CHECK_GUI_THREAD(mSeekForward)
    mSeekForward->setVisible(visible);
    CHECK_GUI_THREAD(mSpinnerActTimeStackAction)
    mSpinnerActTimeStackAction->setVisible(visible);
  }

  void setValue(int32_t val)
  {
    mSpinnerActTimeStack->blockSignals(true);
    mSpinnerActTimeStackAction->blockSignals(true);
    mSpinnerActTimeStack->setValue(val);
    mSpinnerActTimeStack->blockSignals(false);
    mSpinnerActTimeStackAction->blockSignals(false);
  }

  auto value() -> int32_t
  {
    return mSpinnerActTimeStack->value();
  }

  bool isVideoRunning() const
  {
    return mActionPlay->isChecked();
  }

  void setPlay(bool play)
  {
    mActionPlay->setChecked(play);
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
