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

#include "setting_combobox_cluster_out.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui {

QWidget *SettingComboBoxClusterOut::createInputObject()
{
  mComboBox = new QComboBoxMulti();
  mComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  mComboBox->addAction(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE), "");

  clusterNamesChanged();

  SettingBase::connect(mComboBox, &QComboBoxMulti::currentIndexChanged, this,
                       &SettingComboBoxClusterOut::onValueChanged);
  SettingBase::connect(mComboBox, &QComboBoxMulti::currentTextChanged, this,
                       &SettingComboBoxClusterOut::onValueChanged);

  return mComboBox;
}

void SettingComboBoxClusterOut::setDefaultValue(enums::ClusterIdIn defaultVal)
{
  mDefaultValue = defaultVal;
  reset();
}

void SettingComboBoxClusterOut::reset()
{
  if(mDefaultValue.has_value()) {
    setValue({mDefaultValue.value()});
  }
}

void SettingComboBoxClusterOut::clear()
{
  mComboBox->setCurrentIndex(0);
}

void SettingComboBoxClusterOut::clusterNamesChanged()
{
  auto *parent = getParent();
  if(parent != nullptr) {
    mComboBox->blockSignals(true);
    auto actSelected = getValue();
    mComboBox->clear();
    auto [clusteres, _] = parent->getPanelClassification()->getClustersAndClasses();
    for(const auto &data : clusteres) {
      QVariant variant;
      variant = QVariant(toInt(data.first));
      mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)),
                         data.second, variant);
    }
    setValue(actSelected);
    mComboBox->blockSignals(false);
  }
}

QString SettingComboBoxClusterOut::getName(enums::ClusterIdIn key) const
{
  auto idx = mComboBox->findData(toInt(key), Qt::UserRole + 1);
  if(idx >= 0) {
    return mComboBox->itemText(idx);
  }
  return "";
}

enums::ClusterIdIn SettingComboBoxClusterOut::getValue()
{
  return fromInt(mComboBox->currentData().toUInt());
}

void SettingComboBoxClusterOut::setValue(const enums::ClusterIdIn &valueIn)
{
  auto idx = mComboBox->findData(toInt(valueIn));
  if(idx >= 0) {
    (mComboBox)->setCurrentIndex(idx);
  }
}

std::pair<enums::ClusterIdIn, std::string> SettingComboBoxClusterOut::getValueAndNames()
{
  return {fromInt(mComboBox->currentData().toUInt()), mComboBox->currentText().toStdString()};
}

void SettingComboBoxClusterOut::onValueChanged()
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
