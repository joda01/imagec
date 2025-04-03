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
#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/setting/setting_base.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "threshold_settings.hpp"

namespace joda::ui::gui {

class Threshold : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Threshold";
  inline static std::string ICON              = "layer-mask";
  inline static std::string DESCRIPTION       = "Converts a grayscale image to a binary image.";
  inline static std::vector<std::string> TAGS = {"threshold", "background", "binary", "otsu", "li", "triangle", "shanbhag"};

  Threshold(joda::settings::PipelineStep &pipelineStep, settings::ThresholdSettings &settings, QWidget *parent) :
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

    auto *addFilter = addActionButton("Add threshold", generateIcon("add"));
    connect(addFilter, &QAction::triggered, this, &Threshold::addFilter);
  }

private:
  class ThresholdUi
  {
  public:
    ThresholdUi(settings::ThresholdSettings::Threshold &settings, Threshold &outer, helper::TabWidget *tab, int32_t index, QWidget *parent) :
        settings(settings), outer(outer)
    {
      //
      //
      //
      mThresholdAlgorithm = SettingBase::create<SettingComboBox<joda::settings::ThresholdSettings::Methods>>(
          parent, generateIcon("automatic-contrast"), "Threshold algorithm");
      mThresholdAlgorithm->addOptions({{joda::settings::ThresholdSettings::Methods::MANUAL, "Manual", generateIcon("contrast")},
                                       {joda::settings::ThresholdSettings::Methods::LI, "Li"},
                                       {joda::settings::ThresholdSettings::Methods::MIN_ERROR, "Min. error"},
                                       {joda::settings::ThresholdSettings::Methods::TRIANGLE, "Triangle"},
                                       {joda::settings::ThresholdSettings::Methods::MOMENTS, "Moments"},
                                       {joda::settings::ThresholdSettings::Methods::OTSU, "Otsu"},
                                       {joda::settings::ThresholdSettings::Methods::MEAN, "Mean"},
                                       {joda::settings::ThresholdSettings::Methods::SHANBHAG, "Shanbhag"},
                                       {joda::settings::ThresholdSettings::Methods::HUANG, "Huang"},
                                       {joda::settings::ThresholdSettings::Methods::INTERMODES, "Intermodes"},
                                       {joda::settings::ThresholdSettings::Methods::ISODATA, "ISO Data"},
                                       {joda::settings::ThresholdSettings::Methods::MAX_ENTROPY, "Max. entropy"},
                                       {joda::settings::ThresholdSettings::Methods::MINIMUM, "Minimum"},
                                       {joda::settings::ThresholdSettings::Methods::PERCENTILE, "Percentile"},
                                       {joda::settings::ThresholdSettings::Methods::RENYI_ENTROPY, "Renyi entropy"},
                                       {joda::settings::ThresholdSettings::Methods::YEN, "Yen"}});
      mThresholdAlgorithm->setValue(settings.method);
      mThresholdAlgorithm->connectWithSetting(&settings.method);

      //
      //
      //
      mGrayScaleValue = generateThresholdClass("Threshold class", parent);
      mGrayScaleValue->setValue(settings.modelClassId);
      mGrayScaleValue->connectWithSetting(&settings.modelClassId);

      outer.addSetting(tab, "Algorithm", {{mThresholdAlgorithm.get(), true, index}, {mGrayScaleValue.get(), true, index}});

      //
      //
      //
      mThresholdValueMin = SettingBase::create<SettingLineEdit<uint16_t>>(parent, generateIcon("light-min"), "Min. threshold");
      mThresholdValueMin->setPlaceholderText("[0 - 65535]");
      mThresholdValueMin->setUnit("");
      mThresholdValueMin->setMinMax(0, 65535);
      mThresholdValueMin->setValue(settings.thresholdMin);
      mThresholdValueMin->connectWithSetting(&settings.thresholdMin);
      mThresholdValueMin->setShortDescription("Min. ");

      //
      //
      //
      mThresholdValueMax = SettingBase::create<SettingLineEdit<uint16_t>>(parent, generateIcon("light"), "Max. threshold");
      mThresholdValueMax->setPlaceholderText("[0 - 65535]");
      mThresholdValueMax->setUnit("");
      mThresholdValueMax->setMinMax(0, 65535);
      mThresholdValueMax->setValue(settings.thresholdMax);
      mThresholdValueMax->connectWithSetting(&settings.thresholdMax);
      mThresholdValueMax->setShortDescription("Max. ");

      //
      //
      //
      mCValue = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateIcon("add"), "Auto contrast added const");
      mCValue->setEmptyValue(0);
      mCValue->setPlaceholderText("[-32000 - +32000]");
      mCValue->setUnit("");
      mCValue->setMinMax(-32000, 32000);
      mCValue->setValue(settings.cValue);
      mCValue->connectWithSetting(&settings.cValue);
      mCValue->setShortDescription("C:");

      outer.addSetting(tab, "Settings",
                       {
                           {mThresholdValueMin.get(), true, index},
                           {mThresholdValueMax.get(), false, index},
                           {mCValue.get(), false, index},
                       });
    }

    ~ThresholdUi()
    {
      outer.removeSetting({mGrayScaleValue.get(), mThresholdAlgorithm.get(), mThresholdValueMin.get(), mThresholdValueMax.get(), mCValue.get()});
    }

    /////////////////////////////////////////////////////
    std::unique_ptr<SettingComboBox<int32_t>> mGrayScaleValue;
    std::unique_ptr<SettingComboBox<joda::settings::ThresholdSettings::Methods>> mThresholdAlgorithm;
    std::shared_ptr<SettingLineEdit<uint16_t>> mThresholdValueMin;
    std::shared_ptr<SettingLineEdit<uint16_t>> mThresholdValueMax;
    std::unique_ptr<SettingLineEdit<int32_t>> mCValue;

    settings::ThresholdSettings::Threshold &settings;
    Threshold &outer;
  };

  std::list<ThresholdUi> thresholds;
  settings::ThresholdSettings &mSettings;
  QWidget *mParent;

  void removeObjectClass(settings::ThresholdSettings::Threshold *obj)
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
    settings::ThresholdSettings::Threshold objClass;
    auto &ret = mSettings.modelClasses.emplace_back(objClass);
    auto *tab = addTab(
        "Th", [this, &ret] { removeObjectClass(&ret); }, true);
    thresholds.emplace_back(ret, *this, tab, mSettings.modelClasses.size(), mParent);
    updateDisplayText();
  }
};

}    // namespace joda::ui::gui
