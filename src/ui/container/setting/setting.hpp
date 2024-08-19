///
/// \file      container_function.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-23
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     This container holds the settings for one function
///            It can either be just displayed or edited
///

#pragma once

#include <qcombobox.h>
#include <qlabel.h>
#include <qlocale.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qtmetamacros.h>
#include <qvalidator.h>
#include <qvariant.h>
#include <qwidget.h>
#include <QSizePolicy>
#include <QtWidgets>
#include <cstddef>
#include <string>
#include <type_traits>
#include "ui/container/dialog_tooltip.hpp"
#include "setting_base.hpp"

namespace joda::ui {

template <typename T>
struct is_enum
{
  static constexpr bool value = std::is_enum<T>::value;
};

template <typename T>
concept IntFloatConcept = std::same_as<T, int> || std::same_as<T, uint32_t> || std::same_as<T, float> ||
                          std::same_as<T, bool> || std::same_as<T, std::string> || std::is_enum<T>::value;

template <IntFloatConcept VALUE_T, IntFloatConcept VALUE2_T>
class Setting : public SettingBase
{
private:
  static constexpr int32_t TXT_ICON_SIZE  = 16;
  static constexpr int32_t DISP_ICON_SIZE = 16;
  static constexpr int32_t HELP_ICON_SIZE = 8;

public:
  struct ComboEntry
  {
    VALUE_T key;
    QString label;
    QString icon;
  };

  struct ComboEntry2
  {
    VALUE2_T key;
    QString label;
  };

