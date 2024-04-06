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
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::qt {

struct Temp
{
  std::set<std::set<int32_t>> order;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Temp, order);
};

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

  mImageHeatmapOnOff = new QComboBox(groupBox);
  mImageHeatmapOnOff->addItem("No heatmap for images", false);
  mImageHeatmapOnOff->addItem("Generate heatmap for images", true);
  groupBoxLayout->addWidget(mImageHeatmapOnOff);

  auto *sliceLabel = new QLabel("Area sizes to observe for image heatmap generation in [px]", groupBox);
  groupBoxLayout->addWidget(sliceLabel);
  mHeatmapSlice = new QLineEdit("50,100");
  groupBoxLayout->addWidget(mHeatmapSlice);

  mGroupByComboBox = new QComboBox(groupBox);
  mGroupByComboBox->addItem("Ungrouped", "OFF");
  mGroupByComboBox->addItem("Group based on foldername", "FOLDER");
  mGroupByComboBox->addItem("Group based on filename", "FILENAME");
  groupBoxLayout->addWidget(mGroupByComboBox);

  mGroupedHeatmapOnOff = new QComboBox(groupBox);
  mGroupedHeatmapOnOff->addItem("No heatmap generation for group", false);
  mGroupedHeatmapOnOff->addItem("Generate heatmap for group", true);
  groupBoxLayout->addWidget(mGroupedHeatmapOnOff);

  mWellHeatmapOnOff = new QComboBox(groupBox);
  mWellHeatmapOnOff->addItem("No heatmap generation for well", false);
  mWellHeatmapOnOff->addItem("Generate heatmap for well", true);
  groupBoxLayout->addWidget(mWellHeatmapOnOff);

  auto *matrixDesc =
      new QLabel("Matrix of the order of the images in a well. Rows are separated by commas: [[],[],[],[]].", groupBox);
  groupBoxLayout->addWidget(matrixDesc);
  mWellOrderMatrix = new QLineEdit("[[1,2,3,4],[5,6,7,8],[9,10,11,12],[13,14,15,16]]");
  groupBoxLayout->addWidget(mWellOrderMatrix);

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
  mRegexToFindTheWellPosition = new QComboBox(groupBox);
  mRegexToFindTheWellPosition->addItem("_((.)([0-9]+))_([0-9]+)", "_((.)([0-9]+))_([0-9]+)");
  mRegexToFindTheWellPosition->addItem("((.)([0-9]+))_([0-9]+)", "((.)([0-9]+))_([0-9]+)");
  mRegexToFindTheWellPosition->setEditable(true);
  groupBoxLayout->addWidget(mRegexToFindTheWellPosition);
  connect(mRegexToFindTheWellPosition, &QComboBox::editTextChanged, this, &DialogSettings::applyRegex);
  connect(mRegexToFindTheWellPosition, &QComboBox::currentIndexChanged, this, &DialogSettings::applyRegex);
  connect(mRegexToFindTheWellPosition, &QComboBox::currentTextChanged, this, &DialogSettings::applyRegex);

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
  {
    auto idx = mGroupByComboBox->findData(QString(settings.getHeatmapSettings().getGroupByString().data()));
    if(idx >= 0) {
      mGroupByComboBox->setCurrentIndex(idx);
    } else {
      mGroupByComboBox->setCurrentIndex(0);
    }
  }

  {
    auto idx = mGroupedHeatmapOnOff->findData(settings.getHeatmapSettings().getCreateHeatmapForGroup());
    if(idx >= 0) {
      mGroupedHeatmapOnOff->setCurrentIndex(idx);
    } else {
      mGroupedHeatmapOnOff->setCurrentIndex(0);
    }
  }

  {
    auto idx = mImageHeatmapOnOff->findData(settings.getHeatmapSettings().getCreateHeatmapForImage());
    if(idx >= 0) {
      mImageHeatmapOnOff->setCurrentIndex(idx);
    } else {
      mImageHeatmapOnOff->setCurrentIndex(0);
    }
  }
  {
    QString slice;
    for(const auto size : settings.getHeatmapSettings().getImageHeatmapAreaWidth()) {
      slice += QString::number(size) + ",";
    }
    slice = slice.left(slice.lastIndexOf(',') + 1);
    mHeatmapSlice->setText(slice);
  }

  {
    try {
      Temp tm{.order = settings.getHeatmapSettings().getWellImageOrder()};
      nlohmann::json j = tm;
      j                = j["order"];
      mWellOrderMatrix->setText(j.dump().data());
    } catch(...) {
      mWellOrderMatrix->setText("[[1,2,3,4],[5,6,7,8]]");
    }
  }

  mRegexToFindTheWellPosition->setCurrentText(settings.getHeatmapSettings().getFileRegex().data());
  applyRegex();
}

nlohmann::json DialogSettings::toJson()
{
  nlohmann::json wellImageOrderJson = nlohmann::json::parse(mWellOrderMatrix->text().toStdString());
  std::set<std::set<int>> wellImageOrder;

  nlohmann::json data;
  data["heatmap"]["group_by"]                   = mGroupByComboBox->currentData().toString().toStdString();
  data["heatmap"]["image_filename_regex"]       = mRegexToFindTheWellPosition->currentText().toStdString();
  data["heatmap"]["generate_heatmap_for_plate"] = mGroupedHeatmapOnOff->currentData().toBool();
  data["heatmap"]["generate_heatmap_for_well"]  = mWellHeatmapOnOff->currentData().toBool();
  data["heatmap"]["generate_heatmap_for_image"] = mImageHeatmapOnOff->currentData().toBool();
  data["heatmap"]["well_image_order"]           = wellImageOrderJson;

  QStringList pieces = mHeatmapSlice->text().split(",");
  std::set<int> sizes;
  for(const auto &part : pieces) {
    bool okay = false;
    int idx   = part.toInt(&okay);
    if(okay) {
      sizes.emplace(idx);
    }
  }

  data["heatmap"]["image_heatmap_area_width"] = sizes;
  return data;
}

void DialogSettings::applyRegex()
{
  try {
    auto regexResult = joda::pipeline::Reporting::applyRegex(mRegexToFindTheWellPosition->currentText().toStdString(),
                                                             mTestFileName->text().toStdString());

    std::string matching = "Match: " + regexResult.group;
    std::string row      = "| Row: " + std::to_string(regexResult.row);
    std::string column   = "| Col: " + std::to_string(regexResult.col);
    std::string img      = "| Img: " + std::to_string(regexResult.img);
    std::string toText   = matching + row + column + img;
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
