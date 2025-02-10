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

#include "setting_combobox_classification_in.hpp"
#include <qcombobox.h>
#include "backend/enums/enums_classes.hpp"
#include "backend/settings/settings_types.hpp"
#include "ui/gui/window_main/window_main.hpp"

namespace joda::ui::gui {

QWidget *SettingComboBoxClassificationIn::createInputObject()
{
  mComboBox = new QComboBox();
  mComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  mComboBox->addAction(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE), "");

  classsNamesChanged();

  SettingBase::connect(mComboBox, &QComboBox::currentIndexChanged, this, &SettingComboBoxClassificationIn::onValueChanged);
  // SettingBase::connect(mComboBox, &QComboBox::currentTextChanged, this, &SettingComboBoxClassificationIn::onValueChanged);

  return mComboBox;
}

void SettingComboBoxClassificationIn::setDefaultValue(enums::ClassIdIn defaultVal)
{
  mDefaultValue = defaultVal;
  reset();
}

void SettingComboBoxClassificationIn::reset()
{
  if(mDefaultValue.has_value()) {
    setValue({mDefaultValue.value()});
  }
}

void SettingComboBoxClassificationIn::clear()
{
  mComboBox->setCurrentIndex(0);
}

void SettingComboBoxClassificationIn::classsNamesChanged()
{
  outputClassesChanges();
}

void SettingComboBoxClassificationIn::outputClassesChanges()
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

    // Add undefined classs
    mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), "Undefined",
                       QVariant(toInt(enums::ClassIdIn::UNDEFINED)));

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
    auto idx = mComboBox->findData(toInt(actSelected));
    if(idx >= 0) {
      mComboBox->setCurrentIndex(idx);
    }
    SettingBase::triggerValueChanged(mComboBox->currentText(), false);
    mComboBox->blockSignals(false);
  }
}

QString SettingComboBoxClassificationIn::getName(enums::ClassIdIn key) const
{
  auto idx = mComboBox->findData(toInt(key), Qt::UserRole + 1);
  if(idx >= 0) {
    return mComboBox->itemText(idx);
  }
  return "";
}

settings::ObjectInputClasss SettingComboBoxClassificationIn::getValue()
{
  return fromInt(mComboBox->currentData().toUInt());
}

void SettingComboBoxClassificationIn::setValue(const settings::ObjectInputClasss &valueIn)
{
  mComboBox->blockSignals(true);
  auto idx = mComboBox->findData(toInt(valueIn));
  if(idx >= 0) {
    mComboBox->setCurrentIndex(idx);
  }
  onValueChanged();
  mComboBox->blockSignals(false);
}

std::map<enums::ClassIdIn, std::string> SettingComboBoxClassificationIn::getValueAndNames()
{
  std::map<enums::ClassIdIn, std::string> toReturn;
  auto checked = ((QComboBoxMulti *) mComboBox)->getCheckedItems();

  for(const auto &[data, txt] : checked) {
    toReturn.emplace(fromInt(data.toUInt()), txt.toStdString());
  }

  return toReturn;
}

void SettingComboBoxClassificationIn::onValueChanged()
{
  bool hasValueChanged = true;
  if(mSetting != nullptr) {
    hasValueChanged = *mSetting != getValue();
    *mSetting       = getValue();
  }
  QVariant itemData = mComboBox->itemData(mComboBox->currentIndex(), Qt::DecorationRole);
  if(itemData.isValid() && itemData.canConvert<QIcon>()) {
    auto selectedIcon = qvariant_cast<QIcon>(itemData);
    SettingBase::triggerValueChanged(mComboBox->currentText(), hasValueChanged, selectedIcon);
  } else {
    SettingBase::triggerValueChanged(mComboBox->currentText(), hasValueChanged);
  }
}

}    // namespace joda::ui::gui
