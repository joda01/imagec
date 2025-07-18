///
/// \file      iconless_dialog_button_box.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <QDialogButtonBox>
#include <QIcon>
#include <QPushButton>

class IconlessDialogButtonBox : public QDialogButtonBox
{
  Q_OBJECT

public:
  IconlessDialogButtonBox(StandardButtons buttons = NoButton, Qt::Orientation orientation = Qt::Horizontal, QWidget *parent = nullptr);

private:
  void removeIcons();
};
