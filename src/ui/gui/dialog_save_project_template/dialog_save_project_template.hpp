///
/// \file      dialog_save_project_template.hpp
/// \author    Joachim Danmayr
/// \date      2025-04-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qcombobox.h>
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>
#include "backend/settings/settings_meta.hpp"

namespace joda::ui::gui {

class DialogSaveProjectTemplate : public QDialog
{
public:
  QLineEdit *mName;
  QLineEdit *mNotes;
  QLineEdit *mVersion;
  QComboBox *mGroup;
  QLineEdit *mCategory;
  QLineEdit *mTags;
  QLineEdit *mAuthor;
  QLineEdit *mOrganization;

  explicit DialogSaveProjectTemplate(QWidget *parent = nullptr) : QDialog(parent)
  {
    setWindowTitle("Enter template information");
    setMinimumWidth(400);

    auto *mainLayout = new QVBoxLayout(this);

    auto *formLayout = new QFormLayout;
    mName            = new QLineEdit;
    mNotes           = new QLineEdit;
    mVersion         = new QLineEdit;
    mGroup           = new QComboBox;
    mGroup->addItem("EVAnalyzer", "EVAnalyzer");
    mGroup->addItem("Userdefined", "Userdefined");
    mCategory = new QLineEdit;
    mTags     = new QLineEdit;
    mTags->setPlaceholderText("tag1;tag2");
    mAuthor       = new QLineEdit;
    mOrganization = new QLineEdit;

    formLayout->addRow("Name:", mName);
    formLayout->addRow("Notes:", mNotes);
    formLayout->addRow("Version:", mVersion);
    formLayout->addRow("Group:", mGroup);
    formLayout->addRow("Category:", mCategory);
    formLayout->addRow("Tags:", mTags);
    formLayout->addRow("Author:", mAuthor);
    formLayout->addRow("Organization:", mOrganization);

    mainLayout->addLayout(formLayout);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);
  }

  std::vector<std::string> convertToStdVector(const QString &input)
  {
    // Split on comma or semicolon using QRegularExpression
    QStringList list = input.split(QRegularExpression("[,;]"), Qt::SkipEmptyParts);

    std::vector<std::string> result;
    for(const QString &s : list) {
      result.push_back(s.trimmed().toStdString());    // Trim whitespace and convert
    }
    return result;
  }

  joda::settings::SettingsMeta toSettingsMeta()
  {
    std::optional<std::string> category;
    if(!mCategory->text().isEmpty()) {
      category = mCategory->text().toStdString();
    }

    std::optional<std::string> author;
    if(!mAuthor->text().isEmpty()) {
      author = mAuthor->text().toStdString();
    }

    std::optional<std::string> organization;
    if(!mOrganization->text().isEmpty()) {
      organization = mOrganization->text().toStdString();
    }

    return joda::settings::SettingsMeta{.name         = mName->text().toStdString(),
                                        .notes        = mNotes->text().toStdString(),
                                        .revision     = mVersion->text().toStdString(),
                                        .group        = mGroup->currentData().toString().toStdString(),
                                        .category     = category,
                                        .tags         = convertToStdVector(mTags->text()),
                                        .author       = author,
                                        .organization = organization};
  }
};

}    // namespace joda::ui::gui
