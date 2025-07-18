///
/// \file      command.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
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
#include <vector>
#include "ui/gui/editor/widget_pipeline/add_command_button.hpp"
#include "ui/gui/editor/window_main.hpp"
#include "ui/gui/helper/icon_generator.hpp"

namespace joda::ui::gui {

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

Command::Command(joda::settings::PipelineStep &pipelineStep, const QString &title, const QString &description, const std::vector<std::string> &tags,
                 const QString &icon, QWidget *parent, InOut type) :
    mPipelineStep(pipelineStep),
    mParent(parent), mTitle(title), mDescription(description), mTags(tags), mLayout(&mEditView, true, true, false), mDisplayViewLayout(this),
    mInOut(type)
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
      auto color = Color::BLACK;
      if(type.out == InOuts::OBJECT) {
        color = Color::GREEN;
      }
      if(icon.contains("-duotone")) {
        QString tmp(icon);
        tmp.replace("-duotone", "");
        mIcon = generateSvgIcon<Style::DUETONE, Color::BLACK>(icon);
      } else {
        mIcon = generateSvgIcon<Style::REGULAR, Color::BLACK>(icon);
      }

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

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Command::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton && !mPipelineStep.locked) {
    openEditView();
  }

  // Breakpoints
  if(event->button() == Qt::RightButton) {
    // Only one breakpoint is allowed remove the others
    if(nullptr != mOtherCommands) {
      for(auto &cmd : *mOtherCommands) {
        if(cmd.get() != this) {
          cmd->setBreakpoint(false);
          cmd->setIsBreakpointWithoutSignal(false);
        }
      }
    }

    mBreakpoint->blockSignals(true);
    mBreakpoint->setChecked(!isBreakpoint());
    setBreakpoint(mBreakpoint->isChecked());
    emit valueChanged();
    mBreakpoint->blockSignals(false);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Command::mouseDoubleClickEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton && mPipelineStep.locked) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(mParent,                                                              // Parent widget
                                 "Locked",                                                             // Title of the dialog
                                 "This command is locked! Would you like to proceed with editing?",    // Message text
                                 QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::Yes) {
      openEditView();
    }
  }
}

