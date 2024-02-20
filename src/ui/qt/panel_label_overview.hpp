#pragma once

#include <qlabel.h>
#include <qwidget.h>
#include <QtWidgets>

namespace joda::ui::qt {

class PanelLabelOverview : public QWidget
{
public:
  PanelLabelOverview(const QString &icon, const QString &text)
  {
    // Create a QLabel
    QLabel *label    = new QLabel();
    QLabel *labelTxt = new QLabel();

    // Set text for the label
    labelTxt->setText(text);

    // Create a QPixmap for the icon (you may need to adjust the path)
    QIcon bmp(":/icons/" + icon);

    // Set the icon for the label
    label->setPixmap(bmp.pixmap(16, 16));    // You can adjust the size of the icon as needed

    // Create a QHBoxLayout to arrange the text and icon horizontally
    QHBoxLayout *layout = new QHBoxLayout;
    setLayout(layout);
    layout->addWidget(label);
    layout->addWidget(labelTxt);
    layout->addStretch();
  }
};
}    // namespace joda::ui::qt
