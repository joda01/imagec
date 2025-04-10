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
#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "edge_detection_sobel_settings.hpp"

namespace joda::ui::gui {

class EdgeDetectionSobel : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Sobel edge detection";
  inline static std::string ICON              = "choice-rhomb";
  inline static std::string DESCRIPTION       = "An edge detection algorithms which enhances the edges.";
  inline static std::vector<std::string> TAGS = {"edge detection"};

  EdgeDetectionSobel(joda::settings::PipelineStep &pipelineStep, settings::EdgeDetectionSobelSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::IMAGE, InOuts::BINARY}, {InOuts::IMAGE}})
  {
    //
    //
    //
    mKernelSize = SettingBase::create<SettingComboBox<int32_t>>(parent, generateIcon("matrix"), "Kernel size");
    mKernelSize->addOptions({{-1, "ImageJ"},
                             {3, "3x3"},
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

    //
    // Options
    //
    mFunction = SettingBase::create<SettingComboBox<settings::EdgeDetectionSobelSettings::WeightFunction>>(parent, {}, "Weight function");
    mFunction->addOptions({
        {.key = settings::EdgeDetectionSobelSettings::WeightFunction::MAGNITUDE, .label = "Magnitude", .icon = generateIcon("square-root")},
        {.key = settings::EdgeDetectionSobelSettings::WeightFunction::ABS, .label = "Absolute value", .icon = generateIcon("upvote-downvote")},
    });
    mFunction->setValue(settings.weighFunction);
    mFunction->connectWithSetting(&settings.weighFunction);

    //
    //
    //
    mDerivativeOrderX = SettingBase::create<SettingLineEdit<uint16_t>>(parent, generateIcon("x-coordinate"), "Derivation order X");
    mDerivativeOrderX->setPlaceholderText("[0 - 255]");
    mDerivativeOrderX->setUnit("");
    mDerivativeOrderX->setMinMax(0, 255);
    mDerivativeOrderX->setValue(settings.derivativeOrderX);
    mDerivativeOrderX->connectWithSetting(&settings.derivativeOrderX);
    mDerivativeOrderX->setShortDescription("X ");

    //
    //
    //
    mDerivativeOrderY = SettingBase::create<SettingLineEdit<uint16_t>>(parent, generateIcon("y-coordinate"), "Derivation order Y");
    mDerivativeOrderY->setPlaceholderText("[0 - 255]");
    mDerivativeOrderY->setUnit("");
    mDerivativeOrderY->setMinMax(0, 255);
    mDerivativeOrderY->setValue(settings.derivativeOrderY);
    mDerivativeOrderY->connectWithSetting(&settings.derivativeOrderY);
    mDerivativeOrderY->setShortDescription("Y ");

    addSetting({
        {mKernelSize.get(), true, 0},
        {mFunction.get(), false, 0},
        {mDerivativeOrderX.get(), false, 0},
        {mDerivativeOrderY.get(), false, 0},
    });
  }

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<SettingComboBox<int32_t>> mKernelSize;
  std::unique_ptr<SettingComboBox<settings::EdgeDetectionSobelSettings::WeightFunction>> mFunction;
  std::shared_ptr<SettingLineEdit<uint16_t>> mDerivativeOrderX;
  std::shared_ptr<SettingLineEdit<uint16_t>> mDerivativeOrderY;
};

}    // namespace joda::ui::gui
