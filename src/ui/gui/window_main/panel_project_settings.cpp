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
#include <string>
#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/file_grouper/file_grouper_types.hpp"
#include "backend/helper/username.hpp"
#include "backend/settings/project_settings/project_image_setup.hpp"
#include "backend/settings/project_settings/project_plates.hpp"
#include "backend/settings/project_settings/project_settings.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/window_main/window_main.hpp"

namespace joda::ui::gui {

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
  // Project templates
  //
  {
    auto *projectTemplate = new QHBoxLayout;
    mTemplateSelection    = new QComboBox();
    projectTemplate->addWidget(mTemplateSelection);

    auto *bookMarkMenu = new QMenu();
    // Save template
    auto *saveTemplate = bookMarkMenu->addAction(generateIcon("save"), "Save project template");
    connect(saveTemplate, &QAction::triggered, [this]() {});
    // Open template
    auto *openTemplate = bookMarkMenu->addAction(generateIcon("open"), "Open project template");
    connect(openTemplate, &QAction::triggered, [this]() {});

    mTemplateBookmarkButton = new QPushButton(generateIcon("menu"), "");
    mTemplateBookmarkButton->setMenu(bookMarkMenu);
    mTemplateBookmarkButton->setToolTip("Menu");

    /// \todo implement save template
    // projectTemplate->addWidget(mTemplateBookmarkButton);
    projectTemplate->setStretch(0, 1);    // Make label take all available space
    formLayout->addRow(new QLabel(tr("Project template:")), projectTemplate);
    connect(mTemplateSelection, &QComboBox::currentIndexChanged, this, &PanelProjectSettings::onOpenTemplate);
  }
  addSeparator();

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

  //
  // Experiment name
  //
  mExperimentName = new QLineEdit;
  mExperimentName->addAction(generateIcon("rename"), QLineEdit::LeadingPosition);
  mExperimentName->setPlaceholderText("Experiment");
  formLayout->addRow(new QLabel(tr("Experiment name:")), mExperimentName);

  //
  // Scientist
  //
  mScientistsFirstName = new QLineEdit;
  mScientistsFirstName->addAction(generateIcon("name"), QLineEdit::LeadingPosition);
  formLayout->addRow(new QLabel(tr("Scientist:")), mScientistsFirstName);
  connect(mScientistsFirstName, &QLineEdit::editingFinished, this, &PanelProjectSettings::onSettingChanged);
  mScientistsFirstName->setPlaceholderText(joda::helper::getLoggedInUserName());

  //
  // Organization
  //
  mAddressOrganisation = new QLineEdit;
  mAddressOrganisation->addAction(generateIcon("address"), QLineEdit::LeadingPosition);
  mAddressOrganisation->setPlaceholderText("University of Salzburg");
  formLayout->addRow(new QLabel(tr("Organization:")), mAddressOrganisation);

  //
  // Experiment ID
  //
  mExperimentId = new QLineEdit;
  mExperimentId->addAction(generateIcon("binary-code"), QLineEdit::LeadingPosition);
  mExperimentId->setPlaceholderText("6fc87cc8-686e-4806-a78a-3f623c849cb7");
  /// \todo add for advanced mode
  // formLayout->addRow(new QLabel(tr("Experiment ID:")), mExperimentId);

  //
  // Job name
  //
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
  // Image Series
  //
  mImageSeries = new QComboBox();
  for(int32_t series = 0; series < 10; series++) {
    mImageSeries->addItem(("Series " + std::to_string(series)).data(), static_cast<int32_t>(series));
  }
  formLayout->addRow(new QLabel(tr("Series:")), mImageSeries);
  connect(mImageSeries, &QComboBox::currentIndexChanged, this, &PanelProjectSettings::onSettingChanged);
  connect(mImageSeries, &QComboBox::currentIndexChanged, this, &PanelProjectSettings::updateImagePreview);

  //
  // Stack handling
  //
  mStackHandlingZ = new QComboBox();
  mStackHandlingZ->addItem("Each one", static_cast<int32_t>(joda::settings::ProjectImageSetup::ZStackHandling::EACH_ONE));
  mStackHandlingZ->addItem("Defined by pipeline", static_cast<int32_t>(joda::settings::ProjectImageSetup::ZStackHandling::EXACT_ONE));
  formLayout->addRow(new QLabel(tr("Z-Stack:")), mStackHandlingZ);
  connect(mStackHandlingZ, &QComboBox::currentIndexChanged, this, &PanelProjectSettings::onSettingChanged);

  //
  mStackHandlingT = new QComboBox();
  mStackHandlingT->addItem("Each one", static_cast<int32_t>(joda::settings::ProjectImageSetup::TStackHandling::EACH_ONE));
  mStackHandlingT->addItem("Defined by pipeline", static_cast<int32_t>(joda::settings::ProjectImageSetup::TStackHandling::EXACT_ONE));
  connect(mStackHandlingT, &QComboBox::currentIndexChanged, this, &PanelProjectSettings::onSettingChanged);
  formLayout->addRow(new QLabel(tr("T-Stack:")), mStackHandlingT);

