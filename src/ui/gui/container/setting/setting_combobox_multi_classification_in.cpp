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

#include "backend/settings/settings_types.hpp"
#include "ui/gui/window_main/window_main.hpp"

namespace joda::ui::gui {

QWidget *SettingComboBoxMultiClassificationIn::createInputObject()
{
  mComboBox = new QComboBoxMulti();
  mComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  mComboBox->addAction(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE), "");

  classsNamesChanged();

  // SettingBase::connect(mComboBox, &QComboBoxMulti::currentIndexChanged, this, &SettingComboBoxMultiClassificationIn::onValueChanged);
  SettingBase::connect(mComboBox, &QComboBoxMulti::currentTextChanged, this, &SettingComboBoxMultiClassificationIn::onValueChanged);

  return mComboBox;
}

void SettingComboBoxMultiClassificationIn::setDefaultValue(joda::enums::ClassIdIn defaultVal)
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

void SettingComboBoxMultiClassificationIn::classsNamesChanged()
{
  outputClassesChanges();
}

void SettingComboBoxMultiClassificationIn::outputClassesChanges()
{
  auto *parent = getParent();
  if(parent != nullptr) {
    auto outputClasses = parent->getOutputClasses();

    mComboBox->blockSignals(true);
    auto actSelected = getValue();
    mComboBox->clear();

    // Add this classs
    mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), "Default",
                       QVariant(toInt(enums::ClassIdIn::$)));

    auto classes = parent->getPanelClassification()->getClasses();
    std::map<std::string, std::multimap<std::string, enums::ClassId>> orderedClasses;
    for(const auto &data : outputClasses) {
      QString className = classes[static_cast<enums::ClassIdIn>(data)];
      orderedClasses[enums::getPrefixFromClassName(className.toStdString())].emplace(className.toStdString(), data);
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
}

QString SettingComboBoxMultiClassificationIn::getName(joda::enums::ClassIdIn key) const
{
  auto idx = mComboBox->findData(toInt(key), Qt::UserRole + 1);
  if(idx >= 0) {
    return mComboBox->itemText(idx);
  }
  return "";
}

settings::ObjectInputClasses SettingComboBoxMultiClassificationIn::getValue()
{
  settings::ObjectInputClasses toReturn;
  auto checked = (mComboBox)->getCheckedItems();

  for(const auto &[data, _] : checked) {
    toReturn.emplace(fromInt(data.toUInt()));
  }

  return toReturn;
}

void SettingComboBoxMultiClassificationIn::setValue(const settings::ObjectInputClasses &valueIn)
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

std::map<joda::enums::ClassIdIn, std::string> SettingComboBoxMultiClassificationIn::getValueAndNames()
{
  std::map<joda::enums::ClassIdIn, std::string> toReturn;
  auto checked = ((QComboBoxMulti *) mComboBox)->getCheckedItems();

  for(const auto &[data, txt] : checked) {
    toReturn.emplace(fromInt(data.toUInt()), txt.toStdString());
  }

  return toReturn;
}

void SettingComboBoxMultiClassificationIn::onValueChanged()
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
