///
/// \file      add_command_button.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qpushbutton.h>
#include <memory>
#include "backend/settings/pipeline/pipeline_step.hpp"
#include "panel_pipeline_settings.hpp"

namespace joda::ui {

template <class T>
concept command_t = ::std::is_base_of<joda::ui::Command, T>::value;

template <class T>
concept pipelinestep_t = ::std::is_base_of<joda::settings::PipelineStep, T>::value;

///
/// \class
/// \author
/// \brief
///
class AddCommandButtonBase : public QPushButton
{
public:
  AddCommandButtonBase();

public slots:
  void onAddCommandClicked();

private:
  virtual void addCommandClicked() = 0;
};

///
/// \class
/// \author
/// \brief
///
template <command_t CMD>
class AddCommandButton : public AddCommandButtonBase
{
public:
  /////////////////////////////////////////////////////
  AddCommandButton(QWidget *parent, PanelPipelineSettings &pipelinePanel, joda::settings::Pipeline &settings,
                   const settings::PipelineStep &step) :
      AddCommandButtonBase(),
      mParent(parent), mPipelineSettings(pipelinePanel), mSettings(settings), mStepTemplate(step)
  {
    const QIcon myIcon(":/icons/outlined/" + QString(CMD::ICON.data()));
    setIcon(myIcon);
    setText(CMD::TITLE.data());
  }

  std::shared_ptr<CMD> createCommand()
  {
    // mSettings.pipelineSteps.push_back(mStepTemplate);
    // auto &cmdSetting = mSettings.pipelineSteps.back();
    // return std::make_shared<CMD>(cmdSetting, mParent);
    return nullptr;
  }

private:
  void addCommandClicked() override
  {
    mPipelineSettings.addPipelineStep(createCommand());
  }

  /////////////////////////////////////////////////////
  QWidget *mParent;
  PanelPipelineSettings &mPipelineSettings;
  joda::settings::Pipeline &mSettings;
  settings::PipelineStep mStepTemplate;
};

}    // namespace joda::ui
