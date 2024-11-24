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
#include "ui/window_main/window_main.hpp"

namespace joda::ui {

QWidget *SettingComboBoxClassificationIn::createInputObject()
{
  mComboBox = new QComboBox();
  mComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  mComboBox->addAction(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE), "");

  classsNamesChanged();

  SettingBase::connect(mComboBox, &QComboBox::currentIndexChanged, this, &SettingComboBoxClassificationIn::onValueChanged);
  SettingBase::connect(mComboBox, &QComboBox::currentTextChanged, this, &SettingComboBoxClassificationIn::onValueChanged);

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
    if(!outputClasses.empty()) {
      auto oldClasss = *outputClasses.begin();
      for(const auto &data : outputClasses) {
        if(data != enums::ClassId::UNDEFINED) {
          if(oldClasss != data) {
            oldClasss = data;
            mComboBox->insertSeparator(mComboBox->count());
          }

          QVariant variant;
          variant = QVariant(toInt(data));
          mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)),
                             classes[static_cast<enums::ClassIdIn>(data)], variant);
        }
      }
    }
    setValue(actSelected);
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
  auto idx = mComboBox->findData(toInt(valueIn));
  if(idx >= 0) {
    mComboBox->setCurrentIndex(idx);
  }
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