  //
  mCompositeTileSize = new QComboBox();
  mCompositeTileSize->addItem("8192x8192 (~134 MB)", static_cast<int32_t>(8192));
  mCompositeTileSize->addItem("4096x4096 (~34 MB)", static_cast<int32_t>(4096));
  mCompositeTileSize->addItem("2048x2048 (~8 MB)", static_cast<int32_t>(2048));
  mCompositeTileSize->addItem("1024x1024 (~2 MB)", static_cast<int32_t>(1024));
  mCompositeTileSize->addItem("512x512 (~0.5 MB)", static_cast<int32_t>(512));
  mCompositeTileSize->addItem("256x256 (~0.02 MB)", static_cast<int32_t>(256));
  connect(mCompositeTileSize, &QComboBox::currentIndexChanged, this, &PanelProjectSettings::onSettingChanged);
  formLayout->addRow(new QLabel(tr("Tile size:")), mCompositeTileSize);

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
/// \brief      Templates loaded from templates folder
/// \author     Joachim Danmayr
///
void PanelProjectSettings::loadTemplates()
{
  auto foundTemplates = joda::templates::TemplateParser::findTemplates(
      {{"templates/basic", joda::templates::TemplateParser::Category::BASIC},
       {"templates/eva", joda::templates::TemplateParser::Category::EVA},
       {"templates/segmentation", joda::templates::TemplateParser::Category::SEGMENTATION},
       {joda::templates::TemplateParser::getUsersTemplateDirectory().string(), joda::templates::TemplateParser::Category::USER}},
      joda::fs::EXT_PROJECT_TEMPLATE);

  mTemplateSelection->clear();
  mTemplateSelection->addItem("Load template ...", "");
  mTemplateSelection->insertSeparator(mTemplateSelection->count());
  joda::templates::TemplateParser::Category actCategory = joda::templates::TemplateParser::Category::BASIC;
  size_t addedPerCategory                               = 0;
  for(const auto &[category, dataInCategory] : foundTemplates) {
    for(const auto &[_, data] : dataInCategory) {
      // Now the user templates start, add an addition separator
      if(category != actCategory) {
        actCategory = category;
        if(addedPerCategory > 0) {
          mTemplateSelection->insertSeparator(mTemplateSelection->count());
        }
      }
      if(!data.icon.isNull()) {
        mTemplateSelection->addItem(QIcon(data.icon.scaled(28, 28)), data.title.data(), data.path.data());
      } else {
        mTemplateSelection->addItem(generateIcon("favorite"), data.title.data(), data.path.data());
      }
    }
    addedPerCategory = dataInCategory.size();
  }
}

///
/// \brief      Open template
/// \author     Joachim Danmayr
///
void PanelProjectSettings::onOpenTemplate()
{
  auto selection = mTemplateSelection->currentData().toString();
  if(selection == "") {
  } else {
    if(!askForChangeTemplateIndex()) {
      return;
    }
    mParentWindow->openProjectSettings(selection, true);
  }
  mParentWindow->checkForSettingsChanged();
  mTemplateSelection->blockSignals(true);
  mTemplateSelection->setCurrentIndex(0);
  mTemplateSelection->blockSignals(false);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
bool PanelProjectSettings::askForChangeTemplateIndex()
{
  QMessageBox messageBox(mParentWindow);
  messageBox.setIconPixmap(generateIcon("info-blue").pixmap(48, 48));
  messageBox.setWindowTitle("Proceed?");
  messageBox.setText("Actual taken settings will get lost! Load template?");
  QPushButton *noButton  = messageBox.addButton(tr("No"), QMessageBox::NoRole);
  QPushButton *yesButton = messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
  messageBox.setDefaultButton(noButton);
  auto reply = messageBox.exec();
  return messageBox.clickedButton() != noButton;
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
    auto idx = mImageSeries->findData(static_cast<int>(settings.imageSetup.series));
    if(idx >= 0) {
      mImageSeries->setCurrentIndex(idx);
    } else {
      mImageSeries->setCurrentIndex(0);
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
  {
    auto idx = mCompositeTileSize->findData(
        static_cast<int>(std::max(settings.imageSetup.imageTileSettings.tileWidth, settings.imageSetup.imageTileSettings.tileHeight)));
    if(idx >= 0) {
      mCompositeTileSize->setCurrentIndex(idx);
    } else {
      mCompositeTileSize->setCurrentIndex(0);
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

  mSettings.imageSetup.series         = static_cast<int32_t>(mImageSeries->currentData().toInt());
  mSettings.imageSetup.zStackHandling = static_cast<joda::settings::ProjectImageSetup::ZStackHandling>(mStackHandlingZ->currentData().toInt());
  mSettings.imageSetup.tStackHandling = static_cast<joda::settings::ProjectImageSetup::TStackHandling>(mStackHandlingT->currentData().toInt());
  mSettings.imageSetup.imageTileSettings.tileWidth  = static_cast<int32_t>(mCompositeTileSize->currentData().toInt());
  mSettings.imageSetup.imageTileSettings.tileHeight = static_cast<int32_t>(mCompositeTileSize->currentData().toInt());

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

}    // namespace joda::ui::gui
