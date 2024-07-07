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
#include <qdialogbuttonbox.h>
#include <qformlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qscrollarea.h>
#include <qscrollbar.h>
#include <qspinbox.h>
#include <qwidget.h>
#include <QFileDialog>
#include <QFormLayout>
#include <QMessageBox>
#include <exception>
#include <string>
#include <vector>
#include "backend/helper/username.hpp"
#include "backend/results/results.hpp"
#include "backend/settings/experiment_settings.hpp"
#include "ui/dialog_shadow/dialog_shadow.h"
#include "ui/window_main.hpp"
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
DialogExperimentSettings::DialogExperimentSettings(WindowMain *windowMain,
                                                   joda::settings::ExperimentSettings &settings) :
    DialogShadow(windowMain, true, "Apply"),
    mParentWindow(windowMain), mSettings(settings)
{
  setWindowTitle("Settings");
  setBaseSize(650, 200);
  setMinimumWidth(650);
  setMaximumHeight(300);
  createLayout();
  fromSettings();
  applyRegex();
}

int DialogExperimentSettings::exec()
{
  int ret = DialogShadow::exec();
  toSettings();
  return ret;
}

void DialogExperimentSettings::createLayout()
{
  createScientistGroupBox();
  createExperimentGroupBox();

  mNotes = new QTextEdit;
  mNotes->setPlaceholderText("Notes on the experiment...");

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(28, 28, 28, 28);
  mainLayout->addWidget(mScientistsGroup);
  mainLayout->addWidget(mExperimentGroup);
  mainLayout->addWidget(mNotes);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogExperimentSettings::createScientistGroupBox()
{
  mScientistsGroup    = new QGroupBox(tr("Who is?"));
  QFormLayout *layout = new QFormLayout;

  //
  // Scientists
  //
  for(int i = 0; i < NR_OF_SCIENTISTS; ++i) {
    mScientists[i] = new QLineEdit;
    layout->addRow(new QLabel(tr("Scientist name:")), mScientists[i]);
  }

  mScientists[0]->setPlaceholderText(joda::helper::getLoggedInUserName());

  //
  // Location
  //
  mAddressOrganisation = new QLineEdit;
  mAddressOrganisation->setPlaceholderText("Univesity of Salzburg");
  layout->addRow(new QLabel(tr("Organization:")), mAddressOrganisation);

  mScientistsGroup->setLayout(layout);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogExperimentSettings::createExperimentGroupBox()
{
  mExperimentGroup    = new QGroupBox(tr("Experiment"));
  QFormLayout *layout = new QFormLayout;

  //
  // Working directory
  //
  mWorkingDir = new QLineEdit();
  mWorkingDir->setReadOnly(true);
  mWorkingDir->setPlaceholderText("Directory your images are placed in...");
  QHBoxLayout *workingDir = new QHBoxLayout;
  workingDir->addWidget(mWorkingDir);
  QPushButton *openDir = new QPushButton(QIcon(":/icons/outlined/icons8-folder-50.png"), "");
  // openDir->setObjectName("ToolButton");
  connect(openDir, &QPushButton::clicked, this, &DialogExperimentSettings::onOpenWorkingDirectoryClicked);
  workingDir->addWidget(openDir);
  workingDir->setStretch(0, 1);    // Make label take all available space
  layout->addRow(new QLabel(tr("Working directory:")), workingDir);

  //
  // Well order matrix
  //
  mWellOrderMatrix = new QLineEdit("[[1,2,3,4],[5,6,7,8],[9,10,11,12],[13,14,15,16]]");
  layout->addRow(new QLabel(tr("Images in well order:")), mWellOrderMatrix);

  //
  // Group by
  //
  mGroupByComboBox = new QComboBox();
  mGroupByComboBox->addItem("Ungrouped", static_cast<int>(joda::settings::ExperimentSettings::GroupBy::OFF));
  mGroupByComboBox->addItem("Group based on foldername",
                            static_cast<int>(joda::settings::ExperimentSettings::GroupBy::DIRECTORY));
  mGroupByComboBox->addItem("Group based on filename",
                            static_cast<int>(joda::settings::ExperimentSettings::GroupBy::FILENAME));
  layout->addRow(new QLabel(tr("Group by:")), mGroupByComboBox);

  //
  // Regex
  //
  mRegexToFindTheWellPosition = new QComboBox();
  mRegexToFindTheWellPosition->addItem("_((.)([0-9]+))_([0-9]+)", "_((.)([0-9]+))_([0-9]+)");
  mRegexToFindTheWellPosition->addItem("((.)([0-9]+))_([0-9]+)", "((.)([0-9]+))_([0-9]+)");
  mRegexToFindTheWellPosition->addItem("(.*)_([0-9]*)", "(.*)_([0-9]*)");
  mRegexToFindTheWellPosition->setEditable(true);
  connect(mRegexToFindTheWellPosition, &QComboBox::editTextChanged, this, &DialogExperimentSettings::applyRegex);
  connect(mRegexToFindTheWellPosition, &QComboBox::currentIndexChanged, this, &DialogExperimentSettings::applyRegex);
  connect(mRegexToFindTheWellPosition, &QComboBox::currentTextChanged, this, &DialogExperimentSettings::applyRegex);
  layout->addRow(new QLabel(tr("Filename regex:")), mRegexToFindTheWellPosition);

  //
  mTestFileName = new QLineEdit("your_test_image_file_Name_A99_01.tif");
  layout->addRow(new QLabel(tr("Regex test:")), mTestFileName);
  connect(mTestFileName, &QLineEdit::editingFinished, this, &DialogExperimentSettings::applyRegex);

  mTestFileResult = new QLabel();
  layout->addRow(new QLabel(tr("Regex test result:")), mTestFileResult);

  mExperimentGroup->setLayout(layout);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
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

  mWorkingDir->setText(mSettings.wotkingDirectory.data());
  mRegexToFindTheWellPosition->setCurrentText(mSettings.filenameRegex.data());
  mNotes->setText(mSettings.notes.data());
  mAddressOrganisation->setText(mSettings.address.organization.data());
  int idx = 0;
  for(const auto &scientist : mSettings.scientistsNames) {
    if(idx >= mScientists.size()) {
      mScientists.push_back(new QLineEdit());
      /// \todo Add Qline edit of other scientst to layout
    }
    mScientists[idx]->setText(scientist.data());
    idx++;
  }
  applyRegex();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogExperimentSettings::toSettings()
{
  mSettings.wotkingDirectory     = mWorkingDir->text().toStdString();
  mSettings.address.organization = mAddressOrganisation->text().trimmed().toStdString();
  mSettings.notes                = mNotes->toPlainText().toStdString();
  mSettings.scientistsNames.clear();
  for(const auto *textLabel : mScientists) {
    mSettings.scientistsNames.push_back(textLabel->text().trimmed().toStdString());
  }

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

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogExperimentSettings::applyRegex()
{
  try {
    auto regexResult = joda::results::Results::applyRegex(mRegexToFindTheWellPosition->currentText().toStdString(),
                                                          mTestFileName->text().toStdString());

    std::string matching = "| Group: " + regexResult.groupName;
    std::string row      = "| Row: " + std::to_string(regexResult.well.wellPosX);
    std::string column   = "| Col: " + std::to_string(regexResult.well.wellPosY);
    std::string img      = "| Img: " + std::to_string(regexResult.well.imageIdx);
    std::string toText   = matching + row + column + img;
    mTestFileResult->setText(QString(toText.data()));
  } catch(const std::exception &ex) {
    mTestFileResult->setText(ex.what());
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void DialogExperimentSettings::onOpenWorkingDirectoryClicked()
{
  QString folderToOpen      = QDir::homePath();
  QString selectedDirectory = QFileDialog::getExistingDirectory(this, "Select a directory", folderToOpen);

  if(selectedDirectory.isEmpty()) {
    return;
  }
  mWorkingDir->setText(selectedDirectory);
}

void DialogExperimentSettings::onOkayClicked()
{
}
void DialogExperimentSettings::onCancelClicked()
{
}

}    // namespace joda::ui::qt
