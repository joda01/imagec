///
/// \file      container_function.cpp
/// \author    Joachim Danmayr
/// \date      2024-02-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "setting_base.hpp"

namespace joda::ui {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
SettingBase::SettingBase(const QString &icon, const QString &description)
{
  createDisplayAbleWidget(icon, description);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void SettingBase::setUnit(const QString &unit)
{
  mUnit = unit;
  updateDisplayLabel();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void SettingBase::setDisplayIconVisible(bool visible)
{
  mDisplayLabelIcon->setVisible(visible);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void SettingBase::setShortDescription(const QString &description)
{
  mShortDescription = description;
  updateDisplayLabel();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
QString SettingBase::getDisplayLabelText() const
{
  return mDisplayLabel->text();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
QWidget *SettingBase::getDisplayLabelWidget()
{
  return mDisplayable;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void SettingBase::triggerValueChanged(const QString &newValue)
{
  mDisplayValue = newValue;
  updateDisplayLabel();
  emit valueChanged();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
QWidget *SettingBase::createDisplayAbleWidgetPlaceholder()
{
  QWidget *displayable = new QWidget();
  displayable->setContentsMargins(0, 0, 0, 0);
  displayable->setMinimumWidth(110);
  displayable->setMaximumWidth(110);

  // Create a QLabel
  QLabel *displayLabelIcon = new QLabel();
  QLabel *displayLabel     = new QLabel();

  // Set text for the label
  displayLabel->setText("");
  displayLabel->setToolTip("");

  // Create a QPixmap for the icon (you may need to adjust the path)
  QIcon bmp(":/icons/outlined/");

  // Set the icon for the label
  displayLabelIcon->setPixmap(bmp.pixmap(16, 16));    // You can adjust the size of the icon as needed
  displayLabelIcon->setToolTip("");

  // Create a QHBoxLayout to arrange the text and icon horizontally
  QHBoxLayout *layout = new QHBoxLayout;
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(4);
  displayable->setLayout(layout);
  layout->addWidget(displayLabelIcon);
  layout->addWidget(displayLabel);
  layout->addStretch();
  return displayable;
}

///
/// \brief    Update the display text
/// \author   Joachim Danmayr
///
void SettingBase::updateDisplayLabel()
{
  mDisplayLabel->setText((mShortDescription + mDisplayValue + " " + mUnit).trimmed());
}

///
/// \brief    This label is not editable but only shows the entered data
/// \author   Joachim Danmayr
///
void SettingBase::createDisplayAbleWidget(const QString &icon, const QString &tooltip)
{
  mDisplayable = new QWidget();
  mDisplayable->setContentsMargins(0, 0, 0, 0);
  mDisplayable->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
  // Create a QLabel
  mDisplayLabelIcon = new QLabel();
  mDisplayLabel     = new QLabel();

  // Set text for the label
  mDisplayLabel->setToolTip(tooltip);

  // Create a QPixmap for the icon (you may need to adjust the path)
  if(!icon.isEmpty()) {
    QIcon bmp(":/icons/outlined/" + icon);
    mDisplayLabelIcon->setPixmap(bmp.pixmap(DISP_ICON_SIZE, DISP_ICON_SIZE));
    mDisplayLabelIcon->setToolTip(tooltip);
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

}    // namespace joda::ui
