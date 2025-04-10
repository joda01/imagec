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
#include "backend/enums/enums_classes.hpp"

#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/setting/setting_base.hpp"
#include "ui/gui/container/setting/setting_combobox_multi.hpp"
#include "ui/gui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "measure_settings.hpp"

namespace joda::ui::gui {

class Measure : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Measure";
  inline static std::string ICON              = "tool-measure";
  inline static std::string DESCRIPTION       = "Measure the intensity of objects areas in an image channel.";
  inline static std::vector<std::string> TAGS = {"intensity", "measure", "object"};

  Measure(joda::settings::PipelineStep &pipelineStep, settings::MeasureSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::OBJECT}, {InOuts::OBJECT}}), mSettings(settings),
      mParent(parent)
  {
    //
    //
    classesIn = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, generateIcon("circle"), "Classes in");
    classesIn->setValue(settings.inputClasses);
    classesIn->connectWithSetting(&settings.inputClasses);

    //
    //
    cStackIndex = generateCStackCombo<SettingComboBoxMulti<int32_t>>("Image channel", parent);
    std::set<int32_t> cStacks;
    enums::ZProjection zProject = enums::ZProjection::NONE;
    int32_t zStackIdx           = 0;
    for(const auto &cStack : settings.planesIn) {
      cStacks.emplace(cStack.imagePlane.cStack);
      zProject  = cStack.zProjection;
      zStackIdx = cStack.imagePlane.zStack;
    }
    cStackIndex->setValue(cStacks);
    connect(cStackIndex.get(), &SettingBase::valueChanged, this, &Measure::onCStackChanged);

    //
    //
    zProjection = generateZProjection(true, parent);
    zProjection->setValue(zProject);
    connect(zProjection.get(), &SettingBase::valueChanged, this, &Measure::onCStackChanged);

    //
    //
    //
    zStackIndex = generateStackIndexCombo(true, "Z-Channel", parent);
    zStackIndex->setValue(zStackIdx);
    connect(zStackIndex.get(), &SettingBase::valueChanged, this, &Measure::onCStackChanged);

    //
    //
    auto *tab = addTab(
        "Input class", [] {}, false);
    addSetting(tab, "Input classes", {{classesIn.get(), true, 0}});
    addSetting(tab, "Input image channels", {{cStackIndex.get(), true, 0}, {zProjection.get(), true, 0}, {zStackIndex.get(), false, 0}});

    // auto *addClassifier = addActionButton("Add class", "icons8-genealogy");
    //  connect(addClassifier, &QAction::triggered, this, &Classifier::addClassifier);
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBoxMultiClassificationIn> classesIn;
  std::unique_ptr<SettingComboBoxMulti<int32_t>> cStackIndex;
  std::unique_ptr<SettingComboBox<enums::ZProjection>> zProjection;
  std::unique_ptr<SettingSpinBox<int32_t>> zStackIndex;

  settings::MeasureSettings &mSettings;
  QWidget *mParent;
private slots:

  void onCStackChanged()
  {
    auto cStacks = cStackIndex->getValue();
    mSettings.planesIn.clear();
    for(const auto &cStack : cStacks) {
      enums::ImageId id;
      id.imagePlane.cStack = cStack;
      id.imagePlane.zStack = zStackIndex->getValue();
      id.zProjection       = zProjection->getValue();
      mSettings.planesIn.emplace_back(id);
    }
  }
};

}    // namespace joda::ui::gui
