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
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <QtWidgets>
#include <cstddef>
#include <string>
#include <unordered_map>

namespace joda::ui::qt {

template <typename T>
concept IntFloatConcept =
    std::same_as<T, int> || std::same_as<T, float> || std::same_as<T, bool> || std::same_as<T, QString>;

class ContainerFunctionBase : public QWidget
{
public:
};

template <IntFloatConcept VALUE_T>
class ContainerFunction : public ContainerFunctionBase
{
public:
  ContainerFunction(const QString &icon, const QString &placeHolderText, const QString &helpText, const QString &unit,
                    VALUE_T defaultVal, VALUE_T minVal, VALUE_T maxVal, QWidget *parent = nullptr)
    requires std::same_as<VALUE_T, int> || std::same_as<VALUE_T, float>
      : mUnit(unit)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, defaultVal, minVal, maxVal);
    comboxEditingFinished();
  }

  ContainerFunction(const QString &icon, const QString &placeHolderText, const QString &helpText, VALUE_T defaultVal,
                    QWidget *parent = nullptr)
    requires std::same_as<VALUE_T, QString>
      : mUnit("")
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, defaultVal);
    comboxEditingFinished();
  }

  ContainerFunction(const QString &icon, const QString &placeHolderText, const QString &helpText, const QString &unit,
                    VALUE_T defaultVal, const std::unordered_map<VALUE_T, QString> &options, QWidget *parent = nullptr)
    requires std::same_as<VALUE_T, QString> || std::same_as<VALUE_T, int>
      : mUnit(unit)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, unit, options, defaultVal);
    comboxEditingFinished();
  }

  ContainerFunction(const QString &icon, const QString &placeHolderText, const QString &helpText, bool defaultVal,
                    QWidget *parent = nullptr)
    requires std::same_as<VALUE_T, bool>
      : mUnit("")
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, "", {{0, "Off"}, {1, "On"}}, defaultVal);
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

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  int getValue()
    requires std::same_as<VALUE_T, int>
  {
    try {
      return mLineEdit->text().toInt();
    } catch(const std::exception &) {
      return 0;
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
      return mLineEdit->text().toInt();
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
      return mLineEdit->text().toFloat();
    } catch(const std::exception &) {
      return 0;
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
      return mLineEdit->text();
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
    QLabel *label = new QLabel();
    mDisplayLabel = new QLabel();

    // Set text for the label
    mDisplayLabel->setText(mDisplayText);
    mDisplayLabel->setToolTip(helpText);

    // Create a QPixmap for the icon (you may need to adjust the path)
    QIcon bmp(":/icons/" + icon);

    // Set the icon for the label
    label->setPixmap(bmp.pixmap(16, 16));    // You can adjust the size of the icon as needed
    label->setToolTip(helpText);

    // Create a QHBoxLayout to arrange the text and icon horizontally
    QHBoxLayout *layout = new QHBoxLayout;
    mDisplayable->setLayout(layout);
    layout->addWidget(label);
    layout->addWidget(mDisplayLabel);
    layout->addStretch();
  }

  void createEditableWidget(const QString &icon, const QString &placeHolderText, const QString &helpText,
                            VALUE_T defaultVal, VALUE_T min = 0, VALUE_T max = 0)
    requires std::same_as<VALUE_T, int> || std::same_as<VALUE_T, float> || std::same_as<VALUE_T, QString>
  {
    mEditable = new QWidget();
    mEditable->setObjectName("panelFunction");
    mEditable->setStyleSheet(
        "QLineEdit { border-radius: 4px; border: 1px solid rgba(32, 27, 23, 0.6); padding-top: 10px; padding-bottom: "
        "10px;}"
        "QWidget#panelFunction { background-color: rgba(0, 104, 117, 0);}");
    QVBoxLayout *layout = new QVBoxLayout(mParent);
    layout->setContentsMargins(8, 8, 8, 0);

    const QIcon myIcon(":/icons/" + icon);

    mLineEdit = new QLineEdit();
    QFont fontLineEdit;
    fontLineEdit.setPixelSize(16);
    mLineEdit->setFont(fontLineEdit);
    mLineEdit->setClearButtonEnabled(true);
    mLineEdit->addAction(myIcon, QLineEdit::LeadingPosition);
    mLineEdit->setPlaceholderText(placeHolderText);
    layout->addWidget(mLineEdit);
    // connect(mLineEdit, &QLineEdit::editingFinished, this, &ContainerFunction::lineEditingFinished);
    connect(mLineEdit, &QLineEdit::textChanged, this, &ContainerFunction::lineEditingChanged);
    if constexpr(std::same_as<VALUE_T, int> || std::same_as<VALUE_T, float>) {
      QIntValidator *validator = new QIntValidator(min, max, mLineEdit);
      mLineEdit->setValidator(validator);
      mLineEdit->setText(QString::number(defaultVal));
    } else {
      mLineEdit->setText(defaultVal);
    }

    createHelperText(layout, helpText);
    mEditable->setLayout(layout);
  }

  void createEditableWidget(const QString &icon, const QString &placeHolderText, const QString &helpText,
                            const QString &unit, const std::unordered_map<VALUE_T, QString> &options,
                            const VALUE_T &defaultVal)
    requires std::same_as<VALUE_T, QString> || std::same_as<VALUE_T, int> || std::same_as<VALUE_T, bool>
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
        "   min-width: 6em;"
        "   color: #333;"
        "   background-color: #fff;"
        "   selection-background-color: rgba(48,140,198,0.7);"
        "}"

        "QComboBox:editable {"
        "   background: #fff;"
        "   padding-left: 10px;"
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
        "   image: url(:/icons/icons8-sort-down-50.png);"
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
    QVBoxLayout *layout = new QVBoxLayout(mParent);
    layout->setContentsMargins(8, 8, 8, 0);

    const QIcon myIcon(":/icons/" + icon);

    mComboBox = new QComboBox();
    QFont fontLineEdit;
    fontLineEdit.setPixelSize(16);
    for(const auto &[key, label] : options) {
      mComboBox->insertItem(0, label, QVariant(key));    // unorderd_map stored items in reverse order
    }
    mComboBox->setFont(fontLineEdit);
    mComboBox->setPlaceholderText(placeHolderText);
    layout->addWidget(mComboBox);
    connect(mComboBox, &QComboBox::currentIndexChanged, this, &ContainerFunction::comboxEditingFinished);
    auto idx = mComboBox->findData(defaultVal);
    if(idx >= 0) {
      mComboBox->setCurrentIndex(idx);
    }

    createHelperText(layout, helpText);
    mEditable->setLayout(layout);
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

  void updateDisplayText()
  {
    mDisplayLabel->setText(mDisplayText);
  }

  /////////////////////////////////////////////////////
  QString mDisplayText = "";

  /////////////////////////////////////////////////////
  QLineEdit *mLineEdit  = nullptr;
  QComboBox *mComboBox  = nullptr;
  QLabel *mDisplayLabel = nullptr;
  QString mUnit;

  QWidget *mParent      = nullptr;
  QWidget *mDisplayable = nullptr;
  QWidget *mEditable    = nullptr;

private slots:
  void lineEditingChanged(const QString &text)
  {
    if(mLineEdit != nullptr) {
      if(!text.isNull() && !text.isEmpty()) {
        mDisplayText = text + " " + mUnit;
      } else {
        mDisplayText = "- " + mUnit;
      }
      updateDisplayText();
    }
  }

  void comboxEditingFinished()
  {
    if(mComboBox != nullptr) {
      mDisplayText = mComboBox->currentText();
      updateDisplayText();
    }
  }
};

}    // namespace joda::ui::qt
