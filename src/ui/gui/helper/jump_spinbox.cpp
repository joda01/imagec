///
/// \file      jump_spinbox.cpp
/// \author    Joachim Danmayr
/// \date      2025-08-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "jump_spinbox.hpp"

JumpSpinBox::JumpSpinBox(QWidget *parent) : QDoubleSpinBox(parent)
{
  setRange(-1.0, 100.0);    // -1 is allowed
  setDecimals(2);
  setSingleStep(0.1);    // normal step size
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void JumpSpinBox::stepBy(int steps)
{
  double v = value();

  if(v == -1.0 && steps > 0) {
    setValue(0.0);
    return;
  }
  if(v == 0.0 && steps < 0) {
    setValue(-1.0);
    return;
  }

  QDoubleSpinBox::stepBy(steps);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] double JumpSpinBox::valueFromText(const QString &text) const
{
  bool ok    = false;
  double val = text.toDouble(&ok);
  if(!ok) {
    return QDoubleSpinBox::valueFromText(text);
  }

  // normalize exact -1 and 0 to avoid floating quirks
  if(qFuzzyCompare(val + 1.0, 0.0)) {
    return -1.0;
  }
  if(qFuzzyCompare(val, 0.0)) {
    return 0.0;
  }

  return val;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] QString JumpSpinBox::textFromValue(double value) const
{
  if(value == -1.0) {
    return "-1";    // or "Disabled", if you want a label
  }
  if(value == 0.0) {
    return "0";
  }
  return QDoubleSpinBox::textFromValue(value);
}
