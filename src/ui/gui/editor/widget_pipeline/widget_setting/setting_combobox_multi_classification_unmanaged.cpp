///
/// \file      setting_line_Edit.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-22
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "setting_combobox_multi_classification_unmanaged.hpp"
#include "backend/enums/enums_classes.hpp"

#include "ui/gui/editor/window_main.hpp"

namespace joda::ui::gui {

QWidget *SettingComboBoxMultiClassificationUnmanaged::createInputObject()
{
  mComboBox = new QComboBoxMulti();
  mComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  mComboBox->addAction(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE), "");

  // SettingBase::connect(mComboBox, &QComboBoxMulti::currentIndexChanged, this, &SettingComboBoxMultiClassificationUnmanaged::onValueChanged);
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
  QVariantList toCheck;
  for(const auto &value : actSelected) {
    toCheck.append(toInt(value));
  }
  (mComboBox)->setCheckedItems(toCheck);
  SettingBase::triggerValueChanged(mComboBox->currentText(), false);
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
  mComboBox->blockSignals(true);
  QVariantList toCheck;
  for(const auto &value : valueIn) {
    toCheck.append(toInt(value));
  }
  (mComboBox)->setCheckedItems(toCheck);
  onValueChanged();
  mComboBox->blockSignals(false);
}

std::map<joda::enums::ClassId, std::pair<std::string, std::string>> SettingComboBoxMultiClassificationUnmanaged::getValueAndNames()
{
  std::map<joda::enums::ClassId, std::pair<std::string, std::string>> toReturn;
  auto checked = ((QComboBoxMulti *) mComboBox)->getCheckedItems();

  for(const auto &[data, txt] : checked) {
    toReturn.emplace(fromInt(data.toUInt()), std::pair<std::string, std::string>{txt.toStdString(), txt.toStdString()});
  }

  return toReturn;
}

void SettingComboBoxMultiClassificationUnmanaged::onValueChanged()
{
  bool hasValueChanged = true;
  if(mSetting != nullptr) {
    hasValueChanged = *mSetting != getValue();
    *mSetting       = getValue();
  }
  QVariant itemData = mComboBox->itemData(mComboBox->currentIndex(), Qt::DecorationRole);
  if(itemData.isValid() && itemData.canConvert<QIcon>()) {
    auto selectedIcon = qvariant_cast<QIcon>(itemData);
    SettingBase::triggerValueChanged(((QComboBoxMulti *) mComboBox)->getDisplayText(), hasValueChanged, selectedIcon);
  } else {
    SettingBase::triggerValueChanged(((QComboBoxMulti *) mComboBox)->getDisplayText(), hasValueChanged);
  }
}

}    // namespace joda::ui::gui
