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
/// \brief     A short description what happens here.
///

#include "panel_project_settings.hpp"
#include "backend/helper/username.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui::qt {

struct Temp
{
  std::vector<std::vector<int32_t>> order;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Temp, order);
};

///
/// \brief      Constructor
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
PanelProjectSettings::PanelProjectSettings(joda::settings::ExperimentSettings &settings, WindowMain *parentWindow) :
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
  QPushButton *openDir = new QPushButton(QIcon(":/icons/outlined/icons8-folder-50.png"), "");
  connect(openDir, &QPushButton::clicked, this, &PanelProjectSettings::onOpenWorkingDirectoryClicked);
  workingDir->addWidget(openDir);
  workingDir->setStretch(0, 1);    // Make label take all available space
  formLayout->addRow(new QLabel(tr("Working directory:")), workingDir);

  addSeparator();

  for(int i = 0; i < NR_OF_SCIENTISTS; ++i) {
    mScientists[i] = new QLineEdit;
    formLayout->addRow(new QLabel(tr("Scientist:")), mScientists[i]);
    connect(mScientists[i], &QLineEdit::editingFinished, this, &PanelProjectSettings::onSettingChanged);
  }

  mScientists[0]->setPlaceholderText(joda::helper::getLoggedInUserName());

  mAddressOrganisation = new QLineEdit;
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
  mGroupByComboBox->addItem("Ungrouped", static_cast<int>(joda::settings::ExperimentSettings::GroupBy::OFF));
  mGroupByComboBox->addItem("Group based on foldername",
                            static_cast<int>(joda::settings::ExperimentSettings::GroupBy::DIRECTORY));
  mGroupByComboBox->addItem("Group based on filename",
                            static_cast<int>(joda::settings::ExperimentSettings::GroupBy::FILENAME));
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

  //
  // Well order matrix
  //
  mWellOrderMatrix      = new QLineEdit("[[1,2,3,4],[5,6,7,8],[9,10,11,12],[13,14,15,16]]");
  mWellOrderMatrixLabel = new QLabel(tr("Well order:"));
  formLayout->addRow(mWellOrderMatrixLabel, mWellOrderMatrix);

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
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelProjectSettings::fromSettings(joda::settings::ExperimentSettings &settings)
{
  {
    auto idx = mGroupByComboBox->findData(static_cast<int>(settings.groupBy));
    if(idx >= 0) {
      mGroupByComboBox->setCurrentIndex(idx);
    } else {
      mGroupByComboBox->setCurrentIndex(0);
    }
  }

  {
    auto idx = mGroupByComboBox->findData(static_cast<int>(settings.groupBy));
    if(idx >= 0) {
      mGroupByComboBox->setCurrentIndex(idx);
    } else {
      mGroupByComboBox->setCurrentIndex(0);
    }
  }

  {
    try {
      Temp tm{.order = settings.wellImageOrder};
      nlohmann::json j = tm;
      j                = j["order"];
      mWellOrderMatrix->setText(j.dump().data());
    } catch(...) {
      mWellOrderMatrix->setText("[[1,2,3,4],[5,6,7,8]]");
    }
  }
  {
    auto val = settings.plateSize.rows * 100 + settings.plateSize.cols;
    auto idx = mPlateSize->findData(val);
    if(idx >= 0) {
      mPlateSize->setCurrentIndex(idx);
    }
  }

  mWorkingDir->setText(settings.workingDirectory.data());
  mRegexToFindTheWellPosition->setCurrentText(settings.filenameRegex.data());
  mNotes->setText(settings.notes.data());
  mAddressOrganisation->setText(settings.address.organization.data());
  int idx = 0;
  for(const auto &scientist : settings.scientistsNames) {
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
void PanelProjectSettings::toSettings()
{
  mSettings.workingDirectory     = mWorkingDir->text().toStdString();
  mSettings.address.organization = mAddressOrganisation->text().trimmed().toStdString();
  mSettings.notes                = mNotes->toPlainText().toStdString();
  mSettings.scientistsNames.clear();
  for(const auto *textLabel : mScientists) {
    if(!textLabel->text().trimmed().isEmpty()) {
      mSettings.scientistsNames.push_back(textLabel->text().trimmed().toStdString());
    }
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

  auto value               = mPlateSize->currentData().toUInt();
  mSettings.plateSize.rows = value / 100;
  mSettings.plateSize.cols = value % 100;

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
  mSettings.workingDirectory = mWorkingDir->text().toStdString();
  mParentWindow->getController()->setWorkingDirectory(mSettings.workingDirectory);
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
    auto regexResult = joda::results::Results::applyRegex(mRegexToFindTheWellPosition->currentText().toStdString(),
                                                          mTestFileName->text().toStdString());

    if(regexResult.groupName.length() > 20) {
      regexResult.groupName =
          (QString(regexResult.groupName.data()).left(8) + "..." + QString(regexResult.groupName.data()).right(10))
              .toStdString();
    }
    std::string matching = "<html><b>Group:</b> " + regexResult.groupName;
    std::string row;
    if(regexResult.well.wellPosX != UINT16_MAX) {
      row = "| <b>Row:</b> " + std::to_string(regexResult.well.wellPosX);
    }
    std::string column;
    if(regexResult.well.wellPosY != UINT16_MAX) {
      column = "| <b>Col:</b> " + std::to_string(regexResult.well.wellPosY);
    }
    std::string img    = "| <b>Img:</b> " + std::to_string(regexResult.well.imageIdx);
    std::string toText = matching + row + column + img + "</html>";
    mTestFileResult->setText(QString(toText.data()));
  } catch(const std::exception &ex) {
    mTestFileResult->setText(ex.what());
  }
  onSettingChanged();
}

}    // namespace joda::ui::qt
