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
#include <qtablewidget.h>
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>
#include <filesystem>
#include "backend/settings/settings_meta.hpp"
#include "ui/gui/helper/template_parser/template_parser.hpp"

namespace joda::ui::gui {

class DialogOpenTemplate : public QDialog
{
public:
  /////////////////////////////////////////////////////
  explicit DialogOpenTemplate(const std::set<std::string> &directories, const std::string &endian, QWidget *parent = nullptr);
  void show();
  void loadTemplates();

private:
  /////////////////////////////////////////////////////

  struct TemplateTableFilter
  {
    QString searchText;
    QString category;
  };

  /////////////////////////////////////////////////////
  int addTemplateToTable(const joda::templates::TemplateParser::Data &data, const std::string &category);
  void addTitleToTable(const std::string &title, const std::string &category);
  bool eventFilter(QObject *obj, QEvent *event) override;
  void filterCommands(const TemplateTableFilter &filter);

  /////////////////////////////////////////////////////
  const std::set<std::string> mDirectories;
  const std::string mEndian;

  QLineEdit *mSearch = nullptr;
  QTableWidget *mTableTemplates;

  std::vector<joda::templates::TemplateParser::Data> mTemplateList;
  std::map<int32_t, int32_t> mTemplateMap;       // Key is the array index of the command in the mCommandList vector, value the index in the table
  std::map<std::string, int32_t> mTitleINdex;    // Key is the group ID, value the index in the table
};

}    // namespace joda::ui::gui
