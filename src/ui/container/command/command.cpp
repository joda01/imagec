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
#include <qcolor.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <algorithm>
#include <cstddef>
#include <string>
#include <type_traits>
#include "ui/container/pipeline/add_command_button.hpp"
#include "ui/helper/icon_generator.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui {

WrapLabel::WrapLabel(InOut inout) : QLabel(), inout(inout)
{
  setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
  setWordWrap(true);
  setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
}

void WrapLabel::resizeEvent(QResizeEvent *event)
{
  QLabel::resizeEvent(event);
}

Command::Command(joda::settings::PipelineStep &pipelineStep, const QString &title, const QString &icon, QWidget *parent, InOut type) :
    mPipelineStep(pipelineStep), mParent(parent), mTitle(title), mLayout(&mEditView, true, true, false), mDisplayViewLayout(this), mInOut(type)
{
  setContentsMargins(0, 0, 4, 0);
  mDisplayViewLayout.setContentsMargins(0, 0, 0, 0);
  setLayout(&mDisplayViewLayout);
  mDisplayViewLayout.setSpacing(4);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

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
      mIcon = generateIcon(icon);
      mDisplayLabelIcon->setPixmap(mIcon.pixmap(16, 16));    // You can adjust the size of the icon as needed
      layout->addWidget(mDisplayLabelIcon);
    }
    // layout->addWidget(new QLabel(title));
    // layout->addStretch();
    headerWidget->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    mDisplayViewLayout.addWidget(headerWidget, 0, 0);
  }
  // Content
  {
    mDisplayableText = new WrapLabel(type);
    mDisplayViewLayout.addWidget(mDisplayableText, 0, 1);
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

void Command::paintEvent(QPaintEvent *event)
{
  auto getColor = [](InOuts inouts) {
    switch(inouts) {
      case InOuts::ALL:
        return Qt::lightGray;
      case InOuts::IMAGE:
        return Qt::darkGray;
      case InOuts::BINARY:
        return Qt::white;
      case InOuts::OBJECT:
        return Qt::green;
    }
    return Qt::lightGray;
  };

  QWidget::paintEvent(event);

  QPainter painter(this);
  const int LINE_WIDTH = 5;

  int heightToPaint = std::ceil(static_cast<float>(height()) / 2.0);
  auto colorIn      = getColor(mInOut.in);
  if(colorIn != Qt::lightGray) {
    painter.fillRect((width() - LINE_WIDTH), 0, LINE_WIDTH, heightToPaint, colorIn);
  }
  auto colorOut = getColor(mInOut.out);
  if(colorOut != Qt::lightGray) {
    painter.fillRect((width() - LINE_WIDTH), heightToPaint, LINE_WIDTH, heightToPaint, colorOut);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Command::registerAddCommandButton(joda::settings::Pipeline &settings, PanelPipelineSettings *pipelineSettingsUi, WindowMain *mainWindow)
{
  // Add command button
  {
    auto *cmdButton = new AddCommandButtonBase(settings, pipelineSettingsUi, &mPipelineStep, getInOut().out, mainWindow);
    cmdButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mDisplayViewLayout.addWidget(cmdButton, 2, 0, 1, 2);
  }
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
  mDisabled = mLayout.addActionButton("Disable", generateIcon("invisible"));
  mDisabled->setCheckable(true);
  mDisabled->setChecked(mPipelineStep.disabled);
  connect(mDisabled, &QAction::triggered, [this, pipelineSettingsUi](bool) {
    if(mDisabled->isChecked()) {
      QFont font;
      font.setItalic(true);
      mDisplayableText->setFont(font);
      mDisplayableText->setStyleSheet("QLabel { color : #808080; }");
    } else {
      QFont font;
      font.setItalic(false);
      mDisplayableText->setFont(font);
      mDisplayableText->setStyleSheet("QLabel { color: black; }");
    }
    mDisplayableText->repaint();
    mPipelineStep.disabled = mDisabled->isChecked();
  });

  auto *okayBottom = mLayout.addActionBottomButton("Okay", generateIcon("accept"));
  connect(okayBottom, &QAction::triggered, [this]() { mEditDialog->close(); });

  connect(mLayout.getDeleteButton(), &QAction::triggered, [this, pipelineSettingsUi]() {
    QMessageBox messageBox(mParent);
    messageBox.setIconPixmap(generateIcon("warning-yellow").pixmap(48, 48));
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
                                          const std::vector<std::tuple<SettingBase *, bool, int32_t>> &settings, helper::VerticalPane *col)
{
  auto containsPtr = [&](SettingBase *toCheck) {
    for(const auto &[ptr, _, group] : mSettings) {
      if(ptr == toCheck) {
        return true;
      }
    }
    return false;
  };

  for(auto &[data, bo, group] : settings) {
    if(!containsPtr(data)) {
      mSettings.emplace_back(data, bo, group);

      {
        auto *casted = dynamic_cast<SettingComboBox<enums::ClusterIdIn> *>(data);
        if(casted) {
          mClusters.emplace_back(casted);
        }
      }

      {
        auto *casted = dynamic_cast<SettingComboBox<enums::ClassIdIn> *>(data);
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
        auto *casted = dynamic_cast<SettingComboBoxMulti<enums::ClassIdIn> *>(data);
        if(casted) {
          mClassesMulti.emplace_back(casted);
        }
      }
    }
  }
  auto convert = [&]() {
    std::vector<SettingBase *> vec;
    std::transform(settings.begin(), settings.end(), std::back_inserter(vec), [](auto &kv) { return std::get<0>(kv); });
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
  for(const auto &[setting, show, group] : settings) {
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
  QString txt = "<html>";
  if(!mSettings.empty()) {
    int cnt      = 0;
    int oldGroup = std::get<2>(*mSettings.begin());
    for(const auto &[setting, show, group] : mSettings) {
      if(show) {
        if(setting == nullptr) {
          continue;
        }
        if(!setting->getDisplayLabelText().isEmpty()) {
          if(oldGroup != group) {
            if(txt.endsWith(", ")) {
              txt.chop(2);
            } else if(txt.endsWith("<br>")) {
              txt.chop(4);
            }
            oldGroup = group;
            cnt      = 0;
            txt += "<hr>";
          }
          txt = txt + setting->getDisplayLabelText();
          if(cnt > 0 && cnt % 3 == 0) {
            txt += "<br>";
          } else {
            txt += ", ";
          }
          cnt++;
        }
      }
    }
  }
  if(txt.endsWith(", ")) {
    txt.chop(2);
  } else if(txt.endsWith("<br>")) {
    txt.chop(4);
  }
  txt += "</html>";
  mDisplayableText->setText(txt);
  // mDisplayableText->adjustSize();
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
                                           const std::map<enums::ClassIdIn, QString> &classNames)
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
