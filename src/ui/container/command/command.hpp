///
/// \file      command.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qaction.h>
#include <qboxlayout.h>
#include <qdialog.h>
#include <qicon.h>
#include <qwidget.h>
#include <memory>
#include <thread>
#include "../setting/setting_base.hpp"
#include "backend/commands/command.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_combobox_multi.hpp"
#include "ui/helper/layout_generator.hpp"

namespace joda::ui {
enum class InOuts
{
  ALL,
  IMAGE,
  BINARY,
  OBJECT,
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
  Command(joda::settings::PipelineStep &pipelineStep, const QString &title, const QString &icon, QWidget *parent, InOut type);

  helper::TabWidget *addTab(const QString &title, std::function<void()> beforeTabClose);
  void removeAllTabsExceptFirst();
  void registerDeleteButton(PanelPipelineSettings *pipelineSettingsUi);
  void registerAddCommandButton(joda::settings::Pipeline &settings, PanelPipelineSettings *pipelineSettingsUi, WindowMain *mainWindow);
  void setCommandBefore(std::shared_ptr<Command> commandBefore)
  {
    mCommandBefore = commandBefore;
  }

  void addSetting(const std::vector<std::tuple<SettingBase *, bool, int32_t>> &settings)
  {
    addSetting(addTab("", [] {}), "", settings);
  }
  void addSetting(helper::TabWidget *tab, const std::vector<std::tuple<SettingBase *, bool, int32_t>> &settings)
  {
    addSetting(tab, "", settings);
  }
  helper::VerticalPane *addSetting(helper::TabWidget *tab, const QString &boxTitle,
                                   const std::vector<std::tuple<SettingBase *, bool, int32_t>> &settings, helper::VerticalPane *col = nullptr);

  [[nodiscard]] InOut getInOut() const
  {
    return mInOut;
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

    for(int m = mClasses.size() - 1; m >= 0; m--) {
      if(toRemove.contains(mClasses[m])) {
        mClasses.erase(mClasses.begin() + m);
      }
    }

    for(int m = mClassesMulti.size() - 1; m >= 0; m--) {
      if(toRemove.contains(mClassesMulti[m])) {
        mClassesMulti.erase(mClassesMulti.begin() + m);
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

  void openEditView()
  {
    mEditDialog->show();
    mEditDialog->setFixedSize(mEditDialog->sizeHint());      // Set size according to content
    mEditDialog->setMaximumSize(mEditDialog->sizeHint());    // Set size according to content
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

  void updateClassesAndClasssNames(const std::map<enums::ClassIdIn, QString> &classNames);

  const QString &getTitle()
  {
    return mTitle;
  }

  const QIcon &getIcon()
  {
    return mIcon;
  }

signals:
  void valueChanged();

protected:
  void updateClassesAndClasses();
  [[nodiscard]] bool isDisabled() const
  {
    return mDisabled->isChecked();
  }

  void setIsDisabled(bool disabled)
  {
    mDisabled->setChecked(disabled);
  }

private:
  /////////////////////////////////////////////////////

  ///
  /// \brief      Constructor
  /// \author     Joachim Danmayr
  ///
  void mousePressEvent(QMouseEvent *event) override
  {
    if(event->button() == Qt::LeftButton) {
      openEditView();
    }
  }
  void setDisabled(bool);
  void paintEvent(QPaintEvent *event) override;

  /////////////////////////////////////////////////////
  joda::settings::PipelineStep &mPipelineStep;
  QAction *mDisabled;
  QWidget *mParent;
  QString mTitle;
  QIcon mIcon;
  QWidget mEditView;
  helper::LayoutGenerator mLayout;
  QGridLayout mDisplayViewLayout;
  QDialog *mEditDialog;
  WrapLabel *mDisplayableText;
  std::vector<std::tuple<SettingBase *, bool, int32_t>> mSettings;
  std::vector<SettingComboBox<enums::ClassIdIn> *> mClasses;
  std::vector<SettingComboBoxMulti<enums::ClassIdIn> *> mClassesMulti;
  const InOut mInOut;
  std::shared_ptr<Command> mCommandBefore = nullptr;
protected slots:
  void updateDisplayText();
};

}    // namespace joda::ui
