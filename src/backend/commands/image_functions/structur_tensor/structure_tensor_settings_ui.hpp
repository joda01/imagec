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
#include "structure_tensor_settings.hpp"

namespace joda::ui::gui {

class StructureTensor : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Structure Tensor";
  inline static std::string ICON              = "brackets-angle";
  inline static std::string DESCRIPTION       = "Calculates the structure tensor of the image";
  inline static std::vector<std::string> TAGS = {"structure tensor", "eigenvalue", "coherence"};

  StructureTensor(joda::settings::AnalyzeSettings *analyzeSettings, joda::settings::PipelineStep &pipelineStep,
                  settings::StructureTensorSettings &settings, QWidget *parent) :
      Command(analyzeSettings, pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent,
              {{InOuts::IMAGE, InOuts::IMAGE}, {InOuts::IMAGE}})
  {
    //
    //
    mMode = SettingBase::create<SettingComboBox<settings::StructureTensorSettings::Mode>>(parent, {}, "Structure tensor mode");
    mMode->addOptions({{settings::StructureTensorSettings::Mode::Coherence, "Coherence"},
                       {settings::StructureTensorSettings::Mode::EigenvaluesX, "Eigenvalue X"},
                       {settings::StructureTensorSettings::Mode::EigenvaluesY, "Eigenvalue Y"}});
    mMode->setValue(settings.mode);
    mMode->connectWithSetting(&settings.mode);

    //
    //
    //
    mKernelSize = SettingBase::create<SettingComboBox<int32_t>>(parent, {}, "Kernel size");
    mKernelSize->addOptions({{-1, "Off"},
                             {3, "3x3 (σ=0.3)"},
                             {5, "5x5 (σ=0.7)"},
                             {7, "7x7 (σ=1.0)"},
                             {9, "9x9 (σ=1.3)"},
                             {11, "11x11 (σ=1.6)"},
                             {13, "13x13 (σ=2.0)"},
                             {15, "15x15 (σ=2.3)"},
                             {17, "17x17 (σ=2.6)"},
                             {19, "19x19 (σ=3.0)"},
                             {21, "21x21 (σ=3.3)"},
                             {23, "23x23 (σ=3.6)"}});
    mKernelSize->setValue(settings.kernelSize);
    mKernelSize->connectWithSetting(&settings.kernelSize);
    mKernelSize->setShortDescription("Kernel: ");

    addSetting({{mMode.get(), true, 0}, {mKernelSize.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<settings::StructureTensorSettings::Mode>> mMode;
  std::shared_ptr<SettingComboBox<int>> mKernelSize;
};

}    // namespace joda::ui::gui
