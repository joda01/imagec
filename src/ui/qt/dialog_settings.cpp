///
/// \file      dialog_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-29
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "dialog_settings.hpp"
#include <qboxlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <exception>
#include <string>
#include "backend/pipelines/reporting/reporting.hpp"
#include "backend/settings/analze_settings_parser.hpp"

namespace joda::ui::qt {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogSettings::DialogSettings(QWidget *windowMain) : QDialog(windowMain)
{
  setWindowTitle("Settings");
  setBaseSize(500, 200);

  auto *mainLayout     = new QVBoxLayout(this);
  auto *groupBox       = new QGroupBox("Reporting settings", this);
  auto *groupBoxLayout = new QVBoxLayout(groupBox);

  mGroupByComboBox = new QComboBox(groupBox);
  mGroupByComboBox->addItem("Ungrouped", "OFF");
  mGroupByComboBox->addItem("Group by folder", "FOLDER");
  mGroupByComboBox->addItem("Group by Well", "FILENAME");
  groupBoxLayout->addWidget(mGroupByComboBox);

  // mPlateComboBox = new QComboBox(groupBox);
  // mPlateComboBox->addItem("No plate (1x1)");
  // mPlateComboBox->addItem("6 Well plate (3x2)");
  // mPlateComboBox->addItem("12 Well plate (4x3)");
  // mPlateComboBox->addItem("24 Well plate (6x4)");
  // mPlateComboBox->addItem("96 Well plate (12x8)");
  // mPlateComboBox->addItem("384 Well plate (24x16)");
  //  groupBoxLayout->addWidget(mPlateComboBox);

  auto *groupByLabel = new QLabel("Regex to extract coordinates of Well in plate from image filename", groupBox);
  groupBoxLayout->addWidget(groupByLabel);
  mRegexToFindTheWellPosition = new QLineEdit(groupBox);
  mRegexToFindTheWellPosition->setText("_((.)([0-9]+))_");
  groupBoxLayout->addWidget(mRegexToFindTheWellPosition);
  connect(mRegexToFindTheWellPosition, &QLineEdit::editingFinished, this, &DialogSettings::applyRegex);

  mTestFileName = new QLineEdit("your_test_image_file_Name_A99_01.tif", groupBox);
  groupBoxLayout->addWidget(mTestFileName);
  connect(mTestFileName, &QLineEdit::editingFinished, this, &DialogSettings::applyRegex);

  mTestFileResult = new QLabel(groupBox);
  groupBoxLayout->addWidget(mTestFileResult);

  groupBoxLayout->addStretch();

  mainLayout->addWidget(groupBox);
  applyRegex();
}

void DialogSettings::fromJson(const settings::json::AnalyzeSettingsReporting &settings)
{
  auto idx = mGroupByComboBox->findData(QString(settings.getGroupByString().data()));
  if(idx >= 0) {
    mGroupByComboBox->setCurrentIndex(idx);
  } else {
    mGroupByComboBox->setCurrentIndex(0);
  }

  mRegexToFindTheWellPosition->setText(settings.getFileRegex().data());
  applyRegex();
}

nlohmann::json DialogSettings::toJson()
{
  nlohmann::json data;
  data["group_by"]                   = mGroupByComboBox->currentData().toString().toStdString();
  data["image_filename_regex"]       = mRegexToFindTheWellPosition->text().toStdString();
  data["generate_heatmap_for_group"] = true;
  data["generate_heatmap_for_image"] = false;
  data["image_heatmap_area_width"]   = 0;
  return data;
}

void DialogSettings::applyRegex()
{
  try {
    auto regexResult = joda::pipeline::Reporting::applyRegex(mRegexToFindTheWellPosition->text().toStdString(),
                                                             mTestFileName->text().toStdString());

    std::string matching = "Match: " + regexResult.group;
    std::string row      = "| Row: " + std::to_string(regexResult.row);
    std::string column   = "| Col: " + std::to_string(regexResult.col);
    std::string toText   = matching + row + column;
    mTestFileResult->setText(QString(toText.data()));
  } catch(const std::exception &ex) {
    mTestFileResult->setText(ex.what());
  }
}

void DialogSettings::onOkayClicked()
{
}
void DialogSettings::onCancelClicked()
{
}

}    // namespace joda::ui::qt
