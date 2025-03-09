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
#include <qwidget.h>
#include <cstdint>
#include "backend/commands/command.hpp"
#include "backend/enums/enums_classes.hpp"
#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/setting/setting_base.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_combobox_classes_out.hpp"
#include "ui/gui/container/setting/setting_combobox_classification_in.hpp"
#include "ui/gui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "enhance_contrast_settings.hpp"

namespace joda::ui::gui {

class EnhanceContrast : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Enhance contrast";
  inline static std::string ICON              = "contrast";
  inline static std::string DESCRIPTION       = "Enhance the contrast of an image.";
  inline static std::vector<std::string> TAGS = {"contrast", "enhancement", "normalize", "equalize histogram"};

  EnhanceContrast(joda::settings::PipelineStep &pipelineStep, settings::EnhanceContrastSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::IMAGE}, {InOuts::IMAGE}}), mParent(parent)
  {
    auto *modelTab = addTab(
        "Base", [] {}, false);

    //
    //
    mSaturatedPixels = SettingBase::create<SettingLineEdit<float>>(parent, generateIcon("percent"), "Saturated pixels [0-1]");
    mSaturatedPixels->setPlaceholderText("[0 - 1]");
    mSaturatedPixels->setUnit("%");
    mSaturatedPixels->setMinMax(0, 1);
    mSaturatedPixels->setValue(settings.saturatedPixels);
    mSaturatedPixels->connectWithSetting(&settings.saturatedPixels);
    mSaturatedPixels->setShortDescription("Sat. ");

    //
    // Options
    //
    mNormalize = SettingBase::create<SettingComboBox<bool>>(parent, {}, "Normalize");
    mNormalize->addOptions({
        {.key = false, .label = "Off"},
        {.key = true, .label = "On"},
    });
    mNormalize->setValue(settings.normalize);
    mNormalize->connectWithSetting(&settings.normalize);

    //
    //
    //
    mEqualizeHistogram = SettingBase::create<SettingComboBox<bool>>(parent, {}, "Equalize histogram");
    mEqualizeHistogram->addOptions({
        {.key = false, .label = "Off"},
        {.key = true, .label = "On"},
    });
    mEqualizeHistogram->setValue(settings.equalizeHistogram);
    mEqualizeHistogram->connectWithSetting(&settings.equalizeHistogram);

    //
    //
    addSetting(modelTab, {{mSaturatedPixels.get(), true, 0}, {mNormalize.get(), true, 0}, {mEqualizeHistogram.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::shared_ptr<SettingLineEdit<float>> mSaturatedPixels;
  std::unique_ptr<SettingComboBox<bool>> mNormalize;
  std::unique_ptr<SettingComboBox<bool>> mEqualizeHistogram;

  /////////////////////////////////////////////////////

private slots:
};

}    // namespace joda::ui::gui
