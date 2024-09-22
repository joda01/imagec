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

#include "setting_combobox_classes_out.hpp"
#include <string>
#include "ui/window_main/window_main.hpp"

namespace joda::ui {

QWidget *SettingComboBoxClassesOut::createInputObject()
{
  mComboBox = new QComboBox();
  mComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  mComboBox->addAction(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE), "");

  clusterNamesChanged();

  SettingBase::connect(mComboBox, &QComboBox::currentIndexChanged, this, &SettingComboBoxClassesOut::onValueChanged);
  SettingBase::connect(mComboBox, &QComboBox::currentTextChanged, this, &SettingComboBoxClassesOut::onValueChanged);

  return mComboBox;
}

void SettingComboBoxClassesOut::setDefaultValue(enums::ClassIdIn defaultVal)
{
  mDefaultValue = defaultVal;
  reset();
}

void SettingComboBoxClassesOut::reset()
{
  if(mDefaultValue.has_value()) {
    setValue({mDefaultValue.value()});
  }
}

void SettingComboBoxClassesOut::clear()
{
  mComboBox->setCurrentIndex(0);
}

void SettingComboBoxClassesOut::clusterNamesChanged()
{
  auto *parent = getParent();
  if(parent != nullptr) {
    mComboBox->blockSignals(true);
    auto actSelected = getValue();
    mComboBox->clear();
    auto [_, classes] = parent->getPanelClassification()->getClustersAndClasses();
    for(const auto &data : classes) {
      QVariant variant;
      variant = QVariant(toInt(data.first));
      if(!SettingBase::getIcon().isNull()) {
        mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), data.second, variant);
      } else {
        mComboBox->addItem(QIcon(QIcon(":/icons/icons/icons8-unknown-status-50.png").pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)),
                           data.second, variant);
      }
    }
    setValue(actSelected);
    mComboBox->blockSignals(false);
  }
}

QString SettingComboBoxClassesOut::getName(enums::ClassIdIn key) const
{
  auto idx = mComboBox->findData(toInt(key), Qt::UserRole + 1);
  if(idx >= 0) {
    return mComboBox->itemText(idx);
  }
  return "";
}

enums::ClassIdIn SettingComboBoxClassesOut::getValue()
{
  return fromInt(mComboBox->currentData().toUInt());
}

void SettingComboBoxClassesOut::setValue(const enums::ClassIdIn &valueIn)
{
  auto idx = mComboBox->findData(toInt(valueIn));
  if(idx >= 0) {
    (mComboBox)->setCurrentIndex(idx);
  }
}

std::pair<enums::ClassIdIn, std::string> SettingComboBoxClassesOut::getValueAndNames()
{
  return {fromInt(mComboBox->currentData().toUInt()), mComboBox->currentText().toStdString()};
}

void SettingComboBoxClassesOut::onValueChanged()
{
  if(mSetting != nullptr) {
    *mSetting = getValue();
  }
  QVariant itemData = mComboBox->itemData(mComboBox->currentIndex(), Qt::DecorationRole);
  if(itemData.isValid() && itemData.canConvert<QIcon>()) {
    auto selectedIcon = qvariant_cast<QIcon>(itemData);
    SettingBase::triggerValueChanged(mComboBox->currentText(), selectedIcon);
  } else {
    SettingBase::triggerValueChanged(mComboBox->currentText());
  }
}

}    // namespace joda::ui
