///
/// \file      setting_line_Edit.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-22
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "setting_combobox_multi_classification_in.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/settings/settings_types.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui {

QWidget *SettingComboBoxMultiClassificationIn::createInputObject()
{
  mComboBox = new QComboBoxMulti();
  mComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  mComboBox->addAction(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE), "");

  clusterNamesChanged();

  SettingBase::connect(mComboBox, &QComboBoxMulti::currentIndexChanged, this, &SettingComboBoxMultiClassificationIn::onValueChanged);
  SettingBase::connect(mComboBox, &QComboBoxMulti::currentTextChanged, this, &SettingComboBoxMultiClassificationIn::onValueChanged);

  return mComboBox;
}

void SettingComboBoxMultiClassificationIn::setDefaultValue(settings::ClassificatorSetting defaultVal)
{
  mDefaultValue = defaultVal;
  reset();
}

void SettingComboBoxMultiClassificationIn::reset()
{
  if(mDefaultValue.has_value()) {
    setValue({mDefaultValue.value()});
  }
}

void SettingComboBoxMultiClassificationIn::clear()
{
  mComboBox->setCurrentIndex(0);
}

void SettingComboBoxMultiClassificationIn::clusterNamesChanged()
{
  outputClustersChanges();
}

void SettingComboBoxMultiClassificationIn::outputClustersChanges()
{
  auto *parent = getParent();
  if(parent != nullptr) {
    auto outputClusters = parent->getOutputClasses();
    mComboBox->blockSignals(true);
    auto actSelected = getValue();
    mComboBox->clear();
    auto [clusteres, classes] = parent->getPanelClassification()->getClustersAndClasses();
    if(outputClusters.size() > 0) {
      auto oldCluster = outputClusters.begin()->clusterId;
      for(const auto &data : outputClusters) {
        if(data.classId != enums::ClassId::UNDEFINED) {
          if(oldCluster != data.clusterId) {
            oldCluster = data.clusterId;
            mComboBox->insertSeparator(mComboBox->count());
          }

          QVariant variant;
          variant = QVariant(toInt(data));

          mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)),
                             clusteres[static_cast<enums::ClusterIdIn>(data.clusterId)] + "@" + classes[data.classId], variant);
        }
      }
    }
    setValue(actSelected);
    mComboBox->blockSignals(false);
  }
}

QString SettingComboBoxMultiClassificationIn::getName(settings::ClassificatorSetting key) const
{
  auto idx = mComboBox->findData(toInt(key), Qt::UserRole + 1);
  if(idx >= 0) {
    return mComboBox->itemText(idx);
  }
  return "";
}

settings::ObjectInputClusters SettingComboBoxMultiClassificationIn::getValue()
{
  settings::ObjectInputClusters toReturn;
  auto checked = (mComboBox)->getCheckedItems();

  for(const auto &[data, _] : checked) {
    toReturn.emplace(fromInt(data.toUInt()));
  }

  return toReturn;
}

void SettingComboBoxMultiClassificationIn::setValue(const settings::ObjectInputClusters &valueIn)
{
  QVariantList toCheck;
  for(const auto &value : valueIn) {
    toCheck.append(toInt(value));
  }
  (mComboBox)->setCheckedItems(toCheck);
}

std::map<settings::ClassificatorSetting, std::string> SettingComboBoxMultiClassificationIn::getValueAndNames()
{
  std::map<settings::ClassificatorSetting, std::string> toReturn;
  auto checked = ((QComboBoxMulti *) mComboBox)->getCheckedItems();

  for(const auto &[data, txt] : checked) {
    toReturn.emplace(fromInt(data.toUInt()), txt.toStdString());
  }

  return toReturn;
}

void SettingComboBoxMultiClassificationIn::onValueChanged()
{
  if(mSetting != nullptr) {
    *mSetting = getValue();
  }
  QVariant itemData = mComboBox->itemData(mComboBox->currentIndex(), Qt::DecorationRole);
  if(itemData.isValid() && itemData.canConvert<QIcon>()) {
    auto selectedIcon = qvariant_cast<QIcon>(itemData);
    SettingBase::triggerValueChanged(((QComboBoxMulti *) mComboBox)->getDisplayText(), selectedIcon);
  } else {
    SettingBase::triggerValueChanged(((QComboBoxMulti *) mComboBox)->getDisplayText());
  }
}

}    // namespace joda::ui
