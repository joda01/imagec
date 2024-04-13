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
#include <qtmetamacros.h>
#include <qvalidator.h>
#include <qvariant.h>
#include <qwidget.h>
#include <QSizePolicy>
#include <QtWidgets>
#include <cstddef>
#include <string>
#include <type_traits>
#include "container_function_base.hpp"

namespace joda::ui::qt {

template <typename T>
struct is_enum
{
  static constexpr bool value = std::is_enum<T>::value;
};

template <typename T>
concept IntFloatConcept = std::same_as<T, int> || std::same_as<T, float> || std::same_as<T, bool> ||
                          std::same_as<T, QString> || std::is_enum<T>::value;

template <IntFloatConcept VALUE_T, IntFloatConcept VALUE2_T>
class ContainerFunction : public ContainerFunctionBase
{
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

  ContainerFunction(const QString &icon, const QString &placeHolderText, const QString &helpText, const QString &unit,
                    std::optional<VALUE_T> defaultVal, VALUE_T minVal, VALUE_T maxVal, QWidget *parent = nullptr)

    requires std::same_as<VALUE_T, int> || std::same_as<VALUE_T, float> || std::is_enum<VALUE_T>::value
      : mUnit(unit), mDefaultValue(defaultVal)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, defaultVal, minVal, maxVal);
    comboxEditingFinished();
  }

  ContainerFunction(const QString &icon, const QString &placeHolderText, const QString &helpText, const QString &unit,
                    std::optional<VALUE_T> defaultVal, VALUE_T minVal, VALUE_T maxVal,
                    const std::vector<ComboEntry2> &optionsSecond, const VALUE2_T &comboSecondDefault,
                    QWidget *parent = nullptr)
    requires std::same_as<VALUE_T, int> || std::same_as<VALUE_T, float> || std::is_enum<VALUE_T>::value
      : mUnit(unit), mDefaultValue(defaultVal), mComboSecondDefaultValue(comboSecondDefault)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, defaultVal, minVal, maxVal, unit, optionsSecond);
    comboxEditingFinished();
  }

  ContainerFunction(const QString &icon, const QString &placeHolderText, const QString &helpText,
                    std::optional<VALUE_T> defaultVal, QWidget *parent = nullptr)
    requires std::same_as<VALUE_T, QString>
      : mUnit(""), mDefaultValue(defaultVal)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, defaultVal);
    comboxEditingFinished();
  }

  ContainerFunction(const QString &icon, const QString &placeHolderText, const QString &helpText, const QString &unit,
                    std::optional<VALUE_T> defaultVal, const std::vector<ComboEntry> &options,
                    QWidget *parent = nullptr)
    requires std::same_as<VALUE_T, QString> || std::same_as<VALUE_T, int> || std::is_enum<VALUE_T>::value
      : mUnit(unit), mDefaultValue(defaultVal)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, unit, options, {}, defaultVal);
    comboxEditingFinished();
  }

  ContainerFunction(const QString &icon, const QString &placeHolderText, const QString &helpText, const QString &unit,
                    std::optional<VALUE_T> defaultVal, const std::vector<ComboEntry> &options,
                    const std::vector<ComboEntry2> &optionsSecond, const VALUE2_T &comboSecondDefault,
                    QWidget *parent = nullptr)
    requires std::same_as<VALUE_T, QString> || std::same_as<VALUE_T, int> || std::is_enum<VALUE_T>::value
      : mUnit(unit), mDefaultValue(defaultVal), mComboSecondDefaultValue(comboSecondDefault)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, unit, options, optionsSecond, defaultVal);
    comboxEditingFinished();
  }

  ContainerFunction(const QString &icon, const QString &placeHolderText, const QString &helpText, bool defaultVal,
                    QWidget *parent = nullptr)
    requires std::same_as<VALUE_T, bool>
      : mUnit(""), mDefaultValue(defaultVal)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, "", {{0, "Off"}, {1, "On"}}, {}, defaultVal);
    comboxEditingFinished();
  }

  QWidget *getLabelWidget()
  {
    return mDisplayable;
  }

  QWidget *getEditableWidget()
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
      if constexpr(std::same_as<VALUE_T, QString>) {
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
    requires std::same_as<VALUE_T, int> || std::same_as<VALUE_T, float> || std::same_as<VALUE_T, QString> ||
             std::is_enum<VALUE_T>::value
  {
    if(mLineEdit != nullptr) {
      if constexpr(std::same_as<VALUE_T, QString>) {
        mLineEdit->setText(newValue);
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
    requires std::same_as<VALUE2_T, int> || std::same_as<VALUE2_T, float> || std::same_as<VALUE2_T, QString> ||
             std::is_enum<VALUE2_T>::value
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
  QString getValue()
    requires std::same_as<VALUE_T, QString>
  {
    try {
      if(mLineEdit != nullptr) {
        return mLineEdit->text();
      } else if(mComboBox != nullptr) {
        return mComboBox->currentData().toString();
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
  QString getValueSecond()
    requires std::same_as<VALUE2_T, QString>
  {
    try {
      if(mComboBoxSecond != nullptr) {
        return mComboBoxSecond->currentData().toString();
      } else {
        return "";
      }
    } catch(const std::exception &) {
      return "";
    }
  }

private:
  /////////////////////////////////////////////////////
  void createDisplayAbleWidget(const QString &icon, const QString &placeHolderText, const QString &helpText)
  {
    mDisplayable = new QWidget();

    // Create a QLabel
    mDisplayLabelIcon = new QLabel();
    mDisplayLabel     = new QLabel();

    // Set text for the label
    mDisplayLabel->setText(mDisplayText);
    mDisplayLabel->setToolTip(helpText);

    // Create a QPixmap for the icon (you may need to adjust the path)
    QIcon bmp(":/icons/outlined/" + icon);

    // Set the icon for the label
    mDisplayLabelIcon->setPixmap(bmp.pixmap(16, 16));    // You can adjust the size of the icon as needed
    mDisplayLabelIcon->setToolTip(helpText);

    // Create a QHBoxLayout to arrange the text and icon horizontally
    QHBoxLayout *layout = new QHBoxLayout;
    mDisplayable->setLayout(layout);
    layout->addWidget(mDisplayLabelIcon);
    layout->addWidget(mDisplayLabel);
    layout->addStretch();
  }

  void createEditableWidget(const QString &icon, const QString &placeHolderText, const QString &helpText,
                            std::optional<VALUE_T> defaultVal, VALUE_T min = 0, VALUE_T max = 0)
    requires std::same_as<VALUE_T, int> || std::same_as<VALUE_T, float> || std::same_as<VALUE_T, QString> ||
             std::is_enum<VALUE_T>::value
  {
    mEditable = new QWidget();
    mEditable->setObjectName("panelFunction");
    mEditable->setStyleSheet(
        "QLineEdit { border-radius: 4px; border: 1px solid rgba(32, 27, 23, 0.6); padding-top: 10px; padding-bottom: "
        "10px;}"
        "QWidget#panelFunction { background-color: rgba(0, 104, 117, 0);}");
    QVBoxLayout *layout = new QVBoxLayout(mParent);
    layout->setContentsMargins(8, 8, 8, 0);

    const QIcon myIcon(":/icons/outlined/" + icon);

    mLineEdit = new QLineEdit();
    QFont fontLineEdit;
    fontLineEdit.setPixelSize(16);
    mLineEdit->setFont(fontLineEdit);
    mLineEdit->setClearButtonEnabled(true);
    mLineEdit->addAction(QIcon(myIcon.pixmap(28, 28)), QLineEdit::LeadingPosition);
    mLineEdit->setPlaceholderText(placeHolderText);
    layout->addWidget(mLineEdit);
    // connect(mLineEdit, &QLineEdit::editingFinished, this, &ContainerFunction::lineEditingFinished);
    connect(mLineEdit, &QLineEdit::editingFinished, this, &ContainerFunction::lineEditingChanged);
    connect(mLineEdit, &QLineEdit::returnPressed, this, &ContainerFunction::lineEditingChanged);
    connect(mLineEdit, &QLineEdit::textChanged, this, &ContainerFunction::textChanged);

    if constexpr(std::same_as<VALUE_T, int> || std::same_as<VALUE_T, float>) {
      QValidator *validator;
      if constexpr(std::same_as<VALUE_T, int>) {
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

    createHelperText(layout, helpText);
    mEditable->setLayout(layout);
  }

  void createEditableWidget(const QString &icon, const QString &placeHolderText, const QString &helpText,
                            std::optional<VALUE_T> defaultVal, VALUE_T min, VALUE_T max, const QString &unit,
                            const std::vector<ComboEntry2> &optionsSecond)
    requires std::same_as<VALUE_T, QString> || std::same_as<VALUE_T, int> || std::same_as<VALUE_T, bool> ||
             std::is_enum<VALUE_T>::value
  {
    mEditable = new QWidget();
    mEditable->setObjectName("panelFunction");
    mEditable->setStyleSheet(
        "QLineEdit { border-radius: 4px; border: 1px solid rgba(32, 27, 23, 0.6); padding-top: 10px; padding-bottom: "
        "10px;}"
        "QComboBox {"
        "   border: 1px solid rgba(32, 27, 23, 0.6);"
        "   border-radius: 4px;"
        "   padding-top: 10px;"
        "   padding-bottom: 10px;"
        "   padding-left: 10px;"
        "   color: #333;"
        "   background-color: #fff;"
        "   selection-background-color: rgba(48,140,198,0.7);"
        "}"
        "QComboBox:editable {"
        "   background: #fff;"
        "   padding-left: 20px;"
        "}"

        "QComboBox::drop-down {"
        "   subcontrol-origin: padding;"
        "   subcontrol-position: right top;"
        "   width: 20px;"
        "   border-left: none;"
        "   border-radius: 4px 4px 4px 4px;"
        "   background: #fff;"
        "}"

        "QComboBox::down-arrow {"
        "   image: url(:/icons/outlined/icons8-sort-down-50.png);"
        "   width: 16px;"
        "   background: #fff;"
        "}"

        "QComboBox::down-arrow:on {"
        "   top: 1px;"
        "}"

        "QComboBox QAbstractItemView {"
        "   border: none;"
        "   background-color: #fff;"
        "}"
        "QWidget#panelFunction { background-color: rgba(0, 104, 117, 0);}");
    QVBoxLayout *layoutVertical = new QVBoxLayout(mParent);
    layoutVertical->setContentsMargins(8, 8, 8, 0);

    QWidget *horizontaContainer   = new QWidget();
    QHBoxLayout *layoutHorizontal = new QHBoxLayout(mParent);
    layoutHorizontal->setContentsMargins(0, 0, 0, 0);
    layoutHorizontal->setSpacing(4);

    const QIcon myIcon(":/icons/outlined/" + icon);
    mLineEdit = new QLineEdit();
    QFont fontLineEdit;
    fontLineEdit.setPixelSize(16);
    mLineEdit->setFont(fontLineEdit);
    mLineEdit->setClearButtonEnabled(true);
    mLineEdit->addAction(QIcon(myIcon.pixmap(28, 28)), QLineEdit::LeadingPosition);
    mLineEdit->setPlaceholderText(placeHolderText);
    layoutHorizontal->addWidget(mLineEdit);
    // connect(mLineEdit, &QLineEdit::editingFinished, this, &ContainerFunction::lineEditingFinished);
    connect(mLineEdit, &QLineEdit::editingFinished, this, &ContainerFunction::lineEditingChanged);
    connect(mLineEdit, &QLineEdit::returnPressed, this, &ContainerFunction::lineEditingChanged);
    connect(mLineEdit, &QLineEdit::textChanged, this, &ContainerFunction::textChanged);

    if constexpr(std::same_as<VALUE_T, int> || std::same_as<VALUE_T, float>) {
      QValidator *validator;
      if constexpr(std::same_as<VALUE_T, int>) {
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
    requires std::same_as<VALUE_T, QString> || std::same_as<VALUE_T, int> || std::same_as<VALUE_T, bool> ||
             std::is_enum<VALUE_T>::value
  {
    mEditable = new QWidget();
    mEditable->setObjectName("panelFunction");
    mEditable->setStyleSheet(
        "QComboBox {"
        "   border: 1px solid rgba(32, 27, 23, 0.6);"
        "   border-radius: 4px;"
        "   padding-top: 10px;"
        "   padding-bottom: 10px;"
        "   padding-left: 10px;"
        "   color: #333;"
        "   background-color: #fff;"
        "   selection-background-color: rgba(48,140,198,0.7);"
        "}"
        "QComboBox:editable {"
        "   background: #fff;"
        "   padding-left: 20px;"
        "}"

        "QComboBox::drop-down {"
        "   subcontrol-origin: padding;"
        "   subcontrol-position: right top;"
        "   width: 20px;"
        "   border-left: none;"
        "   border-radius: 4px 4px 4px 4px;"
        "   background: #fff;"
        "}"

        "QComboBox::down-arrow {"
        "   image: url(:/icons/outlined/icons8-sort-down-50.png);"
        "   width: 16px;"
        "   background: #fff;"
        "}"

        "QComboBox::down-arrow:on {"
        "   top: 1px;"
        "}"

        "QComboBox QAbstractItemView {"
        "   border: none;"
        "   background-color: #fff;"
        "}"
        "QWidget#panelFunction { background-color: rgba(0, 104, 117, 0);}");
    QVBoxLayout *layoutVertical = new QVBoxLayout(mParent);
    layoutVertical->setContentsMargins(8, 8, 8, 0);

    QWidget *horizontaContainer   = new QWidget();
    QHBoxLayout *layoutHorizontal = new QHBoxLayout(mParent);
    layoutHorizontal->setContentsMargins(0, 0, 0, 0);
    layoutHorizontal->setSpacing(4);

    const QIcon myIcon(":/icons/outlined/" + icon);
    mComboBox = new QComboBox();
    mComboBox->addAction(myIcon, "");    // const QIcon &icon, const QString &text
    QFont fontLineEdit;
    fontLineEdit.setPixelSize(16);
    for(const auto &data : options) {
      QVariant variant;
      if constexpr(std::is_enum<VALUE_T>::value) {
        variant = QVariant(static_cast<int>(data.key));
      } else {
        variant = QVariant(data.key);
      }
      if(data.icon.isEmpty()) {
        mComboBox->addItem(QIcon(myIcon.pixmap(28, 28)), data.label, variant);
      } else {
        const QIcon myIcon(":/icons/outlined/" + data.icon);
        mComboBox->addItem(QIcon(myIcon.pixmap(28, 28)), data.label, variant);
      }
    }
    mComboBox->setFont(fontLineEdit);
    mComboBox->setPlaceholderText(placeHolderText);
    layoutHorizontal->addWidget(mComboBox);
    connect(mComboBox, &QComboBox::currentIndexChanged, this, &ContainerFunction::comboxEditingFinished);
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
    layout->addWidget(helperText);
  }

  QComboBox *createSecondCombo(const std::vector<ComboEntry2> &optionsSecond)
  {
    mComboBoxSecond = new QComboBox();
    mComboBoxSecond->setObjectName("SecondCombo");
    QFont fontLineEdit;
    fontLineEdit.setPixelSize(16);
    for(const auto &data : optionsSecond) {
      QVariant variant;
      if constexpr(std::is_enum<VALUE2_T>::value) {
        variant = QVariant(static_cast<int>(data.key));
      } else {
        variant = QVariant(data.key);
      }
      mComboBoxSecond->addItem(data.label, variant);
    }
    mComboBoxSecond->setFont(fontLineEdit);
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
    connect(mComboBoxSecond, &QComboBox::currentIndexChanged, this, &ContainerFunction::comboxEditingFinished);

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

  QWidget *mParent      = nullptr;
  QWidget *mDisplayable = nullptr;
  QWidget *mEditable    = nullptr;

private slots:
  void lineEditingChanged()
  {
    if(mLineEdit != nullptr) {
      if(!mLineEdit->text().isNull() && !mLineEdit->text().isEmpty()) {
        mDisplayText = mLineEdit->text() + " " + mUnit;
      } else {
        mDisplayText = "- " + mUnit;
      }
      updateDisplayText();
      ContainerFunction<VALUE_T, VALUE2_T>::triggerValueChanged();
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
        mDisplayLabelIcon->setPixmap(selectedIcon.pixmap(16, 16));    // You can adjust the size of the icon as needed
      }
    }

    updateDisplayText();
    ContainerFunction<VALUE_T, VALUE2_T>::triggerValueChanged();
  }
};

}    // namespace joda::ui::qt
