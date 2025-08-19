
#pragma once

#include <QDoubleSpinBox>
#include <cmath>

class JumpSpinBox : public QDoubleSpinBox
{
  Q_OBJECT
public:
  JumpSpinBox(QWidget *parent = nullptr);
  ~JumpSpinBox() override = default;

protected:
  // handle stepping (buttons, wheel, up/down keys)
  void stepBy(int steps) override;

  // convert from text to value (manual typing)
  [[nodiscard]] double valueFromText(const QString &text) const override;

  // convert from value to text (for display)
  [[nodiscard]] QString textFromValue(double value) const override;
};
