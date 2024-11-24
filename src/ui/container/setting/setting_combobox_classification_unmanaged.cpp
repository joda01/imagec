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
  SettingBase::connect(mComboBox, &QComboBox::currentTextChanged, this, &SettingComboBoxClassificationUnmanaged::onValueChanged);

  return mComboBox;
}

void SettingComboBoxClassificationUnmanaged::addOptions(const std::map<joda::enums::ClassId, QString> &dataIn)
{
  mComboBox->blockSignals(true);
  auto actSelected = getValue();
  mComboBox->clear();
  if(!dataIn.empty()) {
    auto oldClasss = dataIn.begin()->first;
    for(const auto &[data, label] : dataIn) {
      if(oldClasss != data) {
        oldClasss = data;
        mComboBox->insertSeparator(mComboBox->count());
      }

      QVariant variant;
      variant = QVariant(toInt(data));
      mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), label, variant);
    }
  }
  setValue(actSelected);
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
  auto idx = mComboBox->findData(toInt(valueIn));
  if(idx >= 0) {
    mComboBox->setCurrentIndex(idx);
  }
}

std::pair<joda::enums::ClassId, std::pair<std::string, std::string>> SettingComboBoxClassificationUnmanaged::getValueAndNames()
{
  std::string classsName;
  std::string className;
  auto split = mComboBox->currentText().split("@");
  if(split.size() == 2) {
    classsName = split[0].toStdString();
    className  = split[1].toStdString();
  }
  return {fromInt(mComboBox->currentData().toUInt()), {classsName, className}};
}

void SettingComboBoxClassificationUnmanaged::onValueChanged()
{
  QVariant itemData = mComboBox->itemData(mComboBox->currentIndex(), Qt::DecorationRole);
  if(itemData.isValid() && itemData.canConvert<QIcon>()) {
    auto selectedIcon = qvariant_cast<QIcon>(itemData);
    SettingBase::triggerValueChanged(mComboBox->currentText(), selectedIcon);
  } else {
    SettingBase::triggerValueChanged(mComboBox->currentText());
  }
}

}    // namespace joda::ui