[[nodiscard]] InOuts Command::getResolvedInput() const
{
  if(mCommandBefore != nullptr) {
    auto commandBeforeBefore = mCommandBefore;
    auto outTmp              = commandBeforeBefore->getInOut().out;
    while(outTmp == InOuts::OUTPUT_EQUAL_TO_INPUT && commandBeforeBefore != nullptr) {
      commandBeforeBefore = commandBeforeBefore->getCommandBefore();
      if(commandBeforeBefore != nullptr) {
        outTmp = commandBeforeBefore->getInOut().out;
      } else {
        break;
      }
    }

    auto iter = mInOut.in.find(outTmp);
    if(iter != mInOut.in.end()) {
      return *iter;
    }
  }
  return *mInOut.in.begin();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Command::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  const int LINE_WIDTH = 6;

  //
  // Paint the breakpoint
  //
  {
    if(isBreakpoint()) {
      painter.setBrush(Qt::red);
      painter.setPen(Qt::red);
      painter.drawEllipse(width() - LINE_WIDTH - 16, height() / 2 - 6, 8, 8);
    }
  }

  //
  // Paint the arrow at the left
  //
  {
    std::function<QColor(InOuts)> getColor = [&](InOuts inouts) -> QColor {
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

    auto getColorSet = [&](std::set<InOuts> inouts) -> QColor {
      // Select the input color depending on the previous command output
      if(mCommandBefore != nullptr) {
        auto outTmp = mCommandBefore->getInOut().out;
        if(outTmp == InOuts::OUTPUT_EQUAL_TO_INPUT) {
          auto commandBeforeBefore = mCommandBefore;
          outTmp                   = commandBeforeBefore->getResolvedInput();
          while(outTmp == InOuts::OUTPUT_EQUAL_TO_INPUT && commandBeforeBefore != nullptr) {
            commandBeforeBefore = commandBeforeBefore->getCommandBefore();
            if(commandBeforeBefore == nullptr) {
              break;
            }
            outTmp = commandBeforeBefore->getResolvedInput();
          }
        }
        if(inouts.contains(outTmp)) {
          return getColor(outTmp);
        }
        // This command is not allowed
        return Qt::red;
      }
      return getColor(*inouts.begin());
    };

    int heightToPaint    = std::ceil(static_cast<float>(height()) / 2.0);
    auto paintTopPolygon = [&](const QColor &color) {
      QPen pen(color, 1);    // Black pen with 3px width
      painter.setPen(pen);
      QBrush brush(color);    // Fill with blue color
      painter.setBrush(brush);
      QPolygon chevron;
      auto left         = static_cast<float>(((width() - 1) - LINE_WIDTH));
      auto middle       = static_cast<float>(((width() - 1) - static_cast<float>(LINE_WIDTH) / 2.0));
      auto right        = static_cast<float>(width() - 1);
      auto heightStart  = 0.0f;
      auto heightMiddle = 8;    // static_cast<float>(heightToPaint / 3.0);
      auto heightEnd    = static_cast<float>(heightToPaint);
      chevron << QPoint(left, heightStart)       // Top-left
              << QPoint(middle, heightMiddle)    // Top-middle
              << QPoint(right, heightStart)      // Top-right
              << QPoint(right, heightEnd)        // Bottom point
              << QPoint(left, heightEnd)         // Bottom-left
              << QPoint(left, heightStart);      // Return to middle

      // Draw the chevron
      painter.drawPolygon(chevron);
    };

    auto paintBottomPolygon = [&](const QColor &color) {
      QPen pen(color, 1);    // Black pen with 3px width
      painter.setPen(pen);
      QBrush brush(color);    // Fill with blue color
      painter.setBrush(brush);

      QPolygon chevron;
      auto left         = static_cast<float>(((width() - 1) - LINE_WIDTH));
      auto middle       = static_cast<float>(((width() - 1) - static_cast<float>(LINE_WIDTH) / 2.0));
      auto right        = static_cast<float>(width() - 1);
      auto heightStart  = heightToPaint;
      auto heightMiddle = static_cast<float>(heightToPaint * 2) - 8;    // (static_cast<float>(heightToPaint / 3.0));
      auto heightEnd    = static_cast<float>(heightToPaint * 2);
      chevron << QPoint(left, heightStart)      // Top-left
              << QPoint(right, heightStart)     // Top-middle
              << QPoint(right, heightMiddle)    // Top-right
              << QPoint(middle, heightEnd)      // Bottom point
              << QPoint(left, heightMiddle)     // Bottom-left
              << QPoint(left, heightStart);     // Return to middle

      // Draw the chevron
      painter.drawPolygon(chevron);
    };

    QWidget::paintEvent(event);

    auto colorIn = getColorSet(mInOut.in);
    if(colorIn != Qt::lightGray) {
      // painter.fillRect((width() - LINE_WIDTH), 0, LINE_WIDTH, heightToPaint, colorIn);
      paintTopPolygon(colorIn);
    }
    QColor colorOut;
    if(mInOut.out == InOuts::OUTPUT_EQUAL_TO_INPUT) {
      colorOut = getColorSet(mInOut.in);
    } else {
      colorOut = getColor(mInOut.out);
    }
    if(colorOut != Qt::lightGray) {
      // painter.fillRect((width() - LINE_WIDTH), heightToPaint, LINE_WIDTH, heightToPaint, colorOut);
      paintBottomPolygon(colorOut);
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
InOuts Command::getOut() const
{
  auto outTmp = getInOut().out;
  if(outTmp == InOuts::OUTPUT_EQUAL_TO_INPUT) {
    if(mCommandBefore != nullptr) {
      auto outTmp = mCommandBefore->getInOut().out;
      if(outTmp == InOuts::OUTPUT_EQUAL_TO_INPUT) {
        outTmp = mCommandBefore->getResolvedInput();
      }
      return outTmp;
    }
    outTmp = *mInOut.in.begin();
  }
  return outTmp;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Command::registerAddCommandButton(std::shared_ptr<Command> commandBefore, std::shared_ptr<DialogCommandSelection> &cmdDialog,
                                       joda::settings::Pipeline &settings, PanelPipelineSettings *pipelineSettingsUi, WindowMain *mainWindow)
{
  mCommandBefore = commandBefore;
  mCmdButton     = new AddCommandButtonBase(cmdDialog, settings, pipelineSettingsUi, &mPipelineStep, getOut(), mainWindow);
  mCmdButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mDisplayViewLayout.addWidget(mCmdButton, 2, 0, 1, 2);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Command::setCommandBefore(std::shared_ptr<Command> commandBefore)
{
  mCommandBefore = commandBefore;
  mCmdButton->setInOutBefore(getOut());
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
  //
  // Disabled button
  //
  mDisabled = mLayout.addActionButton("Disable", generateSvgIcon<Style::REGULAR, Color::BLACK>("eye-slash"));
  mDisabled->setCheckable(true);
  mDisabled->setChecked(mPipelineStep.disabled);
  connect(mDisabled, &QAction::triggered, [this, pipelineSettingsUi](bool) {
    setDisabled(mDisabled->isChecked());
    emit valueChanged();
  });

  //
  // Locked button
  //
  mLocked = mLayout.addActionButton("Locked", generateSvgIcon<Style::REGULAR, Color::BLACK>("lock-simple"));
  mLocked->setCheckable(true);
  mLocked->setChecked(mPipelineStep.locked);
  connect(mLocked, &QAction::triggered, [this, pipelineSettingsUi](bool) {
    setLocked(mLocked->isChecked());
    emit valueChanged();
  });

  //
  // Breakpoint button
  //
  mBreakpoint = mLayout.addActionButton("Breakpoint", generateSvgIcon<Style::DUETONE, Color::RED>("stop"));
  mBreakpoint->setCheckable(true);
  mBreakpoint->setChecked(mPipelineStep.breakPoint);
  mBreakpoint->setVisible(false);
  connect(mBreakpoint, &QAction::triggered, [this, pipelineSettingsUi](bool) {
    setBreakpoint(mBreakpoint->isChecked());
    emit valueChanged();
  });
  mOtherCommands = pipelineSettingsUi->getListOfCommands();

  //
  // Okay button
  //
  auto *okayBottom = mLayout.addActionBottomButton("Okay", generateSvgIcon<Style::DUETONE, Color::BLACK>("arrow-elbow-down-left"));
  connect(okayBottom, &QAction::triggered, [this]() { mEditDialog->close(); });

  connect(mLayout.getDeleteButton(), &QAction::triggered, [this, pipelineSettingsUi]() {
    QMessageBox messageBox(mParent);
    messageBox.setIconPixmap(generateSvgIcon<Style::REGULAR, Color::YELLOW>("warning").pixmap(48, 48));
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

  setDisplayTextFont();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Command::setDisabled(bool disabled)
{
  mPipelineStep.disabled = disabled;
  setDisplayTextFont();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Command::setLocked(bool locked)
{
  mPipelineStep.locked = locked;
  setDisplayTextFont();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Command::setBreakpoint(bool breakPoint)
{
  mPipelineStep.breakPoint = breakPoint;
  setDisplayTextFont();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Command::setDisplayTextFont()
{
  if(mPipelineStep.disabled) {
    QFont font;
    font.setStrikeOut(true);
    mDisplayableText->setFont(font);
    mDisplayableText->setStyleSheet("QLabel { color : #808080; }");
  } else if(mPipelineStep.locked) {
    QFont font;
    mDisplayableText->setFont(font);
    mDisplayableText->setStyleSheet("QLabel { color : #808080; }");
  } else {
    QFont font;
    font.setItalic(false);
    mDisplayableText->setFont(font);
    mDisplayableText->setStyleSheet("QLabel { color: black; }");
  }
  mDisplayableText->repaint();
  update();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
helper::TabWidget *Command::addTab(const QString &title, std::function<void()> beforeTabClose, bool showCloseButton)
{
  auto *tab = mLayout.addTab(title, beforeTabClose, showCloseButton);
  /// \todo rethink this
  // if(mEditDialog->isVisible()) {
  //   std::thread([this] {
  //     std::this_thread::sleep_for(100ms);
  //     mEditDialog->adjustSize();
  //   }).detach();
  // }
  return tab;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Command::removeTab(int32_t idx)
{
  if(idx > 0 && idx < mLayout.getNrOfTabs()) {
    mLayout.onRemoveTab(idx);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Command::removeAllTabsExceptFirst()
{
  int n = mLayout.getNrOfTabs() - 1;
  for(; n >= 1; n--) {
    mLayout.onRemoveTab(n);
  }
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
    connect(setting, &SettingBase::displayTextChanged, this, &Command::displayTextChanged);
    connect(setting, &SettingBase::valueChanged, this, &Command::valueChanged);
  }
  updateDisplayText();
  connect(this, &Command::displayTextChanged, this, &Command::updateDisplayText);

  return col;
}

helper::VerticalPane *Command::addWidgets(helper::TabWidget *tab, const QString &boxTitle, const std::vector<QWidget *> &settings,
                                          helper::VerticalPane *col)
{
  if(nullptr == col) {
    col = tab->addVerticalPanel();
  }
  if(boxTitle.isEmpty()) {
    col->addWidgetGroup(settings, 220, 300);
  } else {
    col->addWidgetGroup(boxTitle, settings, 220, 300);
  }

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
}

}    // namespace joda::ui::gui
