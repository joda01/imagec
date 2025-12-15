///
/// \file      widget_video_control_button_group.cpp
/// \author    Joachim Danmayr
/// \date      2025-12-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "widget_video_control_button_group.hpp"
#include <QWidget>

namespace joda::ui::gui {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
VideoControlButtonGroup::VideoControlButtonGroup(const std::function<void(void)> &onSettingsChanged, QToolBar *parentToolbar) :
    mCallback(onSettingsChanged), mParentToolbar(parentToolbar)
{
  // Video timer
  mPlayTimer = new QTimer();
  QObject::connect(mPlayTimer, &QTimer::timeout, [this] {
    if(getMaxTimeStacks() <= 1) {
      mPlayTimer->stop();
      mActionPlay->setChecked(false);
    } else {
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
    }
  });

  mSpinnerActTimeStack = new QSpinBox();
  mSpinnerActTimeStack->setValue(0);
  mSpinnerActTimeStack->setMinimumWidth(65);
  connect(mSpinnerActTimeStack, &QSpinBox::valueChanged, [this] { mCallback(); });

  mSeekBack = new QAction(generateSvgIcon<Style::REGULAR, Color::BLUE>("skip-back"), "Backward");
  connect(mSeekBack, &QAction::triggered, [this] {
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
    connect(action, &QAction::triggered, [timeMs, this]() {
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
  connect(mActionPlay, &QAction::triggered, [this](bool selected) {
    if(selected) {
      mPlayTimer->start(mPlaybackSpeed);
    } else {
      mPlayTimer->stop();
    }
  });

  parentToolbar->addAction(mActionPlay);
  mSpinnerActTimeStackAction = parentToolbar->addWidget(mSpinnerActTimeStack);

  mSeekForward = new QAction(generateSvgIcon<Style::REGULAR, Color::BLUE>("skip-forward"), "Forward");
  connect(mSeekForward, &QAction::triggered, [this] {
    if(mSpinnerActTimeStack->value() < getMaxTimeStacks()) {
      mSpinnerActTimeStack->blockSignals(true);
      mSpinnerActTimeStack->setValue(mSpinnerActTimeStack->value() + 1);
      mSpinnerActTimeStack->blockSignals(false);
    }
    mCallback();
  });
  parentToolbar->addAction(mSeekForward);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void VideoControlButtonGroup::setMaxTimeStacks(int32_t maxTStacks)
{
  if(nullptr == mSeekForward) {
    return;
  }
  std::lock_guard<std::mutex> lock(mSetMutex);
  mTempMaxTimeStacks = maxTStacks;

  QMetaObject::invokeMethod(
      mSpinnerActTimeStack,
      [this, maxTStacks] {
        if(mSpinnerActTimeStack == nullptr) {
          return;
        }
        CHECK_GUI_THREAD(mSpinnerActTimeStack)
        mSpinnerActTimeStack->setMaximum(maxTStacks);
      },
      Qt::QueuedConnection);

  if(mTempMaxTimeStacks <= 1) {
    if(mSpinnerActTimeStack->value() > 0) {
      QMetaObject::invokeMethod(
          mSpinnerActTimeStack,
          [this] {
            if(mSpinnerActTimeStack == nullptr) {
              return;
            }
            CHECK_GUI_THREAD(mSpinnerActTimeStack)
            mSpinnerActTimeStack->setValue(0);
          },
          Qt::QueuedConnection);
    }
    if(mSeekForward->isEnabled()) {
      QMetaObject::invokeMethod(this, "disableUiForPlayback", Qt::QueuedConnection, Q_ARG(bool, false));
    }
  } else {
    if(!mSeekForward->isEnabled()) {
      QMetaObject::invokeMethod(this, "disableUiForPlayback", Qt::QueuedConnection, Q_ARG(bool, true));
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void VideoControlButtonGroup::disableUiForPlayback(bool enabled)
{
  if((mSeekForward == nullptr) || (mSeekBack == nullptr) || (mActionPlay == nullptr) || (mSpinnerActTimeStack == nullptr)) {
    return;
  }
  CHECK_GUI_THREAD(mSeekForward)
  mSeekForward->setEnabled(enabled);
  CHECK_GUI_THREAD(mSeekBack)
  mSeekBack->setEnabled(enabled);
  CHECK_GUI_THREAD(mActionPlay)
  mActionPlay->setEnabled(enabled);
  CHECK_GUI_THREAD(mSpinnerActTimeStack)
  mSpinnerActTimeStack->setEnabled(enabled);
  if(!enabled) {
    mActionPlay->setChecked(enabled);
    mPlayTimer->stop();
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void VideoControlButtonGroup::setEnabled(bool enabled)
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

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void VideoControlButtonGroup::setVisible(bool visible)
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

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void VideoControlButtonGroup::setValue(int32_t val)
{
  mSpinnerActTimeStack->blockSignals(true);
  mSpinnerActTimeStackAction->blockSignals(true);
  mSpinnerActTimeStack->setValue(val);
  mSpinnerActTimeStack->blockSignals(false);
  mSpinnerActTimeStackAction->blockSignals(false);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
int32_t VideoControlButtonGroup::getMaxTimeStacks() const
{
  return mTempMaxTimeStacks;
}
}    // namespace joda::ui::gui
