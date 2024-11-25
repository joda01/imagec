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

void SettingComboBoxMultiClassificationUnmanaged::setDefaultValue(joda::enums::ClassId defaultVal)
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

void SettingComboBoxMultiClassificationUnmanaged::addOptions(const std::map<joda::enums::ClassId, QString> &dataIn)
{
  mComboBox->blockSignals(true);
  auto actSelected = getValue();
  mComboBox->clear();

  auto getPrefix = [](const QString &className) -> QString {
    auto areas = className.trimmed().split(" ");
    if(areas.size() > 1) {
      return areas[0].trimmed();
    }
    return "";
  };

  std::map<std::string, std::multimap<std::string, enums::ClassId>> orderedClasses;
  for(const auto &[id, className] : dataIn) {
    orderedClasses[getPrefix(className).toStdString()].emplace(className.toStdString(), id);
  }

  for(const auto &[prefix, group] : orderedClasses) {
    for(const auto &[className, id] : group) {
      QVariant variant;
      variant = QVariant(toInt(id));
      mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), className.data(), variant);
    }
    mComboBox->insertSeparator(mComboBox->count());
  }
  setValue(actSelected);
  mComboBox->blockSignals(false);
}

QString SettingComboBoxMultiClassificationUnmanaged::getName(joda::enums::ClassId key) const
{
  auto idx = mComboBox->findData(toInt(key), Qt::UserRole + 1);
  if(idx >= 0) {
    return mComboBox->itemText(idx);
  }
  return "";
}

settings::ObjectInputClassesExp SettingComboBoxMultiClassificationUnmanaged::getValue()
{
  settings::ObjectInputClassesExp toReturn;
  auto checked = (mComboBox)->getCheckedItems();

  for(const auto &[data, _] : checked) {
    toReturn.emplace(fromInt(data.toUInt()));
  }

  return toReturn;
}

void SettingComboBoxMultiClassificationUnmanaged::setValue(const settings::ObjectInputClassesExp &valueIn)
{
  QVariantList toCheck;
  for(const auto &value : valueIn) {
    toCheck.append(toInt(value));
  }
  (mComboBox)->setCheckedItems(toCheck);
}

std::map<joda::enums::ClassId, std::pair<std::string, std::string>> SettingComboBoxMultiClassificationUnmanaged::getValueAndNames()
{
  std::map<joda::enums::ClassId, std::pair<std::string, std::string>> toReturn;
  auto checked = ((QComboBoxMulti *) mComboBox)->getCheckedItems();

  for(const auto &[data, txt] : checked) {
    std::string classsName;
    std::string className;
    auto split = txt.split("@");
    if(split.size() == 2) {
      classsName = split[0].toStdString();
      className  = split[1].toStdString();
    }

    toReturn.emplace(fromInt(data.toUInt()), std::pair<std::string, std::string>{classsName, className});
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