  Setting(const QString &icon, const QString &placeHolderText, const QString &helpText, const QString &unit,
          std::optional<VALUE_T> defaultVal, VALUE_T minVal, VALUE_T maxVal, QWidget *parent,
          const QString &pathToHelpFile = "")
    requires std::same_as<VALUE_T, int> || std::same_as<VALUE_T, uint32_t> || std::same_as<VALUE_T, float> ||
                 std::is_enum<VALUE_T>::value
      : mUnit(unit), mDefaultValue(defaultVal), mParent(parent), mHelpText(helpText), mPathToHelpFile(pathToHelpFile)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, defaultVal, minVal, maxVal);
    comboxEditingFinished();
  }

  Setting(const QString &icon, const QString &placeHolderText, const QString &helpText, const QString &unit,
          std::optional<VALUE_T> defaultVal, VALUE_T minVal, VALUE_T maxVal,
          const std::vector<ComboEntry2> &optionsSecond, const VALUE2_T &comboSecondDefault, QWidget *parent,
          const QString &pathToHelpFile = "")
    requires std::same_as<VALUE_T, int> || std::same_as<VALUE_T, uint32_t> || std::same_as<VALUE_T, float> ||
                 std::is_enum<VALUE_T>::value
      :
      mUnit(unit), mDefaultValue(defaultVal), mComboSecondDefaultValue(comboSecondDefault), mParent(parent),
      mHelpText(helpText), mPathToHelpFile(pathToHelpFile)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, defaultVal, minVal, maxVal, unit, optionsSecond);
    comboxEditingFinished();
  }

  Setting(const QString &icon, const QString &placeHolderText, const QString &helpText,
          std::optional<VALUE_T> defaultVal, QWidget *parent, const QString &pathToHelpFile = "")
    requires std::same_as<VALUE_T, std::string>
      : mUnit(""), mDefaultValue(defaultVal), mParent(parent), mHelpText(helpText), mPathToHelpFile(pathToHelpFile)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, defaultVal);
    comboxEditingFinished();
  }

  Setting(const QString &icon, const QString &placeHolderText, const QString &helpText, const QString &unit,
          std::optional<VALUE_T> defaultVal, const std::vector<ComboEntry> &options, QWidget *parent,
          const QString &pathToHelpFile = "")
    requires std::same_as<VALUE_T, std::string> || std::same_as<VALUE_T, int> || std::same_as<VALUE_T, uint32_t> ||
                 std::is_enum<VALUE_T>::value
      : mUnit(unit), mDefaultValue(defaultVal), mParent(parent), mHelpText(helpText), mPathToHelpFile(pathToHelpFile)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, unit, options, {}, defaultVal);
    comboxEditingFinished();
  }

  Setting(const QString &icon, const QString &placeHolderText, const QString &helpText, const QString &unit,
          std::optional<VALUE_T> defaultVal, const std::vector<ComboEntry> &options,
          const std::vector<ComboEntry2> &optionsSecond, const VALUE2_T &comboSecondDefault, QWidget *parent,
          const QString &pathToHelpFile = "")
    requires std::same_as<VALUE_T, std::string> || std::same_as<VALUE_T, int> || std::same_as<VALUE_T, uint32_t> ||
                 std::is_enum<VALUE_T>::value
      :
      mUnit(unit), mDefaultValue(defaultVal), mComboSecondDefaultValue(comboSecondDefault), mParent(parent),
      mHelpText(helpText), mPathToHelpFile(pathToHelpFile)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, unit, options, optionsSecond, defaultVal);
    comboxEditingFinished();
  }

  Setting(const QString &icon, const QString &placeHolderText, const QString &helpText, bool defaultVal,
          QWidget *parent, const QString &pathToHelpFile = "")
    requires std::same_as<VALUE_T, bool>
      : mUnit(""), mDefaultValue(defaultVal), mParent(parent), mHelpText(helpText), mPathToHelpFile(pathToHelpFile)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, "", {{0, "Off"}, {1, "On"}}, {}, defaultVal);
    comboxEditingFinished();
  }

  void connectWithSetting(VALUE_T *setting, VALUE2_T *setting2)
  {
    mValue1InSetting = setting;
    mValue2InSetting = setting2;
  }

  QWidget *getLabelWidget() override
  {
    return mDisplayable;
  }

  QWidget *getEditableWidget() override
  {
    return mEditable;
  }

  bool hasValue()
  {
    if(mLineEdit != nullptr) {
      return !mLineEdit->text().isEmpty();
    }
    if(mComboBox != nullptr) {
      if constexpr(std::same_as<VALUE_T, int> || std::is_enum<VALUE_T>::value) {
        return mComboBox->currentData().toInt() >= 0;
      }
      if constexpr(std::same_as<VALUE_T, uint32_t>) {
        return mComboBox->currentData().toUInt() >= 0;
      }
      if constexpr(std::same_as<VALUE_T, std::string>) {
        return mComboBox->currentData().toString() != "NONE" && mComboBox->currentData().toString() != "OFF";
      }
    }
    return true;
  }

  void resetToDefault()
  {
    if(mDefaultValue.has_value()) {
      setValue(mDefaultValue.value());
    } else {
      if(mComboBox != nullptr) {
        mComboBox->setCurrentIndex(0);
      }
      if(mLineEdit != nullptr) {
        mLineEdit->setText("");
      }
    }
    if(mComboBoxSecond != nullptr) {
      auto idx = mComboBoxSecond->findData(mComboSecondDefaultValue);
      if(idx >= 0) {
        mComboBoxSecond->setCurrentIndex(idx);
      }
    }
  }

  void clearValue()
  {
    if(mComboBox != nullptr) {
      mComboBox->setCurrentIndex(0);
    }
    if(mLineEdit != nullptr) {
      mLineEdit->setText("");
    }
    if(mComboBoxSecond != nullptr) {
      mComboBoxSecond->setCurrentIndex(0);
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  void setValue(VALUE_T newValue)
    requires std::same_as<VALUE_T, int> || std::same_as<VALUE_T, uint32_t> || std::same_as<VALUE_T, float> ||
             std::same_as<VALUE_T, std::string> || std::is_enum<VALUE_T>::value
  {
    if(mLineEdit != nullptr) {
      if constexpr(std::same_as<VALUE_T, std::string>) {
        mLineEdit->setText(QString(newValue.data()).trimmed());
      } else if constexpr(std::is_enum<VALUE_T>::value) {
        mLineEdit->setText(QString::number(static_cast<int>(newValue)));
      } else {
        mLineEdit->setText(QString::number(newValue));
      }
      lineEditingChanged();
    }
    if(mComboBox != nullptr) {
      int idx = -1;
      if constexpr(std::is_enum<VALUE_T>::value) {
        idx = mComboBox->findData(static_cast<int>(newValue));
      } else if constexpr(std::same_as<VALUE_T, std::string>) {
        idx = mComboBox->findData(newValue.data());
      } else {
        idx = mComboBox->findData(newValue);
      }

      if(idx >= 0) {
        mComboBox->setCurrentIndex(idx);
      } else {
        mComboBox->setCurrentIndex(0);
      }
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  void setValueSecond(VALUE2_T newValue)
    requires std::same_as<VALUE2_T, int> || std::same_as<VALUE2_T, uint32_t> || std::same_as<VALUE2_T, float> ||
             std::same_as<VALUE2_T, std::string> || std::is_enum<VALUE2_T>::value
  {
    if(mComboBoxSecond != nullptr) {
      int idx = -1;
      if constexpr(std::is_enum<VALUE2_T>::value) {
        idx = mComboBoxSecond->findData(static_cast<int>(newValue));
      } else {
        idx = mComboBoxSecond->findData(newValue);
      }
      if(idx >= 0) {
        mComboBoxSecond->setCurrentIndex(idx);
      } else {
        mComboBoxSecond->setCurrentIndex(0);
      }
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  void setValue(bool newValue)
    requires std::same_as<VALUE_T, bool>
  {
    if(mComboBox != nullptr) {
      if(newValue) {
        mComboBox->setCurrentIndex(1);
      } else {
        mComboBox->setCurrentIndex(0);
      }
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  int getValue()
    requires std::same_as<VALUE_T, int>
  {
    try {
      if(mLineEdit != nullptr) {
        return mLineEdit->text().toInt();
      }
      if(mComboBox != nullptr) {
        return mComboBox->currentData().toInt();
      }
      return 0;
    } catch(const std::exception &) {
      return 0;
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  int getValue()
    requires std::same_as<VALUE_T, uint32_t>
  {
    try {
      if(mLineEdit != nullptr) {
        return mLineEdit->text().toUInt();
      }
      if(mComboBox != nullptr) {
        return mComboBox->currentData().toUInt();
      }
      return 0;
    } catch(const std::exception &) {
      return 0;
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  VALUE_T getValue()
    requires std::is_enum<VALUE_T>::value
  {
    try {
      if(mLineEdit != nullptr) {
        return static_cast<VALUE_T>(mLineEdit->text().toInt());
      }
      if(mComboBox != nullptr) {
        return static_cast<VALUE_T>(mComboBox->currentData().toInt());
      }
      return static_cast<VALUE_T>(0);
    } catch(const std::exception &) {
      return static_cast<VALUE_T>(0);
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  bool getValue()
    requires std::same_as<VALUE_T, bool>
  {
    try {
      return mComboBox->currentData().toBool();
    } catch(const std::exception &) {
      return false;
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  float getValue()
    requires std::same_as<VALUE_T, float>
  {
    try {
      if(mLineEdit != nullptr) {
        return mLineEdit->text().toFloat();
      }
      if(mComboBox != nullptr) {
        return mComboBox->currentData().toFloat();
      }
      return 0.0;
    } catch(const std::exception &) {
      return 0.0;
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  std::string getValue()
    requires std::same_as<VALUE_T, std::string>
  {
    try {
      if(mLineEdit != nullptr) {
        return mLineEdit->text().toStdString();
      } else if(mComboBox != nullptr) {
        return mComboBox->currentData().toString().toStdString();
      } else {
        return "";
      }
    } catch(const std::exception &) {
      return "";
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  float getValueSecond()
    requires std::same_as<VALUE2_T, float>
  {
    try {
      if(mComboBoxSecond != nullptr) {
        return mComboBoxSecond->currentData().toFloat();
      }
      return 0.0;
    } catch(const std::exception &) {
      return 0.0;
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  int getValueSecond()
    requires std::same_as<VALUE2_T, int>
  {
    try {
      if(mComboBoxSecond != nullptr) {
        return mComboBoxSecond->currentData().toInt();
      }
      return 0.0;
    } catch(const std::exception &) {
      return 0.0;
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  int getValueSecond()
    requires std::same_as<VALUE2_T, uint32_t>
  {
    try {
      if(mComboBoxSecond != nullptr) {
        return mComboBoxSecond->currentData().toUInt();
      }
      return 0.0;
    } catch(const std::exception &) {
      return 0.0;
    }
  }
  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  VALUE2_T getValueSecond()
    requires std::is_enum<VALUE2_T>::value
  {
    try {
      if(mComboBoxSecond != nullptr) {
        return static_cast<VALUE2_T>(mComboBoxSecond->currentData().toInt());
      }
      return static_cast<VALUE2_T>(0);
    } catch(const std::exception &) {
      return static_cast<VALUE2_T>(0);
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  std::string getValueSecond()
    requires std::same_as<VALUE2_T, std::string>
  {
    try {
      if(mComboBoxSecond != nullptr) {
        return mComboBoxSecond->currentData().toString().toStdString();
      } else {
        return "";
      }
    } catch(const std::exception &) {
      return "";
    }
  }
  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  void setOptions(const QString &icon, const std::vector<ComboEntry> &options, const std::optional<VALUE_T> &defaultVal)
  {
    if(mComboBox != nullptr) {
      mComboBox->clear();
      mDefaultValue = defaultVal;
      const QIcon myIcon(":/icons/outlined/" + icon);
      for(const auto &data : options) {
        QVariant variant;
        if constexpr(std::is_enum<VALUE_T>::value) {
          variant = QVariant(static_cast<int>(data.key));
        } else {
          variant = QVariant(data.key);
        }
        if(data.icon.isEmpty()) {
          mComboBox->addItem(QIcon(myIcon.pixmap(TXT_ICON_SIZE, TXT_ICON_SIZE)), data.label, variant);
        } else {
          const QIcon myIcon(":/icons/outlined/" + data.icon);
          mComboBox->addItem(QIcon(myIcon.pixmap(TXT_ICON_SIZE, TXT_ICON_SIZE)), data.label, variant);
        }
      }
      int32_t idx = 0;
      if(defaultVal.has_value()) {
        if constexpr(std::is_enum<VALUE_T>::value) {
          idx = mComboBox->findData(static_cast<int>(defaultVal.value()));
        } else {
          idx = mComboBox->findData(defaultVal.value());
        }
      }
      if(idx >= 0) {
        mComboBox->setCurrentIndex(idx);
      }
    }
  }

  void setMaxLength(int maxLength)
  {
    if(mLineEdit != nullptr) {
      mLineEdit->setMaxLength(maxLength);
    }
  }

private:
  /////////////////////////////////////////////////////
  void createDisplayAbleWidget(const QString &icon, const QString &placeHolderText, const QString &helpText)
  {
    mDisplayable = new QWidget();
    mDisplayable->setContentsMargins(0, 0, 0, 0);
    mDisplayable->setMinimumWidth(110);
    mDisplayable->setMaximumWidth(110);

    // Create a QLabel
    mDisplayLabelIcon = new QLabel();
    mDisplayLabel     = new QLabel();

    // Set text for the label
    mDisplayLabel->setText(mDisplayText);
    mDisplayLabel->setToolTip(helpText);

    // Create a QPixmap for the icon (you may need to adjust the path)
    if(!icon.isEmpty()) {
      QIcon bmp(":/icons/outlined/" + icon);
      // Set the icon for the label
      mDisplayLabelIcon->setPixmap(
          bmp.pixmap(DISP_ICON_SIZE, DISP_ICON_SIZE));    // You can adjust the size of the icon as needed
      mDisplayLabelIcon->setToolTip(helpText);
    }

    // Create a QHBoxLayout to arrange the text and icon horizontally
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);
    mDisplayable->setLayout(layout);
    if(!icon.isEmpty()) {
      layout->addWidget(mDisplayLabelIcon);
    }
    layout->addWidget(mDisplayLabel);
    layout->addStretch();
  }

  void createEditableWidget(const QString &icon, const QString &placeHolderText, const QString &helpText,
                            std::optional<VALUE_T> defaultVal, VALUE_T min = {}, VALUE_T max = {})
    requires std::same_as<VALUE_T, int> || std::same_as<VALUE_T, uint32_t> || std::same_as<VALUE_T, float> ||
             std::same_as<VALUE_T, std::string> || std::is_enum<VALUE_T>::value
  {
    mEditable = new QWidget();
    mEditable->setContentsMargins(0, 0, 0, 0);
    mEditable->setObjectName("panelFunction");
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    mLineEdit = new QLineEdit();
    mLineEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    // mLineEdit->setClearButtonEnabled(true);

    if(!icon.isEmpty()) {
      const QIcon myIcon(":/icons/outlined/" + icon);
      mLineEdit->addAction(QIcon(myIcon.pixmap(TXT_ICON_SIZE, TXT_ICON_SIZE)), QLineEdit::LeadingPosition);
    }

    mLineEdit->setPlaceholderText(placeHolderText);
    layout->addWidget(mLineEdit);
    // connect(mLineEdit, &QLineEdit::editingFinished, this, &Setting::lineEditingFinished);
    connect(mLineEdit, &QLineEdit::editingFinished, this, &Setting::lineEditingChanged);
    connect(mLineEdit, &QLineEdit::returnPressed, this, &Setting::lineEditingChanged);
    connect(mLineEdit, &QLineEdit::textChanged, this, &Setting::textChanged);

    if constexpr(std::same_as<VALUE_T, int> || std::same_as<VALUE_T, uint32_t> || std::same_as<VALUE_T, float>) {
      QValidator *validator;
      if constexpr(std::same_as<VALUE_T, int>) {
        validator = new QIntValidator(min, max, mLineEdit);
      }
      if constexpr(std::same_as<VALUE_T, uint32_t>) {
        validator = new QIntValidator(min, max, mLineEdit);
      }
      if constexpr(std::same_as<VALUE_T, float>) {
        validator = new QDoubleValidator(min, max, 2, mLineEdit);
        ((QDoubleValidator *) validator)->setLocale(QLocale::C);
      }
      mLineEdit->setValidator(validator);
      if(defaultVal.has_value()) {
        mLineEdit->setText(QString::number(defaultVal.value()));
      }
    } else {
      if(defaultVal.has_value()) {
        mLineEdit->setText(defaultVal.value().data());
      }
    }
    lineEditingChanged();

    createHelperText(layout, helpText);
    mEditable->setLayout(layout);
  }

  void createEditableWidget(const QString &icon, const QString &placeHolderText, const QString &helpText,
                            std::optional<VALUE_T> defaultVal, VALUE_T min, VALUE_T max, const QString &unit,
                            const std::vector<ComboEntry2> &optionsSecond)
    requires std::same_as<VALUE_T, std::string> || std::same_as<VALUE_T, int> || std::same_as<VALUE_T, uint32_t> ||
             std::same_as<VALUE_T, bool> || std::is_enum<VALUE_T>::value
  {
    mEditable = new QWidget();
    mEditable->setContentsMargins(0, 0, 0, 0);
    mEditable->setObjectName("panelFunction");
    QVBoxLayout *layoutVertical = new QVBoxLayout();
    layoutVertical->setContentsMargins(0, 0, 0, 0);
    layoutVertical->setSpacing(0);

    QWidget *horizontaContainer   = new QWidget();
    QHBoxLayout *layoutHorizontal = new QHBoxLayout();
    layoutHorizontal->setContentsMargins(0, 0, 0, 0);
    layoutHorizontal->setSpacing(4);

    mLineEdit = new QLineEdit();
    // mLineEdit->setClearButtonEnabled(true);
    if(!icon.isEmpty()) {
      const QIcon myIcon(":/icons/outlined/" + icon);
      mLineEdit->addAction(QIcon(myIcon.pixmap(TXT_ICON_SIZE, TXT_ICON_SIZE)), QLineEdit::LeadingPosition);
    }
    mLineEdit->setPlaceholderText(placeHolderText);
    layoutHorizontal->addWidget(mLineEdit);
    // connect(mLineEdit, &QLineEdit::editingFinished, this, &Setting::lineEditingFinished);
    connect(mLineEdit, &QLineEdit::editingFinished, this, &Setting::lineEditingChanged);
    connect(mLineEdit, &QLineEdit::returnPressed, this, &Setting::lineEditingChanged);
    connect(mLineEdit, &QLineEdit::textChanged, this, &Setting::textChanged);

    if constexpr(std::same_as<VALUE_T, int> || std::same_as<VALUE_T, uint32_t> || std::same_as<VALUE_T, float>) {
      QValidator *validator;
      if constexpr(std::same_as<VALUE_T, int>) {
        validator = new QIntValidator(min, max, mLineEdit);
      }
      if constexpr(std::same_as<VALUE_T, uint32_t>) {
        validator = new QIntValidator(min, max, mLineEdit);
      }
      if constexpr(std::same_as<VALUE_T, float>) {
        validator = new QDoubleValidator(min, max, 2, mLineEdit);
        ((QDoubleValidator *) validator)->setLocale(QLocale::C);
      }
      mLineEdit->setValidator(validator);
      if(defaultVal.has_value()) {
        mLineEdit->setText(QString::number(defaultVal.value()));
      }
    } else {
      if(defaultVal.has_value()) {
        mLineEdit->setText(defaultVal.value());
      }
    }
    lineEditingChanged();

    if(!optionsSecond.empty()) {
      layoutHorizontal->addWidget(createSecondCombo(optionsSecond));
    }
    horizontaContainer->setLayout(layoutHorizontal);
    layoutVertical->addWidget(horizontaContainer);

    createHelperText(layoutVertical, helpText);
    mEditable->setLayout(layoutVertical);
  }

  void createEditableWidget(const QString &icon, const QString &placeHolderText, const QString &helpText,
                            const QString &unit, const std::vector<ComboEntry> &options,
                            const std::vector<ComboEntry2> &optionsSecond, const std::optional<VALUE_T> &defaultVal)
    requires std::same_as<VALUE_T, std::string> || std::same_as<VALUE_T, int> || std::same_as<VALUE_T, uint32_t> ||
             std::same_as<VALUE_T, bool> || std::is_enum<VALUE_T>::value
  {
    mEditable = new QWidget();
    mEditable->setContentsMargins(0, 0, 0, 0);
    mEditable->setObjectName("panelFunction");
    QVBoxLayout *layoutVertical = new QVBoxLayout();
    layoutVertical->setContentsMargins(0, 0, 0, 0);
    layoutVertical->setSpacing(0);

    QWidget *horizontaContainer   = new QWidget();
    QHBoxLayout *layoutHorizontal = new QHBoxLayout();
    layoutHorizontal->setContentsMargins(0, 0, 0, 0);
    layoutHorizontal->setSpacing(4);

    const QIcon myIcon(":/icons/outlined/" + icon);
    mComboBox = new QComboBox();
    mComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    mComboBox->addAction(myIcon, "");    // const QIcon &icon, const QString &text
    for(const auto &data : options) {
      QVariant variant;
      if constexpr(std::is_enum<VALUE_T>::value) {
        variant = QVariant(static_cast<int>(data.key));
      } else if constexpr(std::same_as<VALUE_T, std::string>) {
        variant = QVariant(data.key.data());
      } else {
        variant = QVariant(data.key);
      }
      if(data.icon.isEmpty()) {
        mComboBox->addItem(QIcon(myIcon.pixmap(TXT_ICON_SIZE, TXT_ICON_SIZE)), data.label, variant);
      } else {
        const QIcon myIcon(":/icons/outlined/" + data.icon);
        mComboBox->addItem(QIcon(myIcon.pixmap(TXT_ICON_SIZE, TXT_ICON_SIZE)), data.label, variant);
      }
    }
    mComboBox->setPlaceholderText(placeHolderText);
    layoutHorizontal->addWidget(mComboBox);
    connect(mComboBox, &QComboBox::currentIndexChanged, this, &Setting::comboxEditingFinished);
    int32_t idx = 0;
    if(defaultVal.has_value()) {
      if constexpr(std::is_enum<VALUE_T>::value) {
        idx = mComboBox->findData(static_cast<int>(defaultVal.value()));
      } else if constexpr(std::same_as<VALUE_T, std::string>) {
        idx = mComboBox->findData(defaultVal.value().data());
      } else {
        idx = mComboBox->findData(defaultVal.value());
      }
    }
    if(idx >= 0) {
      mComboBox->setCurrentIndex(idx);
    }

    if(!optionsSecond.empty()) {
      layoutHorizontal->addWidget(createSecondCombo(optionsSecond));
    }
    horizontaContainer->setLayout(layoutHorizontal);
    layoutVertical->addWidget(horizontaContainer);

    createHelperText(layoutVertical, helpText);
    mEditable->setLayout(layoutVertical);
  }

  void createHelperText(QVBoxLayout *layout, const QString &helpText)
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    auto *helperText = new QLabel();
    helperText->setObjectName("functionHelperText");
    helperText->setText(helpText);
    helperText->setContentsMargins(12, 0, 0, 0);
    // Create a QFont instance
    QFont font;
    font.setPixelSize(12);
    font.setItalic(true);
    font.setBold(false);
    font.setWeight(QFont::Light);
    helperText->setFont(font);
    helperText->setStyleSheet("QLabel#functionHelperText { color : #808080; }");
    hLayout->addWidget(helperText);

    // Info icon
    if(!mPathToHelpFile.isEmpty()) {
      auto *help = new QPushButton();
      help->setObjectName("helpButton");
      help->setStyleSheet(
          "QPushButton#helpButton {"
          "   background-color: rgba(0, 0, 0, 0);"
          "   border: 0px solid rgb(111, 121, 123);"
          "   color: rgb(0, 104, 117);"
          "   padding: 2px 2px 0px 0px;"
          "   margin: 0px 0px 0px 0px;"
          "   border-radius: 4px;"
          "   font-size: 8px;"
          "   font-weight: normal;"
          "   text-align: center;"
          "   text-decoration: none;"
          "   min-height: 0px;"
          "}");
      help->setCursor(Qt::PointingHandCursor);
      help->setIconSize({HELP_ICON_SIZE, HELP_ICON_SIZE});
      help->setIcon(QIcon(":/icons/outlined/icons8-info-48-fill.png"));
      connect(help, &QPushButton::clicked, this, &Setting::onHelpButtonClicked);
      hLayout->addWidget(help);
    }

    hLayout->addStretch(0);

    layout->addLayout(hLayout);
  }

  QComboBox *createSecondCombo(const std::vector<ComboEntry2> &optionsSecond)
  {
    mComboBoxSecond = new QComboBox();
    mComboBoxSecond->setObjectName("SecondCombo");
    mComboBoxSecond->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    for(const auto &data : optionsSecond) {
      QVariant variant;
      if constexpr(std::is_enum<VALUE2_T>::value) {
        variant = QVariant(static_cast<int>(data.key));
      } else {
        variant = QVariant(data.key);
      }
      mComboBoxSecond->addItem(data.label, variant);
    }
    mComboBoxSecond->setPlaceholderText("");
    int idx = -1;
    if constexpr(std::is_enum<VALUE2_T>::value) {
      idx = mComboBoxSecond->findData(static_cast<int>(mComboSecondDefaultValue));
    } else {
      idx = mComboBoxSecond->findData(mComboSecondDefaultValue);
    }
    if(idx >= 0) {
      mComboBoxSecond->setCurrentIndex(idx);
    }
    connect(mComboBoxSecond, &QComboBox::currentIndexChanged, this, &Setting::comboxEditingFinished);

    return mComboBoxSecond;
  }

  void updateDisplayText()
  {
    mDisplayLabel->setText(mDisplayText);
  }

  /////////////////////////////////////////////////////
  QString mDisplayText = "";
  std::optional<VALUE_T> mDefaultValue;
  VALUE2_T mComboSecondDefaultValue;

  /////////////////////////////////////////////////////
  QLineEdit *mLineEdit       = nullptr;
  QComboBox *mComboBox       = nullptr;
  QComboBox *mComboBoxSecond = nullptr;
  QLabel *mDisplayLabel      = nullptr;
  QLabel *mDisplayLabelIcon  = nullptr;
  QString mUnit;
  QString mHelpText;
  QString mPathToHelpFile;

  QWidget *mParent      = nullptr;
  QWidget *mDisplayable = nullptr;
  QWidget *mEditable    = nullptr;

  VALUE_T *mValue1InSetting  = nullptr;
  VALUE2_T *mValue2InSetting = nullptr;

private slots:

  void onHelpButtonClicked()
  {
    DialogToolTip tool(mParent, mHelpText, mPathToHelpFile);
    tool.exec();
  }

  void lineEditingChanged()
  {
    if(mLineEdit != nullptr) {
      if(!mLineEdit->text().isNull() && !mLineEdit->text().isEmpty()) {
        mDisplayText = mLineEdit->text() + " " + mUnit;
      } else {
        mDisplayText = "- " + mUnit;
      }

      if(mComboBoxSecond != nullptr) {
        if(!hasValue()) {
          mComboBoxSecond->setEnabled(false);
        } else {
          mComboBoxSecond->setEnabled(true);
        }
      }

      if(nullptr != mValue1InSetting) {
        *mValue1InSetting = getValue();
      }
      if(nullptr != mValue2InSetting) {
        *mValue2InSetting = getValueSecond();
      }

      updateDisplayText();
      Setting<VALUE_T, VALUE2_T>::triggerValueChanged();
    }
  }

  void textChanged(const QString &newText)
  {
    if(newText.isEmpty()) {
      lineEditingChanged();
    }
  }

  void comboxEditingFinished()
  {
    if(mComboBoxSecond != nullptr) {
      if(!hasValue()) {
        mComboBoxSecond->setEnabled(false);
      } else {
        mComboBoxSecond->setEnabled(true);
      }
    }

    if(mComboBox != nullptr) {
      if(mComboBoxSecond != nullptr && hasValue()) {
        if(mComboBox->currentText().isEmpty()) {
          mDisplayText = mComboBoxSecond->currentText();
        } else {
          mDisplayText = mComboBox->currentText() + " (" + mComboBoxSecond->currentText() + ")";
        }
      } else {
        mDisplayText = mComboBox->currentText();
      }

      QVariant itemData = mComboBox->itemData(mComboBox->currentIndex(), Qt::DecorationRole);
      if(itemData.isValid() && itemData.canConvert<QIcon>()) {
        QIcon selectedIcon = qvariant_cast<QIcon>(itemData);
        // Set the icon for the label
        if(!selectedIcon.isNull()) {
          mDisplayLabelIcon->setPixmap(
              selectedIcon.pixmap(DISP_ICON_SIZE, DISP_ICON_SIZE));    // You can adjust the size of the icon as needed
        }
      }
    }

    if(nullptr != mValue1InSetting) {
      *mValue1InSetting = getValue();
    }
    if(nullptr != mValue2InSetting) {
      *mValue2InSetting = getValueSecond();
    }

    updateDisplayText();
    Setting<VALUE_T, VALUE2_T>::triggerValueChanged();
  }
};

}    // namespace joda::ui
