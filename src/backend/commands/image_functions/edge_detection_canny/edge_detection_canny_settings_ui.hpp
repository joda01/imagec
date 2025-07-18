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
#include "edge_detection_canny_settings.hpp"

namespace joda::ui::gui {

class EdgeDetectionCanny : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Canny edge detection";
  inline static std::string ICON              = "triangle";
  inline static std::string DESCRIPTION       = "An edge detection algorithm which extracts the found edges to a binary image.";
  inline static std::vector<std::string> TAGS = {"edge detection", "edge"};

  EdgeDetectionCanny(joda::settings::PipelineStep &pipelineStep, settings::EdgeDetectionCannySettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::IMAGE, InOuts::BINARY}, {InOuts::BINARY}})
  {
    //
    //
    //
    mThresholdValueMin = SettingBase::create<SettingLineEdit<float>>(parent, generateSvgIcon("brightness-low"), "Min. threshold");
    mThresholdValueMin->setPlaceholderText("[0 - 255]");
    mThresholdValueMin->setUnit("");
    mThresholdValueMin->setMinMax(0, 255);
    mThresholdValueMin->setValue(settings.thresholdMin);
    mThresholdValueMin->connectWithSetting(&settings.thresholdMin);
    mThresholdValueMin->setShortDescription("Min. ");

    //
    //
    //
    mThresholdValueMax = SettingBase::create<SettingLineEdit<float>>(parent, generateSvgIcon("brightness-high"), "Max. threshold");
    mThresholdValueMax->setPlaceholderText("[0 - 255]");
    mThresholdValueMax->setUnit("");
    mThresholdValueMax->setMinMax(0, 255);
    mThresholdValueMax->setValue(settings.thresholdMax);
    mThresholdValueMax->connectWithSetting(&settings.thresholdMax);
    mThresholdValueMax->setShortDescription("Max. ");

    //
    //
    //
    mKernelSize = SettingBase::create<SettingComboBox<int32_t>>(parent, generateSvgIcon("labplot-matrix"), "Kernel size");
    mKernelSize->addOptions({{3, "3x3"}, {5, "5x5"}, {7, "7x7"}});
    mKernelSize->setValue(settings.kernelSize);
    mKernelSize->connectWithSetting(&settings.kernelSize);
    mKernelSize->setShortDescription("Kernel: ");

    addSetting({{mKernelSize.get(), true, 0}, {mThresholdValueMin.get(), true, 0}, {mThresholdValueMax.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<SettingLineEdit<float>> mThresholdValueMin;
  std::shared_ptr<SettingLineEdit<float>> mThresholdValueMax;
  std::shared_ptr<SettingComboBox<int>> mKernelSize;
};

}    // namespace joda::ui::gui
