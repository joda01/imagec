
#pragma once

#include <QDoubleSpinBox>
#include <cmath>

class JumpSpinBox : public QDoubleSpinBox
{
  Q_OBJECT
public:
  explicit JumpSpinBox(QWidget *parent = nullptr) : QDoubleSpinBox(parent)
  {
    setRange(-1.0, 100.0);    // -1 is allowed
    setDecimals(2);
    setSingleStep(0.1);    // normal step size
  }

  ~JumpSpinBox() override = default;

protected:
  // handle stepping (buttons, wheel, up/down keys)
  void stepBy(int steps) override
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

  // convert from text to value (manual typing)
  [[nodiscard]] double valueFromText(const QString &text) const override
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

  // convert from value to text (for display)
  [[nodiscard]] QString textFromValue(double value) const override
  {
    if(value == -1.0) {
      return "-1";    // or "Disabled", if you want a label
    }
    if(value == 0.0) {
      return "0";
    }
    return QDoubleSpinBox::textFromValue(value);
  }
};
