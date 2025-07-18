///
/// \file      dialog_plate_settings.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qdialog.h>
#include <qlineedit.h>
#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QSpinBox>
#include <QVBoxLayout>

namespace joda::ui::gui {

class DialogPlateSettings : public QDialog
{
  Q_OBJECT

public:
  DialogPlateSettings(std::vector<std::vector<int32_t>> *setting, QWidget *parent = nullptr);

private slots:
  void updateMatrix();

private:
  void accept() override;
  void fromSettings();

  QSpinBox *rowSpin;
  QSpinBox *colSpin;
  QGridLayout *matrixLayout;
  std::vector<std::vector<int32_t>> *mSetting;

  std::map<std::pair<int32_t, int32_t>, QLineEdit *> mLineEdits;    // <row, cols>
};

}    // namespace joda::ui::gui
