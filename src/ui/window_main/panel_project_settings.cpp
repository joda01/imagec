///
/// \file      panel_project_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-06
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "panel_project_settings.hpp"
#include <qcombobox.h>
#include "backend/helper/file_grouper/file_grouper_types.hpp"
#include "backend/helper/username.hpp"
#include "backend/settings/project_settings/project_image_setup.hpp"
#include "backend/settings/project_settings/project_plates.hpp"
#include "backend/settings/project_settings/project_settings.hpp"
#include "ui/helper/icon_generator.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
PanelProjectSettings::PanelProjectSettings(joda::settings::AnalyzeSettings &settings, WindowMain *parentWindow) :
    mSettings(settings), mParentWindow(parentWindow)
{
  auto *layout            = new QVBoxLayout(this);
  QFormLayout *formLayout = new QFormLayout;

  auto addSeparator = [&formLayout]() {
    QFrame *separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    formLayout->addRow(separator);
  };

  //
  // Working directory
  //
  mWorkingDir = new QLineEdit();
  mWorkingDir->setReadOnly(true);
  mWorkingDir->setPlaceholderText("Directory your images are placed in...");
  QHBoxLayout *workingDir = new QHBoxLayout;
  workingDir->addWidget(mWorkingDir);
  QPushButton *openDir = new QPushButton(generateIcon("images-folder"), "");
  connect(openDir, &QPushButton::clicked, this, &PanelProjectSettings::onOpenWorkingDirectoryClicked);
  workingDir->addWidget(openDir);
  workingDir->setStretch(0, 1);    // Make label take all available space
  formLayout->addRow(new QLabel(tr("Working directory:")), workingDir);

  addSeparator();

  mScientistsFirstName = new QLineEdit;
  mScientistsFirstName->addAction(generateIcon("name"), QLineEdit::LeadingPosition);
  formLayout->addRow(new QLabel(tr("Scientist:")), mScientistsFirstName);
  connect(mScientistsFirstName, &QLineEdit::editingFinished, this, &PanelProjectSettings::onSettingChanged);
  mScientistsFirstName->setPlaceholderText(joda::helper::getLoggedInUserName());

  mAddressOrganisation = new QLineEdit;
  mAddressOrganisation->addAction(generateIcon("address"), QLineEdit::LeadingPosition);
  mAddressOrganisation->setPlaceholderText("University of Salzburg");
  formLayout->addRow(new QLabel(tr("Organization:")), mAddressOrganisation);

  mJobName = new QLineEdit;
  mJobName->setPlaceholderText(joda::helper::RandomNameGenerator::GetRandomName().data());
  formLayout->addRow(new QLabel(tr("Job name:")), mJobName);

  addSeparator();

  //
  // Group by
  //
  mGroupByComboBox = new QComboBox();
  mGroupByComboBox->addItem("Ungrouped", static_cast<int>(joda::enums::GroupBy::OFF));
  mGroupByComboBox->addItem("Group based on foldername", static_cast<int>(joda::enums::GroupBy::DIRECTORY));
  mGroupByComboBox->addItem("Group based on filename", static_cast<int>(joda::enums::GroupBy::FILENAME));
  formLayout->addRow(new QLabel(tr("Group by:")), mGroupByComboBox);

  //
  // Regex
  //
  mRegexToFindTheWellPosition = new QComboBox();
  mRegexToFindTheWellPosition->addItem("_((.)([0-9]+))_([0-9]+)", "_((.)([0-9]+))_([0-9]+)");
  mRegexToFindTheWellPosition->addItem("((.)([0-9]+))_([0-9]+)", "((.)([0-9]+))_([0-9]+)");
  mRegexToFindTheWellPosition->addItem("(.*)_([0-9]*)", "(.*)_([0-9]*)");
  mRegexToFindTheWellPosition->setEditable(true);
  mRegexToFindTheWellPositionLabel = new QLabel(tr("Filename regex:"));
  formLayout->addRow(mRegexToFindTheWellPositionLabel, mRegexToFindTheWellPosition);

  //
  mTestFileName      = new QLineEdit("your_test_image_file_Name_A99_01.tif");
  mTestFileNameLabel = new QLabel(tr("Regex test:"));
  formLayout->addRow(mTestFileNameLabel, mTestFileName);

  mTestFileResult = new QLabel();
  formLayout->addRow(mTestFileResult);

  addSeparator();

  //
  // Stack handling
  //
  mStackHandlingZ = new QComboBox();
  mStackHandlingZ->addItem("Intensity projection", static_cast<int32_t>(joda::settings::ProjectImageSetup::ZStackHandling::INTENSITY_PROJECTION));
  mStackHandlingZ->addItem("Each one", static_cast<int32_t>(joda::settings::ProjectImageSetup::ZStackHandling::EACH_ONE));
  mStackHandlingZ->addItem("Exact one", static_cast<int32_t>(joda::settings::ProjectImageSetup::ZStackHandling::EXACT_ONE));
  formLayout->addRow(new QLabel(tr("Z-Stack:")), mStackHandlingZ);
  connect(mStackHandlingZ, &QComboBox::currentIndexChanged, this, &PanelProjectSettings::onSettingChanged);

  //
  mStackHandlingT = new QComboBox();
  mStackHandlingT->addItem("Each one", static_cast<int32_t>(joda::settings::ProjectImageSetup::TStackHandling::EACH_ONE));
  mStackHandlingT->addItem("Exact one", static_cast<int32_t>(joda::settings::ProjectImageSetup::TStackHandling::EACH_ONE));
  connect(mStackHandlingT, &QComboBox::currentIndexChanged, this, &PanelProjectSettings::onSettingChanged);

  formLayout->addRow(new QLabel(tr("T-Stack:")), mStackHandlingT);

  addSeparator();

  //
  // Well order matrix
  //
  mWellOrderMatrix      = new QLineEdit("[[1,2,3,4],[5,6,7,8],[9,10,11,12],[13,14,15,16]]");
  mWellOrderMatrixLabel = new QLabel(tr("Well order:"));
  formLayout->addRow(mWellOrderMatrixLabel, mWellOrderMatrix);

  //
  // Plate size
  //
  mPlateSize = new QComboBox();
  mPlateSize->addItem("1", 1);
  mPlateSize->addItem("2 x 3", 203);
  mPlateSize->addItem("3 x 4", 304);
  mPlateSize->addItem("4 x 6", 406);
  mPlateSize->addItem("6 x 8", 608);
  mPlateSize->addItem("8 x 12", 812);
  mPlateSize->addItem("16 x 24", 1624);
  mPlateSize->addItem("32 x 48", 3248);
  mPlateSize->addItem("48 x 72", 4872);
  formLayout->addRow(new QLabel(tr("Plate size:")), mPlateSize);

  addSeparator();

  layout->addLayout(formLayout);

  mNotes = new QTextEdit;
  mNotes->setPlaceholderText("Notes on the experiment...");
  layout->addWidget(mNotes);

  layout->addStretch();
  setLayout(layout);

  connect(mPlateSize, &QComboBox::currentIndexChanged, this, &PanelProjectSettings::onSettingChanged);
  connect(mNotes, &QTextEdit::textChanged, this, &PanelProjectSettings::onSettingChanged);
  connect(mTestFileName, &QLineEdit::textChanged, this, &PanelProjectSettings::applyRegex);
  connect(mRegexToFindTheWellPosition, &QComboBox::editTextChanged, this, &PanelProjectSettings::applyRegex);
  connect(mRegexToFindTheWellPosition, &QComboBox::currentIndexChanged, this, &PanelProjectSettings::applyRegex);
  connect(mRegexToFindTheWellPosition, &QComboBox::currentTextChanged, this, &PanelProjectSettings::applyRegex);
  connect(mGroupByComboBox, &QComboBox::currentIndexChanged, this, &PanelProjectSettings::onSettingChanged);
  connect(mAddressOrganisation, &QLineEdit::textChanged, this, &PanelProjectSettings::onSettingChanged);
  connect(mWellOrderMatrix, &QLineEdit::textChanged, this, &PanelProjectSettings::onSettingChanged);

  mPlateSize->setCurrentIndex(6);
  applyRegex();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelProjectSettings::fromSettings(const joda::settings::AnalyzeSettings &settings)
{
  {
    auto idx = mGroupByComboBox->findData(static_cast<int>(settings.projectSettings.plates.begin()->groupBy));
    if(idx >= 0) {
      mGroupByComboBox->setCurrentIndex(idx);
    } else {
      mGroupByComboBox->setCurrentIndex(0);
    }
  }

  {
    auto idx = mGroupByComboBox->findData(static_cast<int>(settings.projectSettings.plates.begin()->groupBy));
    if(idx >= 0) {
      mGroupByComboBox->setCurrentIndex(idx);
    } else {
      mGroupByComboBox->setCurrentIndex(0);
    }
  }

  {
    mWellOrderMatrix->setText(joda::settings::vectorToString(settings.projectSettings.plates.begin()->wellImageOrder).data());
  }
  {
    auto val = settings.projectSettings.plates.begin()->rows * 100 + settings.projectSettings.plates.begin()->cols;
    auto idx = mPlateSize->findData(val);
    if(idx >= 0) {
      mPlateSize->setCurrentIndex(idx);
    }
  }
  {
    auto idx = mStackHandlingZ->findData(static_cast<int>(settings.imageSetup.zStackHandling));
    if(idx >= 0) {
      mStackHandlingZ->setCurrentIndex(idx);
    } else {
      mStackHandlingZ->setCurrentIndex(0);
    }
  }
  {
    auto idx = mStackHandlingT->findData(static_cast<int>(settings.imageSetup.tStackHandling));
    if(idx >= 0) {
      mStackHandlingT->setCurrentIndex(idx);
    } else {
      mStackHandlingT->setCurrentIndex(0);
    }
  }

  mWorkingDir->setText(settings.projectSettings.workingDirectory.data());
  mRegexToFindTheWellPosition->setCurrentText(settings.projectSettings.plates.begin()->filenameRegex.data());
  mNotes->setText(settings.projectSettings.experimentSettings.notes.data());
  mAddressOrganisation->setText(settings.projectSettings.address.organization.data());
  mScientistsFirstName->setText(settings.projectSettings.address.firstName.data());

  mJobName->clear();
  applyRegex();
  mParentWindow->getController()->setWorkingDirectory(settings.projectSettings.plates.begin()->plateId,
                                                      settings.projectSettings.plates.begin()->imageFolder);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelProjectSettings::toSettings()
{
  mSettings.projectSettings.workingDirectory         = mWorkingDir->text().toStdString();
  mSettings.projectSettings.address.organization     = mAddressOrganisation->text().trimmed().toStdString();
  mSettings.projectSettings.experimentSettings.notes = mNotes->toPlainText().toStdString();
  mSettings.projectSettings.address.firstName        = mScientistsFirstName->text().toStdString();

  mSettings.projectSettings.plates.begin()->groupBy        = static_cast<joda::enums::GroupBy>(mGroupByComboBox->currentData().toInt());
  mSettings.projectSettings.plates.begin()->filenameRegex  = mRegexToFindTheWellPosition->currentText().toStdString();
  mSettings.projectSettings.plates.begin()->imageFolder    = mWorkingDir->text().toStdString();
  mSettings.projectSettings.plates.begin()->wellImageOrder = joda::settings::stringToVector(mWellOrderMatrix->text().toStdString());

  auto value                                        = mPlateSize->currentData().toUInt();
  mSettings.projectSettings.plates.begin()->rows    = value / 100;
  mSettings.projectSettings.plates.begin()->cols    = value % 100;
  mSettings.projectSettings.plates.begin()->plateId = 1;

  mSettings.imageSetup.zStackHandling = static_cast<joda::settings::ProjectImageSetup::ZStackHandling>(mStackHandlingZ->currentData().toInt());
  mSettings.imageSetup.tStackHandling = static_cast<joda::settings::ProjectImageSetup::TStackHandling>(mStackHandlingT->currentData().toInt());

  mParentWindow->checkForSettingsChanged();
}

///
/// \brief
/// \author     Joachim Danmayr
///
void PanelProjectSettings::onOpenWorkingDirectoryClicked()
{
  QString folderToOpen      = QDir::homePath();
  QString selectedDirectory = QFileDialog::getExistingDirectory(this, "Select a directory", folderToOpen);

  if(selectedDirectory.isEmpty()) {
    return;
  }
  mWorkingDir->setText(selectedDirectory);
  mWorkingDir->update();
  mWorkingDir->repaint();
  mSettings.projectSettings.workingDirectory = mWorkingDir->text().toStdString();
  mParentWindow->getController()->setWorkingDirectory(mSettings.projectSettings.plates.begin()->plateId, selectedDirectory.toStdString());
  mSettings.projectSettings.plates.begin()->imageFolder = mSettings.projectSettings.workingDirectory;
  onSettingChanged();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelProjectSettings::onSettingChanged()
{
  toSettings();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelProjectSettings::applyRegex()
{
  try {
    auto regexResult =
        joda::grp::FileGrouper::applyRegex(mRegexToFindTheWellPosition->currentText().toStdString(), mTestFileName->text().toStdString());

    if(regexResult.groupName.length() > 20) {
      regexResult.groupName = (QString(regexResult.groupName.data()).left(8) + "..." + QString(regexResult.groupName.data()).right(10)).toStdString();
    }
    std::string matching = "<html><b>Group:</b> " + regexResult.groupName;
    std::string row;
    if(regexResult.wellPosX != UINT16_MAX) {
      row = "| <b>Row:</b> " + std::to_string(regexResult.wellPosX);
    }
    std::string column;
    if(regexResult.wellPosY != UINT16_MAX) {
      column = "| <b>Col:</b> " + std::to_string(regexResult.wellPosY);
    }
    std::string img    = "| <b>Img:</b> " + std::to_string(regexResult.imageIdx);
    std::string toText = matching + row + column + img + "</html>";
    mTestFileResult->setText(QString(toText.data()));
  } catch(const std::exception &ex) {
    mTestFileResult->setText(ex.what());
  }
  onSettingChanged();
}

}    // namespace joda::ui
