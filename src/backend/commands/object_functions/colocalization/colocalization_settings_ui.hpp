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

#include <qaction.h>
#include <qwidget.h>
#include <cstdint>
#include "backend/commands/classification/classifier_filter.hpp"
#include "backend/commands/command.hpp"
#include "backend/commands/object_functions/colocalization/colocalization_settings.hpp"
#include "backend/enums/enums_classes.hpp"

#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_combobox_classes_out.hpp"
#include "ui/gui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "colocalization_settings.hpp"

namespace joda::ui::gui {

class Colocalization : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Colocalization";
  inline static std::string ICON              = "bwtonal";
  inline static std::string DESCRIPTION       = "Calculates the overlapping are of two or more image channels.";
  inline static std::vector<std::string> TAGS = {"colocalization", "object", "coloc"};

  Colocalization(joda::settings::PipelineStep &pipelineStep, settings::ColocalizationSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::OBJECT}, {InOuts::OBJECT}}), mSettings(settings),
      mParent(parent)
  {
    auto *modelTab = addTab(
        "Base", [] {}, false);

    if(settings.inputClasses.size() < 4) {
      settings.inputClasses.reserve(4);
      settings.inputClasses.resize(4);
    }
    //=================================================
    //
    //
    //
    mClassesIn01 = SettingBase::create<SettingComboBoxClassesOut>(parent, {}, "Class to coloc.");
    mClassesIn01->setValue(settings.inputClasses.at(0).inputClassId);
    mClassesIn01->connectWithSetting(&settings.inputClasses.at(0).inputClassId);

    mNewClassesIn01 = SettingBase::create<SettingComboBoxClassesOut>(parent, {}, "Reclassify to if coloc.");
    mNewClassesIn01->setValue(settings.inputClasses.at(0).newClassId);
    mNewClassesIn01->connectWithSetting(&settings.inputClasses.at(0).newClassId);
    auto *firstCol = addSetting(modelTab, "Input 1", {{mClassesIn01.get(), true, 0}, {mNewClassesIn01.get(), false, 0}});

    //
    //
    //
    mClassesIn02 = SettingBase::create<SettingComboBoxClassesOut>(parent, {}, "Class to coloc.");
    mClassesIn02->setValue(settings.inputClasses.at(1).inputClassId);
    mClassesIn02->connectWithSetting(&settings.inputClasses.at(1).inputClassId);

    mNewClassesIn02 = SettingBase::create<SettingComboBoxClassesOut>(parent, {}, "Reclassify to if coloc.");
    mNewClassesIn02->setValue(settings.inputClasses.at(1).newClassId);
    mNewClassesIn02->connectWithSetting(&settings.inputClasses.at(1).newClassId);
    addSetting(modelTab, "Input 2", {{mClassesIn02.get(), true, 0}, {mNewClassesIn02.get(), false, 0}}, firstCol);

    //
    //
    //
    mClassesIn03 = SettingBase::create<SettingComboBoxClassesOut>(parent, {}, "Class to coloc.");
    mClassesIn03->setValue(settings.inputClasses.at(2).inputClassId);
    mClassesIn03->connectWithSetting(&settings.inputClasses.at(2).inputClassId);

    mNewClassesIn03 = SettingBase::create<SettingComboBoxClassesOut>(parent, {}, "Reclassify to if coloc.");
    mNewClassesIn03->setValue(settings.inputClasses.at(2).newClassId);
    mNewClassesIn03->connectWithSetting(&settings.inputClasses.at(2).newClassId);
    addSetting(modelTab, "Input 3", {{mClassesIn03.get(), true, 0}, {mNewClassesIn03.get(), false, 0}}, firstCol);

    //
    //
    //
    mClassesIn04 = SettingBase::create<SettingComboBoxClassesOut>(parent, {}, "Class to coloc.");
    mClassesIn04->setValue(settings.inputClasses.at(3).inputClassId);
    mClassesIn04->connectWithSetting(&settings.inputClasses.at(3).inputClassId);

    mNewClassesIn04 = SettingBase::create<SettingComboBoxClassesOut>(parent, {}, "Reclassify to if coloc.");
    mNewClassesIn04->setValue(settings.inputClasses.at(3).newClassId);
    mNewClassesIn04->connectWithSetting(&settings.inputClasses.at(3).newClassId);
    addSetting(modelTab, "Input 4", {{mClassesIn04.get(), true, 0}, {mNewClassesIn04.get(), false, 0}}, firstCol);

    //=====================================================
    //
    //
    //
    mClassOutput = SettingBase::create<SettingComboBoxClassesOut>(parent, generateSvgIcon("choice-round"), "Coloc output class");
    mClassOutput->setValue(settings.outputClass);
    mClassOutput->connectWithSetting(&settings.outputClass);

    //
    //
    mMinIntersection = SettingBase::create<SettingLineEdit<float>>(parent, generateSvgIcon("format-number-percent"), "Min. intersection");
    mMinIntersection->setDefaultValue(0.1);
    mMinIntersection->setPlaceholderText("[0 - 1]");
    mMinIntersection->setUnit("%");
    mMinIntersection->setMinMax(0, 1);
    mMinIntersection->setValue(settings.minIntersection);
    mMinIntersection->connectWithSetting(&settings.minIntersection);
    mMinIntersection->setShortDescription("Cls. ");

    //
    //
    //
    mMode = SettingBase::create<SettingComboBox<joda::settings::ColocalizationSettings::Mode>>(parent, {}, "Mode");
    mMode->addOptions(
        {{.key = joda::settings::ColocalizationSettings::Mode::RECLASSIFY_MOVE, .label = "Reclassify Move", .icon = generateSvgIcon("edit-move")},
         {.key = joda::settings::ColocalizationSettings::Mode::RECLASSIFY_COPY, .label = "Reclassify Copy", .icon = generateSvgIcon("edit-copy")}});
    mMode->setValue(settings.mode);
    mMode->connectWithSetting(&settings.mode);

    //
    //
    //
    mTrackingMode = SettingBase::create<SettingComboBox<joda::settings::ColocalizationSettings::TrackingMode>>(parent, {}, "Tracking mode");
    mTrackingMode->addOptions({{.key   = joda::settings::ColocalizationSettings::TrackingMode::OVERRIDE,
                                .label = "Override existing tracking information",
                                .icon  = generateSvgIcon("join")},
                               /*{.key   = joda::settings::ColocalizationSettings::TrackingMode::KEEP_EXISTING,
                                .label = "Keep existing tracking information",
                                .icon  = generateSvgIcon("view-list-tree")}*/});
    mTrackingMode->setValue(settings.trackingMode);
    mTrackingMode->connectWithSetting(&settings.trackingMode);

    auto *col =
        addSetting(modelTab, "Output",
                   {{mClassOutput.get(), true, 0}, {mMinIntersection.get(), false, 0}, {mMode.get(), false, 0}, {mTrackingMode.get(), false, 0}});
  }

private:
  /////////////////////////////////////////////////////
  settings::ColocalizationSettings &mSettings;
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBoxClassesOut> mClassOutput;
  std::unique_ptr<SettingLineEdit<float>> mMinIntersection;

  std::unique_ptr<SettingComboBoxClassesOut> mClassesIn01;
  std::unique_ptr<SettingComboBoxClassesOut> mNewClassesIn01;

  std::unique_ptr<SettingComboBoxClassesOut> mClassesIn02;
  std::unique_ptr<SettingComboBoxClassesOut> mNewClassesIn02;

  std::unique_ptr<SettingComboBoxClassesOut> mClassesIn03;
  std::unique_ptr<SettingComboBoxClassesOut> mNewClassesIn03;

  std::unique_ptr<SettingComboBoxClassesOut> mClassesIn04;
  std::unique_ptr<SettingComboBoxClassesOut> mNewClassesIn04;

  std::unique_ptr<SettingComboBox<joda::settings::ColocalizationSettings::Mode>> mMode;
  std::unique_ptr<SettingComboBox<joda::settings::ColocalizationSettings::TrackingMode>> mTrackingMode;

  /////////////////////////////////////////////////////
};

}    // namespace joda::ui::gui
