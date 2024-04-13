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

#include "dialog_experiment_settings.hpp"
#include <qboxlayout.h>
#include <qdialog.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <QMessageBox>
#include <exception>
#include <string>
#include <vector>
#include "backend/pipelines/reporting/reporting_helper.hpp"
#include "backend/settings/experiment_settings.hpp"
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::qt {

struct Temp
{
  std::vector<std::vector<int32_t>> order;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Temp, order);
};

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogExperimentSettings::DialogExperimentSettings(QWidget *windowMain, joda::settings::ExperimentSettings &settings) :
    QDialog(windowMain), mSettings(settings)
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
  mGroupByComboBox->addItem("Ungrouped", static_cast<int>(joda::settings::ExperimentSettings::GroupBy::OFF));
  mGroupByComboBox->addItem("Group based on foldername",
                            static_cast<int>(joda::settings::ExperimentSettings::GroupBy::DIRECTORY));
  mGroupByComboBox->addItem("Group based on filename",
                            static_cast<int>(joda::settings::ExperimentSettings::GroupBy::FILENAME));
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

  auto *groupByLabel = new QLabel("Regex to extract coordinates of Well in plate from image filename", groupBox);
  groupBoxLayout->addWidget(groupByLabel);
  mRegexToFindTheWellPosition = new QComboBox(groupBox);
  mRegexToFindTheWellPosition->addItem("_((.)([0-9]+))_([0-9]+)", "_((.)([0-9]+))_([0-9]+)");
  mRegexToFindTheWellPosition->addItem("((.)([0-9]+))_([0-9]+)", "((.)([0-9]+))_([0-9]+)");
  mRegexToFindTheWellPosition->setEditable(true);
  groupBoxLayout->addWidget(mRegexToFindTheWellPosition);
  connect(mRegexToFindTheWellPosition, &QComboBox::editTextChanged, this, &DialogExperimentSettings::applyRegex);
  connect(mRegexToFindTheWellPosition, &QComboBox::currentIndexChanged, this, &DialogExperimentSettings::applyRegex);
  connect(mRegexToFindTheWellPosition, &QComboBox::currentTextChanged, this, &DialogExperimentSettings::applyRegex);

  mTestFileName = new QLineEdit("your_test_image_file_Name_A99_01.tif", groupBox);
  groupBoxLayout->addWidget(mTestFileName);
  connect(mTestFileName, &QLineEdit::editingFinished, this, &DialogExperimentSettings::applyRegex);

  mTestFileResult = new QLabel(groupBox);
  groupBoxLayout->addWidget(mTestFileResult);

  groupBoxLayout->addStretch();

  mainLayout->addWidget(groupBox);

  fromSettings();
  applyRegex();
}

int DialogExperimentSettings::exec()
{
  int ret = QDialog::exec();
  toSettings();
  return ret;
}

void DialogExperimentSettings::fromSettings()
{
  {
    auto idx = mGroupByComboBox->findData(static_cast<int>(mSettings.groupBy));
    if(idx >= 0) {
      mGroupByComboBox->setCurrentIndex(idx);
    } else {
      mGroupByComboBox->setCurrentIndex(0);
    }
  }

  {
    auto idx = mGroupedHeatmapOnOff->findData(static_cast<int>(mSettings.generateHeatmapForPlate));
    if(idx >= 0) {
      mGroupedHeatmapOnOff->setCurrentIndex(idx);
    } else {
      mGroupedHeatmapOnOff->setCurrentIndex(0);
    }
  }

  {
    auto idx = mImageHeatmapOnOff->findData(static_cast<int>(mSettings.generateHeatmapForImage));
    if(idx >= 0) {
      mImageHeatmapOnOff->setCurrentIndex(idx);
    } else {
      mImageHeatmapOnOff->setCurrentIndex(0);
    }
  }
  {
    auto idx = mWellHeatmapOnOff->findData(static_cast<int>(mSettings.generateHeatmapForWell));
    if(idx >= 0) {
      mWellHeatmapOnOff->setCurrentIndex(idx);
    } else {
      mWellHeatmapOnOff->setCurrentIndex(0);
    }
  }
  {
    QString slice;
    for(const auto size : mSettings.imageHeatmapAreaSizes) {
      slice += QString::number(size) + ",";
    }
    slice = slice.left(slice.lastIndexOf(',') + 1);
    mHeatmapSlice->setText(slice);
  }

  {
    auto idx = mGroupByComboBox->findData(static_cast<int>(mSettings.groupBy));
    if(idx >= 0) {
      mGroupByComboBox->setCurrentIndex(idx);
    } else {
      mGroupByComboBox->setCurrentIndex(0);
    }
  }

  {
    try {
      Temp tm{.order = mSettings.wellImageOrder};
      nlohmann::json j = tm;
      j                = j["order"];
      mWellOrderMatrix->setText(j.dump().data());
    } catch(...) {
      mWellOrderMatrix->setText("[[1,2,3,4],[5,6,7,8]]");
    }
  }

  mRegexToFindTheWellPosition->setCurrentText(mSettings.filenameRegex.data());
  applyRegex();
}

void DialogExperimentSettings::toSettings()
{
  QStringList pieces = mHeatmapSlice->text().split(",");
  std::set<int> sizes;
  for(const auto &part : pieces) {
    bool okay = false;
    int idx   = part.toInt(&okay);
    if(okay) {
      sizes.emplace(idx);
    }
  }

  mSettings.imageHeatmapAreaSizes   = sizes;
  mSettings.generateHeatmapForImage = mImageHeatmapOnOff->currentData().toBool();
  mSettings.generateHeatmapForPlate = mGroupedHeatmapOnOff->currentData().toBool();
  mSettings.generateHeatmapForWell  = mGroupedHeatmapOnOff->currentData().toBool();

  mSettings.groupBy = static_cast<joda::settings::ExperimentSettings::GroupBy>(mGroupByComboBox->currentData().toInt());
  mSettings.filenameRegex = mRegexToFindTheWellPosition->currentText().toStdString();

  try {
    nlohmann::json wellImageOrderJson = nlohmann::json::parse(mWellOrderMatrix->text().toStdString());
    nlohmann::json obj;
    obj["order"]             = wellImageOrderJson;
    Temp tm                  = nlohmann::json::parse(obj.dump());
    mSettings.wellImageOrder = tm.order;
  } catch(...) {
    mSettings.wellImageOrder.clear();
    QMessageBox::warning(this, "Warning",
                         "The well matrix format is not well defined. Please correct it in the settings dialog!");
  }
}

void DialogExperimentSettings::applyRegex()
{
  try {
    auto regexResult = joda::pipeline::reporting::Helper::applyRegex(
        mRegexToFindTheWellPosition->currentText().toStdString(), mTestFileName->text().toStdString());

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

void DialogExperimentSettings::onOkayClicked()
{
}
void DialogExperimentSettings::onCancelClicked()
{
}

}    // namespace joda::ui::qt
