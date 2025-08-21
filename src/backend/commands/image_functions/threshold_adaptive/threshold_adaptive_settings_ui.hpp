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
#include <cstdint>
#include "backend/commands/command.hpp"
#include "ui/gui/editor/widget_pipeline/widget_command/command.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_base.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "threshold_adaptive_settings.hpp"

namespace joda::ui::gui {

class ThresholdAdaptive : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Adaptive threshold (ALPHA)";
  inline static std::string ICON              = "circle-half-duotone";
  inline static std::string DESCRIPTION       = "Converts a grayscale image to a binary image.";
  inline static std::vector<std::string> TAGS = {"threshold", "background", "binary", "adaptive", "sauvola", "nitblack", "otsu", "phanskalar"};

  ThresholdAdaptive(joda::settings::PipelineStep &pipelineStep, settings::ThresholdAdaptiveSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::IMAGE}, {InOuts::BINARY}}), mSettings(settings),
      mParent(parent)
  {
    if(settings.modelClasses.empty()) {
      addFilter();
    }
    int cnt = 0;
    for(auto &classifierSetting : settings.modelClasses) {
      auto *tab = addTab(
          "Th", [this, &classifierSetting] { removeObjectClass(&classifierSetting); }, cnt > 0);
      thresholds.emplace_back(classifierSetting, *this, tab, cnt, parent);
      cnt++;
    }

    auto *addFilter = addActionButton("Add threshold", generateSvgIcon<Style::REGULAR, Color::BLACK>("list-plus"));
    connect(addFilter, &QAction::triggered, this, &ThresholdAdaptive::addFilter);
  }

