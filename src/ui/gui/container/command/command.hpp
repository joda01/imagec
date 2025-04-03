///
/// \file      command.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <qaction.h>
#include <qboxlayout.h>
#include <qdialog.h>
#include <qicon.h>
#include <qnamespace.h>
#include <qwidget.h>
#include <memory>
#include <thread>
#include <vector>
#include "../setting/setting_base.hpp"
#include "backend/commands/command.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_combobox_multi.hpp"
#include "ui/gui/helper/layout_generator.hpp"

namespace joda::ui::gui {

class DialogCommandSelection;
class AddCommandButtonBase;

enum class InOuts
{
  ALL,
  IMAGE,
  BINARY,
  OBJECT,
  OUTPUT_EQUAL_TO_INPUT
};

struct InOut
{
  std::set<InOuts> in = {InOuts::ALL};
  InOuts out          = InOuts::ALL;
};

class WrapLabel : public QLabel
{
public:
  WrapLabel(InOut inout);
  void resizeEvent(QResizeEvent *event) override;

private:
  InOut inout;
};

class PanelPipelineSettings;

using namespace std::chrono_literals;

class Command : public QWidget
{
  Q_OBJECT
public:
  /////////////////////////////////////////////////////
  Command(joda::settings::PipelineStep &pipelineStep, const QString &title, const QString &description, const std::vector<std::string> &tags,
          const QString &icon, QWidget *parent, InOut type);

  helper::TabWidget *addTab(const QString &title, std::function<void()> beforeTabClose, bool showCloseButton);
  void removeTab(int32_t idx);
  void removeAllTabsExceptFirst();
  void registerDeleteButton(PanelPipelineSettings *pipelineSettingsUi);
  void registerAddCommandButton(std::shared_ptr<Command> commandBefore, std::shared_ptr<DialogCommandSelection> &cmdDialog,
                                joda::settings::Pipeline &settings, PanelPipelineSettings *pipelineSettingsUi, WindowMain *mainWindow);
  void setCommandBefore(std::shared_ptr<Command> commandBefore);
  void addSetting(const std::vector<std::tuple<SettingBase *, bool, int32_t>> &settings, bool showCloseButton = false)
  {
    addSetting(addTab(
                   "", [] {}, showCloseButton),
               "", settings);
  }
  void addSetting(helper::TabWidget *tab, const std::vector<std::tuple<SettingBase *, bool, int32_t>> &settings)
  {
    addSetting(tab, "", settings);
  }
  helper::VerticalPane *addSetting(helper::TabWidget *tab, const QString &boxTitle,
                                   const std::vector<std::tuple<SettingBase *, bool, int32_t>> &settings, helper::VerticalPane *col = nullptr);

  helper::VerticalPane *addWidgets(helper::TabWidget *tab, const QString &boxTitle, const std::vector<QWidget *> &settings,
                                   helper::VerticalPane *col = nullptr);

  [[nodiscard]] InOut getInOut() const
  {
    return mInOut;
  }

  [[nodiscard]] InOuts getResolvedInput() const
  {
    if(mCommandBefore != nullptr) {
      auto outTmp = mCommandBefore->getInOut().out;
      auto iter   = mInOut.in.find(outTmp);
      if(iter != mInOut.in.end()) {
        return *iter;
      }
    }
    return *mInOut.in.begin();
  }

  void blockComponentSignals(bool bl)
  {
    QWidget::blockSignals(bl);
    for(auto &setting : mSettings) {
      std::get<0>(setting)->blockAllSignals(bl);
    }
  }

  void removeSetting(const std::set<SettingBase *> &toRemove)
  {
    for(int m = mSettings.size() - 1; m >= 0; m--) {
      if(toRemove.contains(std::get<0>(mSettings[m]))) {
        mSettings.erase(mSettings.begin() + m);
      }
    }
  }

  auto getDisplayWidget() const -> const QWidget *
  {
    return this;
  }

  auto getEditWidget() const -> const QWidget *
  {
    return &mEditView;
  }

  auto mutableEditDialog() const -> QWidget *
  {
    return mEditDialog;
  }

  void openEditView()
  {
    mEditDialog->show();
    // mEditDialog->setFixedSize(mEditDialog->sizeHint());      // Set size according to content
    // mEditDialog->setMaximumSize(mEditDialog->sizeHint());    // Set size according to content
    mEditDialog->setMinimumSize(mEditDialog->sizeHint());    // Set size according to content
  }

  void addSeparatorToTopToolbar()
  {
    mLayout.addSeparatorToTopToolbar();
  }
  QAction *addItemToTopToolbar(QWidget *widget)
  {
    return mLayout.addItemToTopToolbar(widget);
  }
  QAction *addActionButton(const QString &text, const QIcon &icon)
  {
    return mLayout.addActionButton(text, icon);
  }

  const QString &getTitle() const
  {
    return mTitle;
  }

  const std::vector<std::string> &getTags() const
  {
    return mTags;
  }

  const QString &getDescription() const
  {
    return mDescription;
  }

  const QIcon &getIcon() const
  {
    return mIcon;
  }

signals:
  void valueChanged();
  void displayTextChanged();

public slots:
  void updateDisplayText();

protected:
  [[nodiscard]] bool isDisabled() const
  {
    return mDisabled->isChecked();
  }

  void setIsDisabled(bool disabled)
  {
    mDisabled->setChecked(disabled);
  }

  [[nodiscard]] bool isLocked() const
  {
    return mLocked->isChecked();
  }

  void setIsLocked(bool locked)
  {
    mLocked->setChecked(locked);
  }

  void setIsBreakpoint(bool breakPoint)
  {
    mBreakpoint->setCheckable(breakPoint);
  }

  [[nodiscard]] bool isBreakpoint() const
  {
    return mBreakpoint->isChecked();
  }

  InOuts getOut() const;

private:
  /////////////////////////////////////////////////////

  ///
  /// \brief      Constructor
  /// \author     Joachim Danmayr
  ///
  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void setDisabled(bool);
  void setLocked(bool);
  void setBreakpoint(bool);
  void paintEvent(QPaintEvent *event) override;
  void setDisplayTextFont();

  /////////////////////////////////////////////////////
  joda::settings::PipelineStep &mPipelineStep;
  QAction *mDisabled;
  QAction *mLocked;
  QAction *mBreakpoint;

  QWidget *mParent;
  QString mTitle;
  QString mDescription;
  QIcon mIcon;
  QWidget mEditView;
  helper::LayoutGenerator mLayout;
  QGridLayout mDisplayViewLayout;
  QDialog *mEditDialog;
  WrapLabel *mDisplayableText;
  std::vector<std::tuple<SettingBase *, bool, int32_t>> mSettings;
  const InOut mInOut;
  std::shared_ptr<Command> mCommandBefore = nullptr;
  const std::vector<std::string> &mTags;
  AddCommandButtonBase *mCmdButton;
};

}    // namespace joda::ui::gui
