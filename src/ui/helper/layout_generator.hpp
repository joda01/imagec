///
/// \file      layout_generator.hpp
/// \author
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <qboxlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qnamespace.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qscrollarea.h>
#include <qscrollbar.h>
#include <qtableview.h>
#include <qtablewidget.h>
#include <qtoolbar.h>
#include <qwidget.h>
#include "ui/container/container_function.hpp"
#include "ui/container/panel_edit_base.hpp"

namespace joda::ui::qt::helper {

class LayoutGenerator
{
  static constexpr int32_t SPACING = 16;

public:
  explicit LayoutGenerator(PanelEdit *parent);
  void disableDeleteButton()
  {
    mToolbarBottom->setVisible(false);
  }

  class VerticalPane : public QVBoxLayout
  {
  public:
    explicit VerticalPane(PanelEdit *parent) : mParent(parent)
    {
    }

    void addGroup(const QString &title, const std::vector<std::shared_ptr<ContainerFunctionBase>> &elements)
    {
      auto *group = new QGroupBox(title);
      group->setMaximumWidth(220);
      auto *layout = new QVBoxLayout;
      for(const auto &element : elements) {
        layout->addWidget(element->getEditableWidget());
        connect(element.get(), &ContainerFunctionBase::valueChanged, mParent, &PanelEdit::onValueChanged);
      }

      group->setLayout(layout);
      addWidget(group);
    }

  private:
    PanelEdit *mParent;
  };

  VerticalPane *addVerticalPanel()
  {
    auto *vboxLayout = new VerticalPane(mParent);
    vboxLayout->setAlignment(Qt::AlignTop);
    mMainLayout->addLayout(vboxLayout, 1);
    return vboxLayout;
  }

  void addSeparator(QFormLayout *formLayout)
  {
    auto *separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    formLayout->addRow(separator);
  }

  QLabel *createTitle(const QString &title)
  {
    auto *label = new QLabel();
    QFont font;
    font.setPixelSize(16);
    font.setBold(true);
    label->setFont(font);
    label->setText(title);

    return label;
  }

  void addSeparatorToTopToolbar();
  QAction *addItemToTopToolbar(QWidget *);

private:
  /////////////////////////////////////////////////////
  QToolBar *mToolbarTop;
  QToolBar *mToolbarBottom;
  QHBoxLayout *mMainLayout;
  PanelEdit *mParent;

  QAction *mSpaceTopToolbar;
};

}    // namespace joda::ui::qt::helper
