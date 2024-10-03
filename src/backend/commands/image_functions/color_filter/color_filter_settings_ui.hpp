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
    mTargetColor = SettingBase::create<SettingLineEdit<std::string>>(parent, generateIcon("color"), "Target color");
    mTargetColor->setPlaceholderText("[#000000 - #FFFFFF]");
    mTargetColor->setUnit("");
    mTargetColor->setValue("#000000");
    mTargetColor->connectWithSetting(&settings.filter.at(0).targetColor);
    mTargetColor->setShortDescription("Color: ");
    auto *targetEdit = mTargetColor->getLineEdit();

    //
    //
    mLowerFilter = SettingBase::create<SettingLineEdit<std::string>>(parent, generateIcon("color"), "Lower color");
    mLowerFilter->setPlaceholderText("[#000000 - #FFFFFF]");
    mLowerFilter->setUnit("");
    mLowerFilter->setValue("#000000");
    mLowerFilter->connectWithSetting(&settings.filter.at(0).lowerColor);
    mLowerFilter->setShortDescription("Color: ");
    auto *lowerEdit = mLowerFilter->getLineEdit();
    connect(lowerEdit, &ClickableLineEdit::mousePressedEvent, [this, parent, lowerEdit]() {
      auto color = pickColor(parent, mLowerFilter->getValue().data());
      if(color.isValid()) {
        mLowerFilter->setValue(color.name().toStdString());
        QString colorStyle = QString("background-color: %1").arg(color.name());
        lowerEdit->setStyleSheet(colorStyle);
      }
    });

    //
    //
    mUpperFilter = SettingBase::create<SettingLineEdit<std::string>>(parent, generateIcon("color"), "Upper color");
    mUpperFilter->setPlaceholderText("[#000000 - #FFFFFF]");
    mUpperFilter->setUnit("");
    mUpperFilter->setValue("#000000");
    mUpperFilter->connectWithSetting(&settings.filter.at(0).upperColor);
    mUpperFilter->setShortDescription("Color: ");
    auto *upperEdit = mUpperFilter->getLineEdit();
    connect(upperEdit, &ClickableLineEdit::mousePressedEvent, [this, parent, upperEdit]() {
      auto color = pickColor(parent, mUpperFilter->getValue().data());
      if(color.isValid()) {
        mUpperFilter->setValue(color.name().toStdString());
        QString colorStyle = QString("background-color: %1").arg(color.name());
        upperEdit->setStyleSheet(colorStyle);
      }
    });

    connect(targetEdit, &ClickableLineEdit::mousePressedEvent, [this, parent, targetEdit, lowerEdit, upperEdit]() {
      auto color = pickColor(parent, QColor(mTargetColor->getValue().data()));
      if(color.isValid()) {
        mTargetColor->setValue(color.name().toStdString());
        QString colorStyle = QString("background-color: %1").arg(color.name());
        targetEdit->setStyleSheet(colorStyle);

        QColor colorUpper = adjustColor(color, 100);
        mUpperFilter->setValue(colorUpper.name().toStdString());
        QString colorUpperStyle = QString("background-color: %1").arg(colorUpper.name());
        upperEdit->setStyleSheet(colorUpperStyle);

        QColor colorLower = adjustColor(color, -100);
        mLowerFilter->setValue(colorLower.name().toStdString());
        QString colorLowerStyle = QString("background-color: %1").arg(colorLower.name());
        lowerEdit->setStyleSheet(colorLowerStyle);
      }
    });

    //
    //
    mGrayscaleMode = SettingBase::create<SettingComboBox<joda::settings::ColorFilterSettings::GrayscaleMode>>(parent, generateIcon("contrast"),
                                                                                                              "Grayscale convert mode");
    mGrayscaleMode->addOptions({{joda::settings::ColorFilterSettings::GrayscaleMode::LINEAR, "Linear"},
                                {joda::settings::ColorFilterSettings::GrayscaleMode::HUMAN, "Human eye"}});
    mGrayscaleMode->setValue(settings.grayScaleConvertMode);
    mGrayscaleMode->connectWithSetting(&settings.grayScaleConvertMode);

    addSetting({{mTargetColor.get(), true, 0}, {mLowerFilter.get(), false, 0}, {mUpperFilter.get(), false, 0}, {mGrayscaleMode.get(), false, 0}});
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
  std::shared_ptr<SettingLineEdit<std::string>> mTargetColor;
  std::shared_ptr<SettingLineEdit<std::string>> mLowerFilter;
  std::shared_ptr<SettingLineEdit<std::string>> mUpperFilter;
  std::unique_ptr<SettingComboBox<joda::settings::ColorFilterSettings::GrayscaleMode>> mGrayscaleMode;
};

}    // namespace joda::ui
