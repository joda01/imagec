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
#include <cstdint>
#include "backend/commands/command.hpp"
#include "ui/container/command/command.hpp"
#include "ui/container/setting/setting_base.hpp"
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/helper/layout_generator.hpp"
#include "threshold_settings.hpp"

namespace joda::ui {

class Threshold : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Threshold";
  inline static std::string ICON  = "icons8-grayscale-50.png";

  Threshold(joda::settings::PipelineStep &pipelineStep, settings::ThresholdSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent), mSettings(settings), mParent(parent)
  {
    if(settings.classes.empty()) {
      addFilter();
    }

    for(auto &classifierSetting : settings.classes) {
      auto *tab = addTab("Th", [this, &classifierSetting] { removeObjectClass(&classifierSetting); });
      thresholds.emplace_back(classifierSetting, *this, tab, parent);
    }

    auto *addFilter = addActionButton("Add threshold", "icons8-add-new-50.png");
    connect(addFilter, &QAction::triggered, this, &Threshold::addFilter);
  }

private:
  class ThresholdUi
  {
  public:
    ThresholdUi(settings::ThresholdSettings::Threshold &settings, Threshold &outer, helper::TabWidget *tab,
                QWidget *parent) :
        settings(settings),
        outer(outer)
    {
      //
      //
      //
      mThresholdAlgorithm = SettingBase::create<SettingComboBox<joda::settings::ThresholdSettings::Mode>>(
          parent, "icons8-grayscale-50.png", "Threshold algorithm");
      mThresholdAlgorithm->addOptions({{joda::settings::ThresholdSettings::Mode::MANUAL, "Manual"},
                                       {joda::settings::ThresholdSettings::Mode::LI, "Li"},
                                       {joda::settings::ThresholdSettings::Mode::MIN_ERROR, "Min. error"},
                                       {joda::settings::ThresholdSettings::Mode::TRIANGLE, "Triangle"},
                                       {joda::settings::ThresholdSettings::Mode::MOMENTS, "Moments"},
                                       {joda::settings::ThresholdSettings::Mode::OTSU, "Otsu"}});
      mThresholdAlgorithm->setValue(settings.mode);
      mThresholdAlgorithm->connectWithSetting(&settings.mode);

      //
      //
      //
      mThresholdValueMin = SettingBase::create<SettingLineEdit<uint16_t>>(parent, "", "Min. threshold");
      mThresholdValueMin->setPlaceholderText("[0 - 65535]");
      mThresholdValueMin->setUnit("");
      mThresholdValueMin->setMinMax(0, 65535);
      mThresholdValueMin->setValue(settings.thresholdMin);
      mThresholdValueMin->connectWithSetting(&settings.thresholdMin);
      mThresholdValueMin->setShortDescription("Min. ");

      //
      //
      //
      mThresholdValueMax = SettingBase::create<SettingLineEdit<uint16_t>>(parent, "", "Max. threshold");
      mThresholdValueMax->setPlaceholderText("[0 - 65535]");
      mThresholdValueMax->setUnit("");
      mThresholdValueMax->setMinMax(0, 65535);
      mThresholdValueMax->setValue(settings.thresholdMax);
      mThresholdValueMax->connectWithSetting(&settings.thresholdMax);
      mThresholdValueMax->setShortDescription("Max. ");

      outer.addSetting(
          tab, "",
          {{mThresholdAlgorithm.get(), true}, {mThresholdValueMin.get(), true}, {mThresholdValueMax.get(), true}});
    }

    ~ThresholdUi()
    {
      outer.removeSetting({mThresholdAlgorithm.get(), mThresholdValueMin.get(), mThresholdValueMax.get()});
    }

    /////////////////////////////////////////////////////
    std::unique_ptr<SettingComboBox<joda::settings::ThresholdSettings::Mode>> mThresholdAlgorithm;
    std::shared_ptr<SettingLineEdit<uint16_t>> mThresholdValueMin;
    std::shared_ptr<SettingLineEdit<uint16_t>> mThresholdValueMax;
    settings::ThresholdSettings::Threshold &settings;
    Threshold &outer;
  };

  std::list<ThresholdUi> thresholds;
  settings::ThresholdSettings &mSettings;
  QWidget *mParent;

  void removeObjectClass(settings::ThresholdSettings::Threshold *obj)
  {
    {
      auto it = mSettings.classes.begin();
      for(; it != mSettings.classes.end(); it++) {
        if(&(*it) == obj) {
          mSettings.classes.erase(it);
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
    auto &ret        = mSettings.classes.emplace_back(objClass);
    ret.modelClassId = UINT16_MAX - (mSettings.classes.size() - 1);
    auto *tab        = addTab("Th", [this, &ret] { removeObjectClass(&ret); });
    thresholds.emplace_back(ret, *this, tab, mParent);
    updateDisplayText();
  }
};

}    // namespace joda::ui
