///
/// \file      command.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "command.hpp"
#include <qlabel.h>
#include <qpushbutton.h>
#include <algorithm>
#include <string>
#include <type_traits>
#include "ui/window_main/window_main.hpp"

namespace joda::ui {

Command::Command(const QString &title, const QString &icon, QWidget *parent) :
    mParent(parent), mTitle(title), mLayout(&mEditView, true, true, false), mDisplayViewLayout(this)
{
  setContentsMargins(0, 0, 4, 0);
  mDisplayViewLayout.setContentsMargins(0, 0, 0, 0);
  setLayout(&mDisplayViewLayout);
  mDisplayViewLayout.setSpacing(4);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  // Header
  {
    // Create a QHBoxLayout to arrange the text and icon horizontally
    auto *headerWidget = new QWidget();
    headerWidget->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);
    headerWidget->setLayout(layout);
    auto *mDisplayLabelIcon = new QLabel();
    if(!icon.isEmpty()) {
      mIcon = QIcon(":/icons/outlined/" + icon);
      mDisplayLabelIcon->setPixmap(mIcon.pixmap(16, 16));    // You can adjust the size of the icon as needed
      layout->addWidget(mDisplayLabelIcon);
    }
    // layout->addWidget(new QLabel(title));
    // layout->addStretch();
    headerWidget->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
    mDisplayViewLayout.addWidget(headerWidget, 0, 0);
  }
  // Content
  {
    mDisplayableText.setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    mDisplayableText.setWordWrap(true);
    mDisplayViewLayout.addWidget(&mDisplayableText, 0, 1);
  }

  // Footer
  {
    QFont font;
    font.setPixelSize(12);
    font.setItalic(true);
    font.setBold(false);
    font.setWeight(QFont::Light);
    auto *label = new QLabel(title);
    label->setObjectName("functionHelperText");
    label->setFont(font);
    label->setStyleSheet("QLabel#functionHelperText { color : #808080; }");
    label->setContentsMargins(0, 0, 0, 0);
    label->setMaximumHeight(10);
    mDisplayViewLayout.addWidget(label, 1, 0, 1, 2);
  }

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  // Prepare edit dialog
  mEditView.setContentsMargins(0, 0, 0, 0);
  auto *layout = new QVBoxLayout();
  layout->addWidget(&mEditView);
  layout->setContentsMargins(0, 0, 0, 0);
  mEditDialog = new QDialog(mParent);
  mEditDialog->setModal(false);
  mEditDialog->setLayout(layout);
  mEditDialog->setMinimumWidth(300);
  mEditDialog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mEditDialog->setWindowTitle(title);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Command::registerDeleteButton(PanelPipelineSettings *pipelineSettingsUi)
{
  connect(mLayout.getDeleteButton(), &QAction::triggered, [this, pipelineSettingsUi]() {
    QMessageBox messageBox(mParent);
    auto *icon = new QIcon(":/icons/outlined/icons8-warning-50.png");
    messageBox.setIconPixmap(icon->pixmap(42, 42));
    messageBox.setWindowTitle("Delete command?");
    messageBox.setText("Delete command from pipeline?");
    QPushButton *noButton  = messageBox.addButton(tr("No"), QMessageBox::NoRole);
    QPushButton *yesButton = messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
    messageBox.setDefaultButton(noButton);
    auto reply = messageBox.exec();
    if(messageBox.clickedButton() == noButton) {
      return;
    }

    mEditDialog->close();
    pipelineSettingsUi->erasePipelineStep(this);
  });
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
helper::TabWidget *Command::addTab(const QString &title, std::function<void()> beforeTabClose)
{
  auto *tab = mLayout.addTab(title, beforeTabClose);
  if(mEditDialog->isVisible()) {
    std::thread([this] {
      std::this_thread::sleep_for(100ms);
      mEditDialog->adjustSize();
    }).detach();
  }
  return tab;
}

helper::VerticalPane *Command::addSetting(helper::TabWidget *tab, const QString &boxTitle,
                                          const std::vector<std::pair<SettingBase *, bool>> &settings,
                                          helper::VerticalPane *col)
{
  auto containsPtr = [&](SettingBase *toCheck) {
    for(const auto &[ptr, _] : mSettings) {
      if(ptr == toCheck) {
        return true;
      }
    }
    return false;
  };

  for(auto &[data, bo] : settings) {
    if(!containsPtr(data)) {
      mSettings.emplace_back(data, bo);

      {
        auto *casted = dynamic_cast<SettingComboBox<enums::ClusterIdIn> *>(data);
        if(casted) {
          mClusters.emplace_back(casted);
        }
      }

      {
        auto *casted = dynamic_cast<SettingComboBox<enums::ClassId> *>(data);
        if(casted) {
          mClasses.emplace_back(casted);
        }
      }

      {
        auto *casted = dynamic_cast<SettingComboBoxMulti<enums::ClusterIdIn> *>(data);
        if(casted) {
          mClustersMulti.emplace_back(casted);
        }
      }

      {
        auto *casted = dynamic_cast<SettingComboBoxMulti<enums::ClassId> *>(data);
        if(casted) {
          mClassesMulti.emplace_back(casted);
        }
      }
    }
  }
  auto convert = [&]() {
    std::vector<SettingBase *> vec;
    std::transform(settings.begin(), settings.end(), std::back_inserter(vec), [](auto &kv) { return kv.first; });
    return vec;
  };

  if(nullptr == col) {
    col = tab->addVerticalPanel();
  }
  if(boxTitle.isEmpty()) {
    col->addGroup(convert(), 220, 300);
  } else {
    col->addGroup(boxTitle, convert(), 220, 300);
  }
  for(const auto &[setting, show] : settings) {
    setting->setDisplayIconVisible(false);
    connect(setting, &SettingBase::valueChanged, this, &Command::valueChanged);
  }
  updateDisplayText();
  updateClassesAndClusters();
  connect(this, &Command::valueChanged, this, &Command::updateDisplayText);
  return col;
}

void Command::updateDisplayText()
{
  QString txt;
  for(const auto &[setting, show] : mSettings) {
    if(show) {
      if(setting == nullptr) {
        continue;
      }
      if(!setting->getDisplayLabelText().isEmpty()) {
        txt = txt + setting->getDisplayLabelText() + ", ";
      }
    }
  }
  txt.chop(2);
  mDisplayableText.setText(txt);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Command::updateClassesAndClusters()
{
  if(mParent != nullptr) {
    auto [clusterNames, classNames] = ((WindowMain *) mParent)->getPanelClassification()->getClustersAndClasses();
    updateClassesAndClusterNames(clusterNames, classNames);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Command::updateClassesAndClusterNames(const std::map<enums::ClusterIdIn, QString> &clusterNames,
                                           const std::map<enums::ClassId, QString> &classNames)
{
  for(auto &cluster : mClusters) {
    cluster->changeOptionText(clusterNames);
  }

  for(auto &classs : mClasses) {
    classs->changeOptionText(classNames);
  }

  for(auto &cluster : mClustersMulti) {
    cluster->changeOptionText(clusterNames);
  }

  for(auto &classs : mClassesMulti) {
    classs->changeOptionText(classNames);
  }
}

}    // namespace joda::ui
