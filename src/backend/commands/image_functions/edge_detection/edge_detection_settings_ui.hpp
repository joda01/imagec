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

#include <qwidget.h>
#include "backend/commands/command.hpp"
#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "edge_detection_settings.hpp"

namespace joda::ui::gui {

class EdgeDetection : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Edge detection";
  inline static std::string ICON  = "triangle";

  EdgeDetection(joda::settings::PipelineStep &pipelineStep, settings::EdgeDetectionSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent, {{InOuts::IMAGE, InOuts::BINARY}, {InOuts::IMAGE}})
  {
    //
    //
    mEdgeDetectionMode =
        SettingBase::create<SettingComboBox<settings::EdgeDetectionSettings::Mode>>(parent, generateIcon("triangle"), "Edge detection mode");
    mEdgeDetectionMode->addOptions(
        {{settings::EdgeDetectionSettings::Mode::SOBEL, "Sobel"}, {settings::EdgeDetectionSettings::Mode::CANNY, "Canny"}});
    mEdgeDetectionMode->setValue(settings.mode);
    mEdgeDetectionMode->connectWithSetting(&settings.mode);

    //
    //
    //
    mThresholdValueMin = SettingBase::create<SettingLineEdit<uint16_t>>(parent, generateIcon("light-min"), "Min. threshold");
    mThresholdValueMin->setPlaceholderText("[0 - 255]");
    mThresholdValueMin->setUnit("");
    mThresholdValueMin->setMinMax(0, 255);
    mThresholdValueMin->setValue(settings.thresholdMin);
    mThresholdValueMin->connectWithSetting(&settings.thresholdMin);
    mThresholdValueMin->setShortDescription("Min. ");

    //
    //
    //
    mThresholdValueMax = SettingBase::create<SettingLineEdit<uint16_t>>(parent, generateIcon("light"), "Max. threshold");
    mThresholdValueMax->setPlaceholderText("[0 - 255]");
    mThresholdValueMax->setUnit("");
    mThresholdValueMax->setMinMax(0, 255);
    mThresholdValueMax->setValue(settings.thresholdMax);
    mThresholdValueMax->connectWithSetting(&settings.thresholdMax);
    mThresholdValueMax->setShortDescription("Max. ");

    //
    //
    //
    mKernelSize = SettingBase::create<SettingComboBox<int32_t>>(parent, generateIcon("matrix"), "Kernel size");
    mKernelSize->addOptions({{-1, "Off"},
                             {3, "3x3"},
                             {4, "4x4"},
                             {5, "5x5"},
                             {7, "7x7"},
                             {9, "9x9"},
                             {11, "11x11"},
                             {13, "13x13"},
                             {15, "15x15"},
                             {17, "17x17"},
                             {19, "19x19"},
                             {21, "21x21"},
                             {23, "23x23"}});
    mKernelSize->setValue(settings.kernelSize);
    mKernelSize->connectWithSetting(&settings.kernelSize);
    mKernelSize->setShortDescription("Kernel: ");

    addSetting({{mEdgeDetectionMode.get(), true, 0},
                {mThresholdValueMin.get(), false, 0},
                {mThresholdValueMax.get(), false, 0},
                {mKernelSize.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<settings::EdgeDetectionSettings::Mode>> mEdgeDetectionMode;

  std::shared_ptr<SettingLineEdit<uint16_t>> mThresholdValueMin;
  std::shared_ptr<SettingLineEdit<uint16_t>> mThresholdValueMax;
  std::shared_ptr<SettingComboBox<int>> mKernelSize;
};

}    // namespace joda::ui::gui
