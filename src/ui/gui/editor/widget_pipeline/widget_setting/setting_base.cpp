///
/// \file      container_function.cpp
/// \author    Joachim Danmayr
/// \date      2024-02-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "setting_base.hpp"
#include <iostream>
#include "backend/enums/enums_units.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/dialog_tooltip.hpp"
#include "ui/gui/editor/widget_project_tabs/panel_classification.hpp"
#include "ui/gui/editor/widget_project_tabs/panel_image.hpp"
#include "ui/gui/editor/window_main.hpp"
#include "ui/gui/helper/widget_generator.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
SettingBase::SettingBase(QWidget *parent, const QIcon &icon, const QString &description, int32_t maxTextLengthToDisplay) :
    mParent(dynamic_cast<WindowMain *>(parent)), mIcon(icon), mDescription(description), mMaxTextLengthToDisplay(maxTextLengthToDisplay)
{
  setObjectName("SettingBase");
  createDisplayAbleWidget(icon, description);
  if(parent != nullptr) {
    connect(mParent->getPanelClassification(), &PanelClassification::settingsChanged, this, &SettingBase::onClassificationNameChanged);
    connect(mParent, &WindowMain::onOutputClassifierChanges, this, &SettingBase::onOutputClassifierChanges);
  }
}
///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void SettingBase::blockAllSignals(bool bl)
{
  SettingBase::blockSignals(bl);
  blockComponentSignals(bl);
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
void SettingBase::changeUnit()
{
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
  CHECK_GUI_THREAD(mDisplayLabelIcon)
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
auto SettingBase::getIcon() -> const QIcon &
{
  return mIcon;
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
QWidget *SettingBase::getEditableWidget()
{
  return mEditable;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
QWidget *SettingBase::getInputObject()
{
  return mInputObject;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void SettingBase::triggerValueChanged(const QString &newValue, bool hasValueChanged, const QIcon &icon)
{
  mDisplayValue = newValue;
  if(!icon.isNull()) {
    mDisplayLabelIcon->setPixmap(icon.pixmap(DISP_ICON_SIZE, DISP_ICON_SIZE));    // You can adjust the size of the icon as needed
  }
  updateDisplayLabel();
  emit displayTextChanged();
  if(hasValueChanged) {
    emit valueChanged();
  }
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

  // Set the icon for the label
  displayLabelIcon->setPixmap(mIcon.pixmap(16, 16));    // You can adjust the size of the icon as needed
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
  auto txtTmp = (mShortDescription + mDisplayValue + " " + mUnit).trimmed();
  if(txtTmp.length() > mMaxTextLengthToDisplay) {
    txtTmp = txtTmp.left(mMaxTextLengthToDisplay - 3) + "...";
  }
  mDisplayLabel->setText(txtTmp);
}

///
/// \brief    This label is not editable but only shows the entered data
/// \author   Joachim Danmayr
///
void SettingBase::createDisplayAbleWidget(const QIcon &icon, const QString & /*tooltip*/)
{
  mDisplayable = new QWidget();
  mDisplayable->setContentsMargins(0, 0, 0, 0);
  mDisplayable->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
  // Create a QLabel
  mDisplayLabelIcon = new QLabel();
  mDisplayLabel     = new QLabel();

  // Set text for the label
  /// \todo the tooltip background is black whyever
  // mDisplayLabel->setToolTip(tooltip);

  // Create a QPixmap for the icon (you may need to adjust the path)
  if(!icon.isNull()) {
    mDisplayLabelIcon->setPixmap(mIcon.pixmap(DISP_ICON_SIZE, DISP_ICON_SIZE));
    /// \todo the tooltip background is black whyever
    // mDisplayLabelIcon->setToolTip(tooltip);
  }

  // Create a QHBoxLayout to arrange the text and icon horizontally
  auto *layout = new QHBoxLayout;
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(4);
  mDisplayable->setLayout(layout);
  if(!icon.isNull()) {
    layout->addWidget(mDisplayLabelIcon);
  }
  layout->addWidget(mDisplayLabel);
  layout->addStretch();
}

///
/// \brief    Create editable widget
/// \author   Joachim Danmayr
///
void SettingBase::createEditableWidget()
{
  mEditable = new QWidget();
  mEditable->setContentsMargins(0, 0, 0, 0);
  mInputObject = createInputObject();
  mInputObject->setObjectName("panelFunction");
  QVBoxLayout *layoutVertical = new QVBoxLayout();
  layoutVertical->setContentsMargins(0, 0, 0, 0);
  layoutVertical->setSpacing(0);

  QWidget *horizontaContainer   = new QWidget();
  QHBoxLayout *layoutHorizontal = new QHBoxLayout();
  layoutHorizontal->setContentsMargins(0, 0, 0, 0);
  layoutHorizontal->setSpacing(4);
  layoutHorizontal->addWidget(mInputObject);

  // if(!optionsSecond.empty()) {
  //   layoutHorizontal->addWidget(createSecondCombo(optionsSecond));
  // }
  horizontaContainer->setLayout(layoutHorizontal);
  layoutVertical->addWidget(horizontaContainer);

  createHelperText(layoutVertical, mDescription);
  mEditable->setLayout(layoutVertical);
}

///
/// \brief    Helper text
/// \author   Joachim Danmayr
///
void SettingBase::createHelperText(QVBoxLayout *layout, const QString &helpText)
{
  auto [label, button, hLayout] = createHelpTextLabel(helpText);
  mHelpButton                   = button;
  connect(mHelpButton, &QPushButton::clicked, this, &SettingBase::onHelpButtonClicked);

  layout->addLayout(hLayout);
}

///
/// \brief    Path to the help file
/// \author   Joachim Danmayr
///
void SettingBase::setPathToHelpFile(const QString &helpFilePath)
{
  mHelpFilePath = helpFilePath;
  if(!helpFilePath.isEmpty()) {
    CHECK_GUI_THREAD(mHelpButton)
    mHelpButton->setVisible(true);
  } else {
    CHECK_GUI_THREAD(mHelpButton)
    mHelpButton->setVisible(false);
  }
}
///
/// \brief    Open help dialog
/// \author   Joachim Danmayr
///
void SettingBase::onHelpButtonClicked()
{
  DialogToolTip tool(mParent, mDescription, mHelpFilePath);
  tool.exec();
}

///
/// \brief    Name of classs or class changed
/// \author   Joachim Danmayr
///
void SettingBase::onClassificationNameChanged()
{
  classsNamesChanged();
}

///
/// \brief    Name of classs or class changed
/// \author   Joachim Danmayr
///
void SettingBase::onOutputClassifierChanges()
{
  outputClassesChanges();
}

///
/// \brief   Returns classes from main
/// \author   Joachim Danmayr
///
[[nodiscard]] auto SettingBase::getClasses() const -> std::map<enums::ClassIdIn, QString>
{
  return mParent->getPanelClassification()->getClasses();
}

///
/// \brief   Returns the actual set unit
/// \author   Joachim Danmayr
///
[[nodiscard]] std::tuple<enums::Units, joda::ome::PhyiscalSize> SettingBase::getUnit() const
{
  if(mParent != nullptr) {
    auto unit                    = mParent->getSettings().pipelineSetup.realSizesUnit;
    auto [path, series, omeInfo] = mParent->getImagePanel()->getSelectedImageOrFirst();
    return {unit, omeInfo.getPhyiscalSize(series)};
  }
  return {};
}

}    // namespace joda::ui::gui
