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

#include "setting_combobox_classification_unmanaged.hpp"
#include "backend/enums/enums_classes.hpp"

#include "ui/window_main/window_main.hpp"

namespace joda::ui {

QWidget *SettingComboBoxClassificationUnmanaged::createInputObject()
{
  mComboBox = new QComboBox();
  mComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  mComboBox->addAction(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE), "");

  SettingBase::connect(mComboBox, &QComboBox::currentIndexChanged, this, &SettingComboBoxClassificationUnmanaged::onValueChanged);
  // SettingBase::connect(mComboBox, &QComboBox::currentTextChanged, this, &SettingComboBoxClassificationUnmanaged::onValueChanged);

  return mComboBox;
}

void SettingComboBoxClassificationUnmanaged::addOptions(const std::map<joda::enums::ClassId, QString> &dataIn)
{
  mComboBox->blockSignals(true);
  auto actSelected = getValue();
  mComboBox->clear();

  std::map<std::string, std::multimap<std::string, enums::ClassId>> orderedClasses;
  for(const auto &[id, className] : dataIn) {
    orderedClasses[enums::getPrefixFromClassName(className.toStdString())].emplace(className.toStdString(), id);
  }

  for(const auto &[prefix, group] : orderedClasses) {
    for(const auto &[className, id] : group) {
      QVariant variant;
      variant = QVariant(toInt(id));
      mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), className.data(), variant);
    }
    mComboBox->insertSeparator(mComboBox->count());
  }
  auto removeLastSeparator = [this]() {
    int lastIndex = mComboBox->count() - 1;
    if(lastIndex >= 0) {
      mComboBox->removeItem(lastIndex);
    }
  };
  removeLastSeparator();
  auto idx = mComboBox->findData(toInt(actSelected));
  if(idx >= 0) {
    mComboBox->setCurrentIndex(idx);
  }
  SettingBase::triggerValueChanged(mComboBox->currentText(), false);
  mComboBox->blockSignals(false);
}

void SettingComboBoxClassificationUnmanaged::setDefaultValue(joda::enums::ClassId defaultVal)
{
  mDefaultValue = defaultVal;
  reset();
}

void SettingComboBoxClassificationUnmanaged::reset()
{
  if(mDefaultValue.has_value()) {
    setValue({mDefaultValue.value()});
  }
}

void SettingComboBoxClassificationUnmanaged::clear()
{
  mComboBox->setCurrentIndex(0);
}

QString SettingComboBoxClassificationUnmanaged::getName(joda::enums::ClassId key) const
{
  auto idx = mComboBox->findData(toInt(key), Qt::UserRole + 1);
  if(idx >= 0) {
    return mComboBox->itemText(idx);
  }
  return "";
}

joda::enums::ClassId SettingComboBoxClassificationUnmanaged::getValue()
{
  return fromInt(mComboBox->currentData().toUInt());
}

void SettingComboBoxClassificationUnmanaged::setValue(const joda::enums::ClassId &valueIn)
{
  mComboBox->blockSignals(true);
  auto idx = mComboBox->findData(toInt(valueIn));
  if(idx >= 0) {
    mComboBox->setCurrentIndex(idx);
  }
  onValueChanged();
  mComboBox->blockSignals(false);
}

std::pair<joda::enums::ClassId, std::pair<std::string, std::string>> SettingComboBoxClassificationUnmanaged::getValueAndNames()
{
  std::string className = mComboBox->currentText().toStdString();
  return {fromInt(mComboBox->currentData().toUInt()), {className, className}};
}

void SettingComboBoxClassificationUnmanaged::onValueChanged()
{
  QVariant itemData = mComboBox->itemData(mComboBox->currentIndex(), Qt::DecorationRole);
  if(itemData.isValid() && itemData.canConvert<QIcon>()) {
    auto selectedIcon = qvariant_cast<QIcon>(itemData);
    SettingBase::triggerValueChanged(mComboBox->currentText(), true, selectedIcon);
  } else {
    SettingBase::triggerValueChanged(mComboBox->currentText(), true);
  }
}

}    // namespace joda::ui
