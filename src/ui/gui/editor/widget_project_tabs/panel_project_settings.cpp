///
/// \file      panel_project_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-06
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "panel_project_settings.hpp"
#include <qcombobox.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <string>
#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/file_grouper/file_grouper_types.hpp"
#include "backend/helper/username.hpp"
#include "backend/settings/project_settings/project_image_setup.hpp"
#include "backend/settings/project_settings/project_plates.hpp"
#include "backend/settings/project_settings/project_settings.hpp"
#include "ui/gui/dialogs/dialog_plate_settings/dialog_plate_settings.hpp"
#include "ui/gui/editor/window_main.hpp"
#include "ui/gui/helper/combo_placeholder.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/iconless_dialog_button_box.hpp"

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
  auto *layout        = new QVBoxLayout(this);
  auto *formLayoutOut = new QFormLayout;

  auto addSeparator = [&formLayoutOut]() {
    auto *separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    formLayoutOut->addRow(separator);
  };

  //
  // Working directory
  //
  {
    mImageFilePath = new QLineEdit();
    mImageFilePath->setReadOnly(true);
    mImageFilePath->setPlaceholderText("Directory your images are placed in...");
    auto *workingDir = new QHBoxLayout;
    workingDir->addWidget(mImageFilePath);
    auto *openDir = new QPushButton(generateSvgIcon<Style::REGULAR, Color::BLUE>("folder-open"), "");
    openDir->setStatusTip("Select image directory");
    connect(openDir, &QPushButton::clicked, this, &PanelProjectSettings::onOpenWorkingDirectoryClicked);
    workingDir->addWidget(openDir);
    workingDir->setStretch(0, 1);    // Make label take all available space
    formLayoutOut->addRow(new QLabel(tr("Image directory")), workingDir);
  }
  //
  // Project path
  //
  {
    mProjectFilePath = new QLineEdit();
    mProjectFilePath->setReadOnly(true);
    mProjectFilePath->setPlaceholderText("Project path");
    auto *projectPath = new QHBoxLayout;
    projectPath->addWidget(mProjectFilePath);
    mOpenProjectPath = new QPushButton(generateSvgIcon<Style::REGULAR, Color::BLUE>("arrow-square-out"), "");
    mOpenProjectPath->setEnabled(false);
    mOpenProjectPath->setStatusTip("Path ImageC uses to store project settings");
    connect(mOpenProjectPath, &QPushButton::clicked, [this]() { QDesktopServices::openUrl(QUrl("file:///" + mProjectFilePath->text())); });
    projectPath->addWidget(mOpenProjectPath);
    projectPath->setStretch(0, 1);    // Make label take all available space
    formLayoutOut->addRow(new QLabel(tr("Project path")), projectPath);
  }    // Meta edit dialog
  {
    //
    // Job name
    //
    mJobName = new QLineEdit;
    mJobName->addAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("person-simple-run"), QLineEdit::LeadingPosition);
    mJobName->setPlaceholderText(joda::helper::RandomNameGenerator::GetRandomName().data());

    auto *expirmentMeta = new QHBoxLayout;
    expirmentMeta->addWidget(mJobName);
    auto *openMetaEditor = new QPushButton(generateSvgIcon<Style::REGULAR, Color::BLACK>("dots-three-outline-vertical"), "");
    openMetaEditor->setStatusTip("Project meta information");
    connect(openMetaEditor, &QPushButton::clicked, [this] { mMetaEditDialog->exec(); });
    expirmentMeta->addWidget(openMetaEditor);
    expirmentMeta->setStretch(0, 1);    // Make label take all available space
    formLayoutOut->addRow(new QLabel(tr("Job name")), expirmentMeta);

    mMetaEditDialog = new QDialog(mParentWindow);
    mMetaEditDialog->setWindowTitle("Project meta");
    mMetaEditDialog->setMinimumWidth(400);
    auto *formLayout = new QFormLayout;

    //
    // Experiment name
    //
    mExperimentName = new QLineEdit;
    mExperimentName->addAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("text-t"), QLineEdit::LeadingPosition);
    mExperimentName->setPlaceholderText("Experiment");
    formLayout->addRow(new QLabel(tr("Experiment title")), mExperimentName);

    //
    // Scientist
    //
    mScientistsFirstName = new QLineEdit;
    mScientistsFirstName->addAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("user"), QLineEdit::LeadingPosition);
    formLayout->addRow(new QLabel(tr("Scientist")), mScientistsFirstName);
    connect(mScientistsFirstName, &QLineEdit::editingFinished, this, &PanelProjectSettings::onSettingChanged);
    mScientistsFirstName->setPlaceholderText(joda::helper::getLoggedInUserName());

    //
    // Organization
    //
    mAddressOrganisation = new QLineEdit;
    mAddressOrganisation->addAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("map-pin"), QLineEdit::LeadingPosition);
    mAddressOrganisation->setPlaceholderText("University of Salzburg");
    formLayout->addRow(new QLabel(tr("Organization")), mAddressOrganisation);

    //
    // Experiment ID
    //
    mExperimentId = new QLineEdit;
    mExperimentId->addAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("binary"), QLineEdit::LeadingPosition);
    mExperimentId->setPlaceholderText("6fc87cc8-686e-4806-a78a-3f623c849cb7");
    formLayout->addRow(new QLabel(tr("Experiment ID")), mExperimentId);

    // Okay and canlce
    auto *buttonBox = new IconlessDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
    connect(buttonBox, &QDialogButtonBox::accepted, mMetaEditDialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, mMetaEditDialog, &QDialog::reject);
    formLayout->addWidget(buttonBox);

    mMetaEditDialog->setLayout(formLayout);
  }

  addSeparator();

  // Grouping settings
  {
    //
    // Group by
    //
    mGroupByComboBox = new ComboWithPlaceholder();
    mGroupByComboBox->setPlaceholderText("Select option...");
    mGroupByComboBox->addItem("Ungrouped", static_cast<int>(joda::enums::GroupBy::OFF));
    mGroupByComboBox->addItem("Group based on foldername", static_cast<int>(joda::enums::GroupBy::DIRECTORY));
    mGroupByComboBox->addItem("Group based on filename", static_cast<int>(joda::enums::GroupBy::FILENAME));
    mGroupByComboBox->setCurrentIndex(-1);
    connect(mGroupByComboBox, &QComboBox::currentIndexChanged, [this](int /*index*/) {
      if(mGroupByComboBox->currentData().toInt() == static_cast<int>(joda::enums::GroupBy::FILENAME)) {
        mOpenGroupingSettings->setEnabled(true);
        mGroupingDialog->exec();
      } else {
        mOpenGroupingSettings->setEnabled(false);
      }
    });

    auto *groupingLayout = new QHBoxLayout;
    groupingLayout->addWidget(mGroupByComboBox);
    mOpenGroupingSettings = new QPushButton(generateSvgIcon<Style::REGULAR, Color::BLACK>("dots-three-outline-vertical"), "");
    mOpenGroupingSettings->setStatusTip("Grouping settings");
    mOpenGroupingSettings->setEnabled(false);
    connect(mOpenGroupingSettings, &QPushButton::clicked, [this] { mGroupingDialog->exec(); });
    groupingLayout->addWidget(mOpenGroupingSettings);
    groupingLayout->setStretch(0, 1);    // Make label take all available space
    formLayoutOut->addRow(new QLabel(tr("Grouping")), groupingLayout);

    mGroupingDialog = new QDialog(parentWindow);
    mGroupingDialog->setWindowTitle("Grouping settings");
    mGroupingDialog->setMinimumWidth(400);
    auto *formLayout = new QFormLayout;

    //
    // Regex
    //
    mRegexToFindTheWellPosition = new QComboBox();
    mRegexToFindTheWellPosition->addItem("_((.)([0-9]+))_([0-9]+)", "_((.)([0-9]+))_([0-9]+)");
    mRegexToFindTheWellPosition->addItem("((.)([0-9]+))_([0-9]+)", "((.)([0-9]+))_([0-9]+)");
    mRegexToFindTheWellPosition->addItem("(.*)_([0-9]*)", "(.*)_([0-9]*)");
    mRegexToFindTheWellPosition->setEditable(true);
    formLayout->addRow("Filename regex", mRegexToFindTheWellPosition);

    //
    mTestFileName = new QLineEdit("your_test_image_file_Name_A99_01.tif");
    formLayout->addRow("Regex test", mTestFileName);

    mTestFileResult = new QLabel();
    formLayout->addRow(mTestFileResult);

    // Okay and canlce
    auto *buttonBox = new IconlessDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
    connect(buttonBox, &QDialogButtonBox::accepted, mGroupingDialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, mGroupingDialog, &QDialog::reject);
    formLayout->addWidget(buttonBox);

    mGroupingDialog->setLayout(formLayout);
  }

  // Plate settings
  {
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

    auto *plateSettingsLayout = new QHBoxLayout;
    auto *openPlateSettings   = new QPushButton(generateSvgIcon<Style::REGULAR, Color::BLACK>("dots-three-outline-vertical"), "");
    openPlateSettings->setStatusTip("Well image order settings");
    connect(openPlateSettings, &QPushButton::clicked, [this] {
      auto *dialogPlateSettings = new DialogPlateSettings(&mSettings.projectSettings.plate.plateSetup.wellImageOrder, mParentWindow);
      if(dialogPlateSettings->exec() == QDialog::Accepted) {
        emit onSettingChanged();
      }
    });

    plateSettingsLayout->addWidget(mPlateSize);
    plateSettingsLayout->addWidget(openPlateSettings);
    plateSettingsLayout->setStretch(0, 1);    // Make label take all available space
    formLayoutOut->addRow(new QLabel(tr("Plate settings")), plateSettingsLayout);
  }

  layout->addLayout(formLayoutOut);

  mNotes = new QTextEdit;
  mNotes->setPlaceholderText("Notes on the experiment...");
  // layout->addWidget(mNotes);

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
  mGroupByComboBox->blockSignals(true);
  mPlateSize->blockSignals(true);
  mNotes->blockSignals(true);
  mAddressOrganisation->blockSignals(true);

  {
    auto idx = mGroupByComboBox->findData(static_cast<int>(settings.projectSettings.plate.groupBy));
    if(idx >= 0) {
      mGroupByComboBox->setCurrentIndex(idx);
    } else {
      mGroupByComboBox->setCurrentIndex(-1);
    }
  }
  {
    auto val = settings.projectSettings.plate.plateSetup.rows * 100 + settings.projectSettings.plate.plateSetup.cols;
    auto idx = mPlateSize->findData(val);
    if(idx >= 0) {
      mPlateSize->setCurrentIndex(idx);
    }
  }

  mImageFilePath->setText(settings.projectSettings.plate.imageFolder.data());
  mRegexToFindTheWellPosition->setCurrentText(settings.projectSettings.plate.filenameRegex.data());
  mNotes->setText(settings.projectSettings.experimentSettings.notes.data());
  mAddressOrganisation->setText(settings.projectSettings.address.organization.data());
  mScientistsFirstName->setText(settings.projectSettings.address.firstName.data());

  mJobName->clear();
  applyRegex();
  mParentWindow->getController()->setWorkingDirectory(settings.projectSettings.plate.imageFolder);

  mGroupByComboBox->blockSignals(false);
  mPlateSize->blockSignals(false);
  mNotes->blockSignals(false);
  mAddressOrganisation->blockSignals(false);
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
  mSettings.projectSettings.address.organization     = mAddressOrganisation->text().trimmed().toStdString();
  mSettings.projectSettings.experimentSettings.notes = mNotes->toPlainText().toStdString();
  mSettings.projectSettings.address.firstName        = mScientistsFirstName->text().toStdString();

  mSettings.projectSettings.plate.groupBy       = static_cast<joda::enums::GroupBy>(mGroupByComboBox->currentData().toInt());
  mSettings.projectSettings.plate.filenameRegex = mRegexToFindTheWellPosition->currentText().toStdString();
  mSettings.projectSettings.plate.imageFolder   = mImageFilePath->text().toStdString();

  auto value                                      = mPlateSize->currentData().toUInt();
  mSettings.projectSettings.plate.plateSetup.rows = value / 100;
  mSettings.projectSettings.plate.plateSetup.cols = value % 100;
  mSettings.projectSettings.plate.plateId         = 1;

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
  mImageFilePath->setText(selectedDirectory);
  mImageFilePath->update();
  mImageFilePath->repaint();
  mParentWindow->getController()->setWorkingDirectory(selectedDirectory.toStdString());
  mSettings.projectSettings.plate.imageFolder = mImageFilePath->text().toStdString();
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
