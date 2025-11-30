///
/// \file      panel_image.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "panel_classification_list.hpp"
#include <qaction.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qgridlayout.h>
#include <qlayoutitem.h>
#include <qlineedit.h>
#include <qmenu.h>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <qwidget.h>
#include <exception>
#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/file_parser/directory_iterator.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/project_settings/project_class.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "backend/settings/project_settings/project_plates.hpp"
#include "backend/settings/settings.hpp"
#include "ui/gui/editor/widget_project_tabs/table_model_classes.hpp"
#include "ui/gui/helper/color_combo/color_combo.hpp"
#include "ui/gui/helper/colord_square_delegate.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/results/dialog_class_settings.hpp"
#include "ui/gui/results/window_results.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::gui {

PanelClassificationList::PanelClassificationList(settings::ResultsSettings *settings) : mResultsSettings(settings)
{
  setWindowTitle("Column settings");
  setFeatures(features() & ~QDockWidget::DockWidgetClosable);
  mClassSettingsDialog = new DialogClassSettings(this);
  mClassSettingsDialog->setEditable(false);
  auto *centralWidget = new QWidget();
  centralWidget->setContentsMargins(0, 0, 0, 0);
  auto *layout = new QVBoxLayout();
  layout->setContentsMargins(0, 2, 0, 0);
  centralWidget->setLayout(layout);
  setMaximumWidth(350);
  setMinimumWidth(350);

  {
    auto *toolbar = new QToolBar();

    layout->addWidget(toolbar);
  }

  {
    mTableClasses = new PlaceholderTableView(this);
    mTableClasses->setPlaceholderText("No classes");
    mTableClasses->setFrameStyle(QFrame::NoFrame);
    mTableClasses->verticalHeader()->setVisible(false);
    mTableClasses->horizontalHeader()->setVisible(true);
    mTableClasses->setAlternatingRowColors(true);
    mTableClasses->setSelectionBehavior(QAbstractItemView::SelectRows);
    mTableClasses->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mTableClasses->setItemDelegateForColumn(0, new ColoredSquareDelegate(mTableClasses));
    mTableModelClasses = new TableModelClasses(&mClassesList, nullptr, mTableClasses);
    mTableClasses->setModel(mTableModelClasses);
    layout->addWidget(mTableClasses);
  }

  setWidget(centralWidget);
  connect(mTableClasses, &QTableView::doubleClicked, [&](const QModelIndex &index) {
    int32_t row = index.row();
    if(row > 0) {
      int32_t tmpRow = row - 1;
      auto it        = std::next(mClassesList.classes.begin(), tmpRow);
      openEditDialog(&*it, row);
    }
  });
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassificationList::openEditDialog(joda::settings::Class *classToModify, int32_t row)
{
  if(mClassSettingsDialog->exec(*classToModify) == 0) {
    QModelIndex indexToUpdt = mTableModelClasses->index(row, 0);
    mTableModelClasses->dataChanged(indexToUpdt, indexToUpdt);
    onSettingChanged();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassificationList::setDatabase(joda::db::Database *database)
{
  mDatabase = database;
  fromSettings();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassificationList::fromSettings()
{
  mTableModelClasses->beginChange();

  if(mDatabase == nullptr) {
    mClassesList.classes.clear();
    mTableModelClasses->endChange();
    return;
  }
  mClassesList.classes.clear();
  auto classes              = mDatabase->selectClasses();
  auto cols                 = mResultsSettings->getColumns();
  auto loadSettingsForClass = [&](const enums::ClassId classs) -> std::vector<settings::ResultsTemplate> {
    std::map<enums::Measurement, settings::ResultsTemplate> retValTmp;
    for(const auto &col : cols) {
      if(col.second.classId == classs) {
        auto &edit          = retValTmp[col.second.measureChannel];
        edit.measureChannel = col.second.measureChannel;
        edit.stats.emplace(col.second.stats);
      }
    }
    std::vector<settings::ResultsTemplate> retVal;
    retVal.reserve(retValTmp.size());
    for(const auto &[_, val] : retValTmp) {
      retVal.emplace_back(val);
    }

    return retVal;
  };

  for(const auto &[id, classs] : classes) {
    joda::settings::Class classTmp = classs;
    classTmp.defaultMeasurements   = loadSettingsForClass(id);
    mClassesList.classes.emplace_back(classTmp);
  }
  mTableModelClasses->endChange();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassificationList::onSettingChanged()
{
  *mResultsSettings = joda::settings::Settings::toResultsSettings(
      joda::settings::Settings::ResultSettingsInput{.classes             = {mClassesList.classes.begin(), mClassesList.classes.end()},
                                                    .outputClasses       = mDatabase->selectOutputClasses(),
                                                    .intersectingClasses = mDatabase->selectIntersectingClassForClasses(),
                                                    .measuredChannels    = mDatabase->selectMeasurementChannelsForClasses(),
                                                    .distanceFromClasses = mDatabase->selectDistanceClassForClasses()});
  emit settingsChanged();
}

}    // namespace joda::ui::gui
