///
/// \file      dialog_plate_settings.cpp
/// \author    Joachim Danmayr
/// \date      2025-07-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "dialog_plate_settings.hpp"
#include <qdialogbuttonbox.h>
#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPainter>
#include <QResizeEvent>
#include <QSpinBox>
#include <QStyleOption>
#include <QVBoxLayout>
#include "backend/settings/project_settings/project_plate_setup.hpp"
#include "ui/gui/helper/iconless_dialog_button_box.hpp"

namespace joda::ui::gui {

// Custom QLineEdit that keeps square shape
class SquareLineEdit : public QLineEdit
{
public:
  SquareLineEdit(QWidget *parent = nullptr) : QLineEdit(parent)
  {
    setAlignment(Qt::AlignCenter);
  }

  QSize sizeHint() const override
  {
    int side = 40;    // Default size
    return QSize(side, side);
  }

  QSize minimumSizeHint() const override
  {
    int side = 30;
    return QSize(side, side);
  }

  void resizeEvent(QResizeEvent *event) override
  {
    int side = qMin(event->size().width(), event->size().height());
    resize(side, side);
    QLineEdit::resizeEvent(event);
  }
};

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
DialogPlateSettings::DialogPlateSettings(std::vector<std::vector<int32_t>> *setting, QWidget *parent) : QDialog(parent), mSetting(setting)
{
  setWindowTitle("Well image order");
  auto *mainLayout = new QVBoxLayout(this);

  // Top controls
  auto *controlLayout = new QHBoxLayout();
  rowSpin             = new QSpinBox();
  rowSpin->setRange(1, 20);
  rowSpin->setValue(3);

  colSpin = new QSpinBox();
  colSpin->setRange(1, 20);
  colSpin->setValue(3);

  controlLayout->addWidget(new QLabel("Rows:"));
  controlLayout->addWidget(rowSpin);
  controlLayout->addWidget(new QLabel("Columns:"));
  controlLayout->addWidget(colSpin);
  mainLayout->addLayout(controlLayout);

  auto *line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  mainLayout->addWidget(line);

  // Matrix layout
  matrixLayout = new QGridLayout();
  matrixLayout->setSpacing(4);
  mainLayout->addLayout(matrixLayout);

  connect(rowSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &DialogPlateSettings::updateMatrix);
  connect(colSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &DialogPlateSettings::updateMatrix);

  auto *line02 = new QFrame();
  line02->setFrameShape(QFrame::HLine);
  line02->setFrameShadow(QFrame::Sunken);
  mainLayout->addWidget(line02);

  auto *buttonBox = new IconlessDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  mainLayout->addWidget(buttonBox);

  fromSettings();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DialogPlateSettings::updateMatrix()
{
  // Clear previous widgets
  QLayoutItem *item;
  while((item = matrixLayout->takeAt(0)) != nullptr) {
    if(item->widget() != nullptr) {
      item->widget()->deleteLater();
    }
    delete item;
  }
  mLineEdits.clear();

  int rows = rowSpin->value();
  int cols = colSpin->value();

  int32_t idx = 1;
  for(int r = 0; r < rows; ++r) {
    for(int c = 0; c < cols; ++c) {
      mLineEdits[{r, c}] = new SquareLineEdit();
      mLineEdits[{r, c}]->setFixedSize(50, 50);    // Initial square size
      mLineEdits[{r, c}]->setText(QString::number(idx));
      idx++;
      matrixLayout->addWidget(mLineEdits[{r, c}], r, c);
    }
  }

  adjustSize();    // Resize dialog
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogPlateSettings::fromSettings()
{
  int32_t rows = 0;
  int32_t cols = 0;
  auto wellPos = joda::settings::transformMatrix(*mSetting, cols, rows);

  rowSpin->blockSignals(true);
  colSpin->blockSignals(true);
  rowSpin->setValue(rows);
  colSpin->setValue(cols);
  rowSpin->blockSignals(false);
  colSpin->blockSignals(false);
  updateMatrix();

  for(auto const &[imgIdx, coordinates] : wellPos) {
    std::pair<int32_t, int32_t> pos{coordinates.y - 1, coordinates.x - 1};
    if(mLineEdits.contains(pos)) {
      mLineEdits.at(pos)->setText(QString::number(imgIdx));
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
void DialogPlateSettings::accept()
{
  std::vector<std::vector<int32_t>> matrix;    // row, col
  for(int row = 0; row < rowSpin->value(); row++) {
    std::vector<int32_t> cols;
    cols.reserve(colSpin->value());
    for(int col = 0; col < colSpin->value(); col++) {
      cols.emplace_back(0);
    }
    matrix.emplace_back(cols);
  }
  std::set<int32_t> indexes;
  for(const auto &[coordinates, line] : mLineEdits) {
    bool okay = false;
    auto idx  = line->text().toInt(&okay);
    if(!okay) {
      line->setStyleSheet("QLineEdit { border: 2px solid red; }");
      QMessageBox::warning(this, "Not a number...", "Values in the matrix must be a number in range of [0-65535].");
      return;
    }

    if(indexes.contains(idx)) {
      line->setStyleSheet("QLineEdit { border: 2px solid red; }");
      QMessageBox::warning(this, "Wrong number", "The index >" + QString::number(idx) + "< is used twice!");
      return;
    }
    line->setStyleSheet("");    // Clears any custom style

    indexes.emplace(idx);
    matrix[coordinates.first][coordinates.second] = idx;
  }

  *mSetting = matrix;

  QDialog::accept();
}

}    // namespace joda::ui::gui
