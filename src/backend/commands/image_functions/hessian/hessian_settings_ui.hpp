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

#include <qwidget.h>
#include "backend/commands/command.hpp"
#include "ui/gui/editor/widget_pipeline/widget_command/command.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "hessian_settings.hpp"

namespace joda::ui::gui {

class Hessian : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Hessian";
  inline static std::string ICON              = "brackets-square";
  inline static std::string DESCRIPTION       = "Local curvature of an image or function using its second-order derivatives.";
  inline static std::vector<std::string> TAGS = {"hessian", "eigenvalue", "coherence", "determinant", "local curvature"};

  Hessian(joda::settings::AnalyzeSettings *analyzeSettings, joda::settings::PipelineStep &pipelineStep, settings::HessianSettings &settings,
          QWidget *parent) :
      Command(analyzeSettings, pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent,
              {{InOuts::IMAGE, InOuts::IMAGE}, {InOuts::IMAGE}})
  {
    //
    //
    mMode = SettingBase::create<SettingComboBox<settings::HessianSettings::Mode>>(parent, {}, "Structure tensor mode");
    mMode->addOptions({{settings::HessianSettings::Mode::Determinant, "Determinant"},
                       {settings::HessianSettings::Mode::EigenvaluesX, "Eigenvalue X"},
                       {settings::HessianSettings::Mode::EigenvaluesY, "Eigenvalue Y"}});
    mMode->setValue(settings.mode);
    mMode->connectWithSetting(&settings.mode);

    addSetting({{mMode.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<settings::HessianSettings::Mode>> mMode;
};

}    // namespace joda::ui::gui
