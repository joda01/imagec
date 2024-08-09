

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
#include <qwidget.h>
#include "ui/container/container_function.hpp"
#include "ui/container/panel_edit_base.hpp"

namespace joda::ui::qt::helper {

class LayoutGenerator
{
  static constexpr int32_t SPACING     = 16;
  static constexpr int32_t PANEL_WIDTH = 200;

public:
  explicit LayoutGenerator(PanelEdit *parent) : mParent(parent)
  {
    auto *scrollArea = new QScrollArea(parent);
    scrollArea->setObjectName("scrollArea");
    scrollArea->setFrameStyle(0);
    scrollArea->setContentsMargins(0, 0, 0, 0);
    scrollArea->verticalScrollBar()->setObjectName("scrollAreaV");

    // Create a widget to hold the panels
    auto *contentWidget = new QWidget;
    contentWidget->setObjectName("contentOverview");

    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);

    // Create a horizontal layout for the panels
    mMainLayout = new QHBoxLayout(contentWidget);
    mMainLayout->setContentsMargins(SPACING, SPACING, 0, 0);
    mMainLayout->setSpacing(SPACING);    // Adjust this value as needed
    contentWidget->setLayout(mMainLayout);
    parent->setLayout(mMainLayout);
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

private:
  QHBoxLayout *mMainLayout;
  PanelEdit *mParent;
};

}    // namespace joda::ui::qt::helper
