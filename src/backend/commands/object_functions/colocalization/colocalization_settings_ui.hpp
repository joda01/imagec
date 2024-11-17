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
#include "backend/commands/classification/classifier_filter.hpp"
#include "backend/commands/command.hpp"
#include "backend/commands/object_functions/colocalization/colocalization_settings.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "ui/container/command/command.hpp"
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_combobox_classes_out.hpp"
#include "ui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/helper/icon_generator.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/helper/setting_generator.hpp"
#include "colocalization_settings.hpp"

namespace joda::ui {

class Colocalization : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Colocalization";
  inline static std::string ICON  = "venn-diagram";

  Colocalization(joda::settings::PipelineStep &pipelineStep, settings::ColocalizationSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent, {{InOuts::OBJECT}, {InOuts::OBJECT}}), mSettings(settings), mParent(parent)
  {
    auto *modelTab = addTab("Base", [] {});

    mClustersIn = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, generateIcon("venn-diagram"), "Classes to coloc.");
    mClustersIn->setValue(settings.inputClusters);
    mClustersIn->connectWithSetting(&settings.inputClusters);

    addSetting(modelTab, "Input", {{mClustersIn.get(), true, 0}});

    //
    //
    //
    mClassOutput = SettingBase::create<SettingComboBoxClassesOut>(parent, generateIcon("circle"), "Output class");
    mClassOutput->setValue(settings.outputCluster.classId);
    mClassOutput->connectWithSetting(&settings.outputCluster.classId);

    //
    //
    mMinIntersection = SettingBase::create<SettingLineEdit<float>>(parent, generateIcon("query-inner-join"), "Min. intersection");
    mMinIntersection->setDefaultValue(0.1);
    mMinIntersection->setPlaceholderText("[0 - 1]");
    mMinIntersection->setUnit("%");
    mMinIntersection->setMinMax(0, 1);
    mMinIntersection->setValue(settings.minIntersection);
    mMinIntersection->connectWithSetting(&settings.minIntersection);
    mMinIntersection->setShortDescription("Cls. ");

    auto *col = addSetting(modelTab, "Output", {{mClassOutput.get(), true, 0}, {mMinIntersection.get(), false, 0}});
  }

private:
  /////////////////////////////////////////////////////
  settings::ColocalizationSettings &mSettings;
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBoxClassesOut> mClassOutput;
  std::unique_ptr<SettingLineEdit<float>> mMinIntersection;
  std::unique_ptr<SettingComboBoxMultiClassificationIn> mClustersIn;

  /////////////////////////////////////////////////////
};

}    // namespace joda::ui
