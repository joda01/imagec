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
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "ui/container/command/command.hpp"
#include "ui/container/setting/setting.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/helper/setting_generator.hpp"
#include "classifier_settings.hpp"

namespace joda::ui {

class Classifier : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Classifier";
  inline static std::string ICON  = "icons8-genealogy-50.png";

  Classifier(settings::ClassifierSettings &settings, QWidget *parent) :
      Command(parent), mSettings(settings), mParent(parent)
  {
    for(auto &classifierSetting : settings.classifiers) {
      classifiers.emplace_back(classifierSetting, *this, parent);
    }

    addFooter(TITLE.data(), ICON.data());
    auto *addClassifier = addActionButton("Add class", "icons8-genealogy-50.png");
    connect(addClassifier, &QAction::triggered, this, &Classifier::addClassifier);
  }

private:
  /////////////////////////////////////////////////////
  struct ClassifierFilter
  {
    ClassifierFilter(settings::ClassifierFilter &settings, Classifier &outer, QWidget *parent) : outer(outer)
    {
      //
      //
      mClusterOut = generateClusterDropDown("Match cluster", parent);
      mClusterOut->setValue(settings.clusterOut);
      mClusterOut->connectWithSetting(&settings.clusterOut, nullptr);

      //
      //
      mClassOut = generateClassDropDown("Match class", parent);
      mClassOut->setValue(settings.classOut);
      mClassOut->connectWithSetting(&settings.classOut, nullptr);
      mClassOut->setDisplayIconVisible(false);

      //
      //
      mMinParticleSize = std::shared_ptr<Setting<int32_t, int32_t>>(
          new Setting<int, int32_t>("", "[0 - " + QString::number(INT32_MAX) + "]", "Min particle size", "px",
                                    std::nullopt, 0, INT32_MAX, parent, ""));
      mMinParticleSize->setValue(settings.minParticleSize);
      mMinParticleSize->connectWithSetting(&settings.minParticleSize, nullptr);
      mMinParticleSize->setShortDescription("Min. ");
      //
      //
      mMaxParticleSize = std::shared_ptr<Setting<int32_t, int32_t>>(
          new Setting<int, int32_t>("", "[0 - " + QString::number(INT32_MAX) + "]", "Max particle size", "px",
                                    std::nullopt, 0, INT32_MAX, parent, ""));
      mMaxParticleSize->setValue(settings.maxParticleSize);
      mMaxParticleSize->connectWithSetting(&settings.maxParticleSize, nullptr);
      mMaxParticleSize->setShortDescription("Max. ");

      //
      //
      mMinCircularity = std::shared_ptr<Setting<float, int32_t>>(
          new Setting<float, int32_t>("", "[0 - 1]", "Min circularity", "%", std::nullopt, 0, 1, parent, ""));
      mMinCircularity->setValue(settings.minCircularity);
      mMinCircularity->connectWithSetting(&settings.minCircularity, nullptr);
      mMinCircularity->setShortDescription("Circ. ");

      outer.addSetting("Filter", {{mClusterOut, false},
                                  {mClassOut, true},
                                  {mMinCircularity, true},
                                  {mMinParticleSize, true},
                                  {mMaxParticleSize, true}});
    }

    std::shared_ptr<Setting<enums::ClusterIdIn, int>> mClusterOut;
    std::shared_ptr<Setting<enums::ClassId, int>> mClassOut;

    std::shared_ptr<Setting<int, int>> mMinParticleSize;
    std::shared_ptr<Setting<int, int>> mMaxParticleSize;
    std::shared_ptr<Setting<float, int>> mMinCircularity;
    std::shared_ptr<Setting<float, int>> mSnapAreaSize;

    Classifier &outer;
  };

  struct ObjectClass
  {
    ObjectClass(settings::ObjectClass &settings, Classifier &outer, QWidget *parent) : outer(outer)
    {
      //
      //
      mClusterOutNoMatch = generateClusterDropDown("No match cluster", parent);
      mClusterOutNoMatch->setValue(settings.clusterOutNoMatch);
      mClusterOutNoMatch->connectWithSetting(&settings.clusterOutNoMatch, nullptr);

      //
      //
      mClassOutNoMatch = generateClassDropDown("No match class", parent);
      mClassOutNoMatch->setValue(settings.classOutNoMatch);
      mClassOutNoMatch->connectWithSetting(&settings.classOutNoMatch, nullptr);

      //
      //
      mGrayScaleValue = std::shared_ptr<Setting<int32_t, int32_t>>(
          new Setting<int, int32_t>("icons8-genealogy-50.png", "[1 - " + QString::number(UINT16_MAX) + "]",
                                    "Grayscale value", "", std::nullopt, 1, UINT16_MAX, parent, ""));
      mGrayScaleValue->setValue(settings.modelClassId);
      mGrayScaleValue->connectWithSetting(&settings.modelClassId, nullptr);
      mGrayScaleValue->setShortDescription("Cls.");

      auto *col = outer.addSetting("Classification", {{mGrayScaleValue, false}});
      outer.addSetting("No match handling", {{mClusterOutNoMatch, false}, {mClassOutNoMatch, false}}, col);

      for(auto &filter : settings.filters) {
        mClassifyFilter.emplace_back(filter, outer, parent);
      }
    }

    std::shared_ptr<Setting<enums::ClusterIdIn, int>> mClusterOutNoMatch;
    std::shared_ptr<Setting<enums::ClassId, int>> mClassOutNoMatch;
    std::shared_ptr<Setting<int, int>> mGrayScaleValue;

    std::list<ClassifierFilter> mClassifyFilter;
    Classifier &outer;
  };

  std::list<ObjectClass> classifiers;
  settings::ClassifierSettings &mSettings;
  QWidget *mParent;

  /////////////////////////////////////////////////////
private slots:
  void addClassifier()
  {
    settings::ObjectClass objClass;
    objClass.modelClassId = 65535;
    auto &ret             = mSettings.classifiers.emplace_back(objClass);
    classifiers.emplace_back(ret, *this, mParent);
    updateDisplayText();
  }
};

}    // namespace joda::ui
