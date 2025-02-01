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

#include <qspinbox.h>
#include <qwidget.h>
#include <memory>
#include <string>
#include "ui/container/command/command.hpp"
#include "ui/container/setting/setting_base.hpp"
#include "ui/container/setting/setting_color_picker.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/container/setting/setting_spinbox.hpp"
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
      Command(pipelineStep, TITLE.data(), ICON.data(), parent, {{InOuts::IMAGE}, {InOuts::IMAGE}})
  {
    if(settings.filter.empty()) {
      settings.filter.emplace_back(settings::ColorFilterSettings::Filter{});
    }

    //
    //
    mTargetColor = SettingBase::create<SettingColorPicker>(parent, generateIcon("color"), "");
    mTargetColor->setValue({settings.filter.begin()->colorRangeFrom, settings.filter.begin()->colorRangeTo});
    mTargetColor->setShortDescription("Color: ");

    auto [mPointA, mPointB] = mTargetColor->getValue();

    auto createSpinBox = [&](const std::string &desc, uint32_t max, uint32_t value) -> std::shared_ptr<SettingSpinBox<uint32_t>> {
      auto box = SettingBase::create<SettingSpinBox<uint32_t>>(parent, generateIcon(""), desc.data());
      box->setMinMax(0, max);
      box->setValue(value);
      connect(box.get(), &SettingBase::valueChanged, this, &ColorFilter::spinnerToColorPicker);
      return box;
    };

    //
    // Spinner boxes
    //
    mPointAHue = createSpinBox("Color from", 359, settings.filter.begin()->colorRangeFrom.hue);
    mPointASat = createSpinBox("Saturation from", 255, settings.filter.begin()->colorRangeFrom.sat);
    mPointAVal = createSpinBox("Value from", 255, settings.filter.begin()->colorRangeFrom.val);

    mPointBHue = createSpinBox("Color to", 359, settings.filter.begin()->colorRangeTo.hue);
    mPointBSat = createSpinBox("Saturation to", 255, settings.filter.begin()->colorRangeTo.sat);
    mPointBVal = createSpinBox("Value to", 255, settings.filter.begin()->colorRangeTo.val);

    //
    //
    mGrayscaleMode = SettingBase::create<SettingComboBox<joda::settings::ColorFilterSettings::GrayscaleMode>>(parent, generateIcon("contrast"),
                                                                                                              "Grayscale convert mode");
    mGrayscaleMode->addOptions({{joda::settings::ColorFilterSettings::GrayscaleMode::LINEAR, "Linear"},
                                {joda::settings::ColorFilterSettings::GrayscaleMode::HUMAN, "Human eye"}});
    mGrayscaleMode->setValue(settings.grayScaleConvertMode);
    mGrayscaleMode->connectWithSetting(&settings.grayScaleConvertMode);

    auto *tab = addTab(
        "", [] {}, false);

    addSetting(tab, "Color chooser",
               {
                   {mTargetColor.get(), true, 0},
                   {mGrayscaleMode.get(), false, 0},
               });

    addSetting(tab, "Fine tuning",
               {
                   {mPointAHue.get(), false, 0},
                   {mPointBHue.get(), false, 0},

                   {mPointASat.get(), false, 0},
                   {mPointBSat.get(), false, 0},

                   {mPointAVal.get(), false, 0},
                   {mPointBVal.get(), false, 0},
               });

    connect(mTargetColor.get(), &SettingBase::valueChanged, this, &ColorFilter::colorPickerToSpinner);
    colorPickerToSpinner();
    mTargetColor->connectWithSetting(&settings.filter.begin()->colorRangeFrom, &settings.filter.begin()->colorRangeTo);
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
  std::shared_ptr<SettingSpinBox<uint32_t>> mPointAHue;
  std::shared_ptr<SettingSpinBox<uint32_t>> mPointASat;
  std::shared_ptr<SettingSpinBox<uint32_t>> mPointAVal;
  std::shared_ptr<SettingSpinBox<uint32_t>> mPointBHue;
  std::shared_ptr<SettingSpinBox<uint32_t>> mPointBSat;
  std::shared_ptr<SettingSpinBox<uint32_t>> mPointBVal;

  std::shared_ptr<SettingColorPicker> mTargetColor;
  std::unique_ptr<SettingComboBox<joda::settings::ColorFilterSettings::GrayscaleMode>> mGrayscaleMode;

private slots:
  void colorPickerToSpinner()
  {
    mPointAHue->blockAllSignals(true);
    mPointASat->blockAllSignals(true);
    mPointAVal->blockAllSignals(true);

    mPointBHue->blockAllSignals(true);
    mPointBSat->blockAllSignals(true);
    mPointBVal->blockAllSignals(true);

    auto [mPointA, mPointB] = mTargetColor->getValue();

    mPointAHue->setValue(static_cast<uint32_t>(mPointA.hue));
    mPointASat->setValue(static_cast<uint32_t>(mPointA.sat));
    mPointAVal->setValue(static_cast<uint32_t>(mPointA.val));

    mPointBHue->setValue(static_cast<uint32_t>(mPointB.hue));
    mPointBSat->setValue(static_cast<uint32_t>(mPointB.sat));
    mPointBVal->setValue(static_cast<uint32_t>(mPointB.val));

    mPointAHue->blockAllSignals(false);
    mPointASat->blockAllSignals(false);
    mPointAVal->blockAllSignals(false);

    mPointBHue->blockAllSignals(false);
    mPointBSat->blockAllSignals(false);
    mPointBVal->blockAllSignals(false);
  }
  void spinnerToColorPicker()
  {
    mTargetColor->blockAllSignals(true);

    mTargetColor->setValue(
        {joda::enums::HsvColor{(int32_t) mPointAHue->getValue(), (int32_t) mPointASat->getValue(), (int32_t) mPointAVal->getValue()},
         joda::enums::HsvColor{(int32_t) mPointBHue->getValue(), (int32_t) mPointBSat->getValue(), (int32_t) mPointBVal->getValue()}});

    mTargetColor->blockAllSignals(false);
  }
};

}    // namespace joda::ui
