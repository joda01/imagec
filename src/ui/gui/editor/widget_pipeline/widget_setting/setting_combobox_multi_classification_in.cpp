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

#include "setting_combobox_multi_classification_in.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/settings/settings_types.hpp"
#include "ui/gui/editor/widget_project_tabs/panel_classification.hpp"
#include "ui/gui/editor/window_main.hpp"

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

    // Add Temp
    mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), "Memory 01",
                       QVariant(toInt(enums::ClassIdIn::TEMP_01)));
    mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), "Memory 02",
                       QVariant(toInt(enums::ClassIdIn::TEMP_02)));
    mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), "Memory 03",
                       QVariant(toInt(enums::ClassIdIn::TEMP_03)));
    mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), "Memory 04",
                       QVariant(toInt(enums::ClassIdIn::TEMP_04)));
    mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), "Memory 05",
                       QVariant(toInt(enums::ClassIdIn::TEMP_05)));
    mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), "Memory 06",
                       QVariant(toInt(enums::ClassIdIn::TEMP_06)));
    mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), "Memory 07",
                       QVariant(toInt(enums::ClassIdIn::TEMP_07)));
    mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), "Memory 08",
                       QVariant(toInt(enums::ClassIdIn::TEMP_08)));
    mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), "Memory 09",
                       QVariant(toInt(enums::ClassIdIn::TEMP_09)));
    mComboBox->insertSeparator(mComboBox->count());

    auto classes = parent->getPanelClassification()->getClasses();
    std::map<std::string, std::multimap<std::string, enums::ClassId>> orderedClasses;
    for(const auto &dataIn : outputClasses) {
      QString className = classes[static_cast<enums::ClassIdIn>(dataIn)];
      orderedClasses[enums::getPrefixFromClassName(className.toStdString())].emplace(className.toStdString(), dataIn);
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

  for(const auto &[dataIn, _] : checked) {
    toReturn.emplace(fromInt(dataIn.toUInt()));
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
  auto checked = (static_cast<QComboBoxMulti *>(mComboBox))->getCheckedItems();

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
    SettingBase::triggerValueChanged((static_cast<QComboBoxMulti *>(mComboBox))->getDisplayText(), hasValueChanged, selectedIcon);
  } else {
    SettingBase::triggerValueChanged((static_cast<QComboBoxMulti *>(mComboBox))->getDisplayText(), hasValueChanged);
  }
}

}    // namespace joda::ui::gui