private:
  class ThresholdAdaptiveUi
  {
  public:
    ThresholdAdaptiveUi(settings::ThresholdAdaptiveSettings::ThresholdAdaptive &settingsIn, ThresholdAdaptive &outerIn, helper::TabWidget *tab,
                        int32_t index, QWidget *parent) :
        settings(settingsIn),
        outer(outerIn)
    {
      //
      //
      //
      mThresholdAdaptiveAlgorithm =
          SettingBase::create<SettingComboBox<joda::settings::ThresholdAdaptiveSettings::Methods>>(parent, {}, "ThresholdAdaptive algorithm");
      mThresholdAdaptiveAlgorithm->addOptions({
          {joda::settings::ThresholdAdaptiveSettings::Methods::BERNSEN, "Bernsen"},
          {joda::settings::ThresholdAdaptiveSettings::Methods::CONTRAST, "Contrast"},
          {joda::settings::ThresholdAdaptiveSettings::Methods::MEAN, "Mean"},
          {joda::settings::ThresholdAdaptiveSettings::Methods::MEDIAN, "Median"},
          // Implemented but slow {joda::settings::ThresholdAdaptiveSettings::Methods::OTSU, "Otsu"},
          //{joda::settings::ThresholdAdaptiveSettings::Methods::MID_GRAY, "Mid gray"},
          //{joda::settings::ThresholdAdaptiveSettings::Methods::NIBLACK, "Nitblack"},
          //{joda::settings::ThresholdAdaptiveSettings::Methods::PHANSALKAR, "Phanskalar"},
          //{joda::settings::ThresholdAdaptiveSettings::Methods::SAUVOLA, "Sauvola"}
      });
      mThresholdAdaptiveAlgorithm->setValue(settings.method);
      mThresholdAdaptiveAlgorithm->connectWithSetting(&settings.method);

      //
      //
      //
      mKernelSize = SettingBase::create<SettingLineEdit<int32_t>>(parent, {}, "Kernel size");
      // mKernelSize->addOptions({{-1, "Off"},
      //                          {3, "3x3"},
      //                          {5, "5x5"},
      //                          {7, "7x7"},
      //                          {9, "9x9"},
      //                          {11, "11x11"},
      //                          {13, "13x13"},
      //                          {15, "15x15"},
      //                          {17, "17x17"},
      //                          {19, "19x19"},
      //                          {21, "21x21"},
      //                          {23, "23x23"}});
      mKernelSize->setValue(settings.kernelSize);
      mKernelSize->connectWithSetting(&settings.kernelSize);
      mKernelSize->setShortDescription("Kernel: ");

      //
      //
      //
      mGrayScaleValue = generateThresholdClass("ThresholdAdaptive class", parent);
      mGrayScaleValue->setValue(settings.modelClassId);
      mGrayScaleValue->connectWithSetting(&settings.modelClassId);

      outer.addSetting(tab, "Algorithm",
                       {{mThresholdAdaptiveAlgorithm.get(), true, index}, {mKernelSize.get(), true, index}, {mGrayScaleValue.get(), true, index}});

      //
      //
      //
      mContrastThreshold = SettingBase::create<SettingLineEdit<int32_t>>(parent, {}, "Contrast threshold");
      mContrastThreshold->setPlaceholderText("[0 - 65535]");
      mContrastThreshold->setUnit("");
      mContrastThreshold->setMinMax(0, 65535);
      mContrastThreshold->setValue(settings.contrastThreshold);
      mContrastThreshold->connectWithSetting(&settings.contrastThreshold);
      mContrastThreshold->setShortDescription("Contrast. ");

      //
      //
      //
      mThresholdOffset = SettingBase::create<SettingLineEdit<int32_t>>(parent, {}, "Threshold offset");
      mThresholdOffset->setPlaceholderText("[0 - 65535]");
      mThresholdOffset->setUnit("");
      mThresholdOffset->setMinMax(0, 65535);
      mThresholdOffset->setValue(settings.thresholdOffset);
      mThresholdOffset->connectWithSetting(&settings.thresholdOffset);
      mThresholdOffset->setShortDescription("Offset. ");

      outer.addSetting(tab, "Settings", {{mContrastThreshold.get(), true, index}, {mThresholdOffset.get(), true, index}});
    }

    ~ThresholdAdaptiveUi()
    {
      outer.removeSetting(
          {mGrayScaleValue.get(), mThresholdAdaptiveAlgorithm.get(), mKernelSize.get(), mThresholdOffset.get(), mContrastThreshold.get()});
    }

    /////////////////////////////////////////////////////
    std::unique_ptr<SettingComboBox<int32_t>> mGrayScaleValue;
    std::unique_ptr<SettingComboBox<joda::settings::ThresholdAdaptiveSettings::Methods>> mThresholdAdaptiveAlgorithm;
    std::shared_ptr<SettingLineEdit<int32_t>> mKernelSize;
    std::shared_ptr<SettingLineEdit<int32_t>> mContrastThreshold;
    std::shared_ptr<SettingLineEdit<int32_t>> mThresholdOffset;

    settings::ThresholdAdaptiveSettings::ThresholdAdaptive &settings;
    ThresholdAdaptive &outer;
  };

  std::list<ThresholdAdaptiveUi> thresholds;
  settings::ThresholdAdaptiveSettings &mSettings;
  QWidget *mParent;

  void removeObjectClass(settings::ThresholdAdaptiveSettings::ThresholdAdaptive *obj)
  {
    {
      auto it = mSettings.modelClasses.begin();
      for(; it != mSettings.modelClasses.end(); it++) {
        if(&(*it) == obj) {
          mSettings.modelClasses.erase(it);
          break;
        }
      }
    }

    {
      auto it = thresholds.begin();
      for(; it != thresholds.end(); it++) {
        if(&(it->settings) == obj) {
          thresholds.erase(it);
          break;
        }
      }
    }
    updateDisplayText();
  }

private slots:
  void addFilter()
  {
    settings::ThresholdAdaptiveSettings::ThresholdAdaptive objClass;
    auto &ret = mSettings.modelClasses.emplace_back(objClass);
    auto *tab = addTab(
        "Th", [this, &ret] { removeObjectClass(&ret); }, true);
    thresholds.emplace_back(ret, *this, tab, mSettings.modelClasses.size(), mParent);
    updateDisplayText();
  }
};

}    // namespace joda::ui::gui
