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
#include "ui/container/command/command.hpp"
#include "ui/container/setting/setting_color_picker.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/helper/icon_generator.hpp"
#include "color_filter_settings.hpp"

namespace joda::ui {

class ColorFilter : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Color filter";
  inline static std::string ICON  = "color";

  ColorFilter(joda::settings::PipelineStep &pipelineStep, settings::ColorFilterSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent, {InOuts::IMAGE, InOuts::IMAGE})
  {
    if(settings.filter.empty()) {
      settings.filter.emplace_back(settings::ColorFilterSettings::Filter{});
    }

    //
    //
    mTargetColor = SettingBase::create<SettingColorPicker>(parent, generateIcon("color"), "Target color");
    mTargetColor->setValue({settings.filter.at(0).filterPointA, settings.filter.at(0).filterPointB, settings.filter.at(0).filterPointC});
    mTargetColor->connectWithSetting(&settings.filter.at(0).filterPointA, &settings.filter.at(0).filterPointB, &settings.filter.at(0).filterPointC);
    mTargetColor->setShortDescription("Color: ");

    //
    //
    mGrayscaleMode = SettingBase::create<SettingComboBox<joda::settings::ColorFilterSettings::GrayscaleMode>>(parent, generateIcon("contrast"),
                                                                                                              "Grayscale convert mode");
    mGrayscaleMode->addOptions({{joda::settings::ColorFilterSettings::GrayscaleMode::LINEAR, "Linear"},
                                {joda::settings::ColorFilterSettings::GrayscaleMode::HUMAN, "Human eye"}});
    mGrayscaleMode->setValue(settings.grayScaleConvertMode);
    mGrayscaleMode->connectWithSetting(&settings.grayScaleConvertMode);

    addSetting({{mTargetColor.get(), true, 0}, {mGrayscaleMode.get(), false, 0}});
  }

private:
  QColor pickColor(QWidget *parent, QColor initialColor)
  {
    // Open the color dialog and get the selected color
    return QColorDialog::getColor(initialColor, parent, "Select Color");
  }

  QColor adjustColor(const QColor &color, int delta)
  {
    int red   = qBound(0, color.red() + delta, 255);
    int green = qBound(0, color.green() + delta, 255);
    int blue  = qBound(0, color.blue() + delta, 255);

    return QColor(red, green, blue);
  }

  /////////////////////////////////////////////////////
  std::shared_ptr<SettingColorPicker> mTargetColor;
  std::unique_ptr<SettingComboBox<joda::settings::ColorFilterSettings::GrayscaleMode>> mGrayscaleMode;
};

}    // namespace joda::ui
