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

#include "setting_combobox_multi_classification_unmanaged.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui {

QWidget *SettingComboBoxMultiClassificationUnmanaged::createInputObject()
{
  mComboBox = new QComboBoxMulti();
  mComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  mComboBox->addAction(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE), "");

  SettingBase::connect(mComboBox, &QComboBoxMulti::currentIndexChanged, this, &SettingComboBoxMultiClassificationUnmanaged::onValueChanged);
  SettingBase::connect(mComboBox, &QComboBoxMulti::currentTextChanged, this, &SettingComboBoxMultiClassificationUnmanaged::onValueChanged);

  return mComboBox;
}

void SettingComboBoxMultiClassificationUnmanaged::setDefaultValue(settings::ClassificatorSettingOut defaultVal)
{
  mDefaultValue = defaultVal;
  reset();
}

void SettingComboBoxMultiClassificationUnmanaged::reset()
{
  if(mDefaultValue.has_value()) {
    setValue({mDefaultValue.value()});
  }
}

void SettingComboBoxMultiClassificationUnmanaged::clear()
{
  mComboBox->setCurrentIndex(0);
}

void SettingComboBoxMultiClassificationUnmanaged::addOptions(const std::map<settings::ClassificatorSettingOut, QString> &dataIn)
{
  mComboBox->blockSignals(true);
  auto actSelected = getValue();
  mComboBox->clear();
  if(!dataIn.empty()) {
    auto oldCluster = dataIn.begin()->first.clusterId;
    for(const auto &[data, label] : dataIn) {
      if(data.classId != enums::ClassId::UNDEFINED) {
        if(oldCluster != data.clusterId) {
          oldCluster = data.clusterId;
          mComboBox->insertSeparator(mComboBox->count());
        }

        QVariant variant;
        variant = QVariant(toInt(data));
        mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), label, variant);
      }
    }
  }
  setValue(actSelected);
  mComboBox->blockSignals(false);
}

QString SettingComboBoxMultiClassificationUnmanaged::getName(settings::ClassificatorSettingOut key) const
{
  auto idx = mComboBox->findData(toInt(key), Qt::UserRole + 1);
  if(idx >= 0) {
    return mComboBox->itemText(idx);
  }
  return "";
}

settings::ObjectInputClustersExp SettingComboBoxMultiClassificationUnmanaged::getValue()
{
  settings::ObjectInputClustersExp toReturn;
  auto checked = (mComboBox)->getCheckedItems();

  for(const auto &[data, _] : checked) {
    toReturn.emplace(fromInt(data.toUInt()));
  }

  return toReturn;
}

void SettingComboBoxMultiClassificationUnmanaged::setValue(const settings::ObjectInputClustersExp &valueIn)
{
  QVariantList toCheck;
  for(const auto &value : valueIn) {
    toCheck.append(toInt(value));
  }
  (mComboBox)->setCheckedItems(toCheck);
}

std::map<settings::ClassificatorSettingOut, std::pair<std::string, std::string>> SettingComboBoxMultiClassificationUnmanaged::getValueAndNames()
{
  std::map<settings::ClassificatorSettingOut, std::pair<std::string, std::string>> toReturn;
  auto checked = ((QComboBoxMulti *) mComboBox)->getCheckedItems();

  for(const auto &[data, txt] : checked) {
    std::string clusterName;
    std::string className;
    auto split = txt.split("@");
    if(split.size() == 2) {
      clusterName = split[0].toStdString();
      className   = split[1].toStdString();
    }

    toReturn.emplace(fromInt(data.toUInt()), std::pair<std::string, std::string>{clusterName, className});
  }

  return toReturn;
}

void SettingComboBoxMultiClassificationUnmanaged::onValueChanged()
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
