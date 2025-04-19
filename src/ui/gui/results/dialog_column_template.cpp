///
/// \file      dialog_column_settings.cpp
/// \author    Joachim Danmayr
/// \date      2024-10-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "dialog_column_template.hpp"
#include <qaction.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qspinbox.h>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/database/plugins/filter.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/results_settings/results_template.hpp"
#include "ui/gui/container/setting/setting_combobox_multi_classification_unmanaged.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/template_parser/template_parser.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::gui {

DialogColumnTemplate::DialogColumnTemplate(settings::ResultsTemplate *filter, QWidget *parent) : mTemplateSettings(filter), QDialog(parent)
{
  setWindowTitle("Table column wizard");
  setMinimumWidth(500);
  auto *vlayout = new QFormLayout();

  auto addSeparator = [&vlayout]() {
    auto *separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    vlayout->addRow(separator);
  };

  auto addColumn = [this, &vlayout]() {
    auto &col = mColumns.emplace_back(Column{});
    //
    //
    col.mMeasurementSelector = new QComboBoxMulti();
    col.mMeasurementSelector->addItem("Count", (int32_t) joda::enums::Measurement::COUNT);
    col.mMeasurementSelector->addItem("Confidence", (int32_t) joda::enums::Measurement::CONFIDENCE);
    col.mMeasurementSelector->addItem("Area size", (int32_t) joda::enums::Measurement::AREA_SIZE);
    col.mMeasurementSelector->addItem("Perimeter", (int32_t) joda::enums::Measurement::PERIMETER);
    col.mMeasurementSelector->addItem("Circularity", (int32_t) joda::enums::Measurement::CIRCULARITY);
    col.mMeasurementSelector->insertSeparator(col.mMeasurementSelector->count());
    col.mMeasurementSelector->addItem("Center of mass X", (int32_t) joda::enums::Measurement::CENTER_OF_MASS_X);
    col.mMeasurementSelector->addItem("Center of mass Y", (int32_t) joda::enums::Measurement::CENTER_OF_MASS_Y);
    col.mMeasurementSelector->insertSeparator(col.mMeasurementSelector->count());
    col.mMeasurementSelector->addItem("Intensity sum.", (int32_t) joda::enums::Measurement::INTENSITY_SUM);
    col.mMeasurementSelector->addItem("Intensity avg.", (int32_t) joda::enums::Measurement::INTENSITY_AVG);
    col.mMeasurementSelector->addItem("Intensity min.", (int32_t) joda::enums::Measurement::INTENSITY_MIN);
    col.mMeasurementSelector->addItem("Intensity max.", (int32_t) joda::enums::Measurement::INTENSITY_MAX);
    col.mMeasurementSelector->insertSeparator(col.mMeasurementSelector->count());
    col.mMeasurementSelector->addItem("Intersection", (int32_t) joda::enums::Measurement::INTERSECTING);
    col.mMeasurementSelector->insertSeparator(col.mMeasurementSelector->count());
    col.mMeasurementSelector->addItem("Object ID", (int32_t) joda::enums::Measurement::OBJECT_ID);
    col.mMeasurementSelector->addItem("Origin object ID", (int32_t) joda::enums::Measurement::ORIGIN_OBJECT_ID);
    col.mMeasurementSelector->addItem("Parent object ID", (int32_t) joda::enums::Measurement::PARENT_OBJECT_ID);
    col.mMeasurementSelector->addItem("Tracking ID", (int32_t) joda::enums::Measurement::TRACKING_ID);
    vlayout->addRow("Measurement:", col.mMeasurementSelector);

    //
    //
    col.mStatsSelector = new QComboBoxMulti();
    col.mStatsSelector->addItem("AVG", (int32_t) joda::enums::Stats::AVG);
    col.mStatsSelector->addItem("MEDIAN", (int32_t) joda::enums::Stats::MEDIAN);
    col.mStatsSelector->addItem("MIN", (int32_t) joda::enums::Stats::MIN);
    col.mStatsSelector->addItem("MAX", (int32_t) joda::enums::Stats::MAX);
    col.mStatsSelector->addItem("STDDEV", (int32_t) joda::enums::Stats::STDDEV);
    col.mStatsSelector->addItem("SUM", (int32_t) joda::enums::Stats::SUM);
    col.mStatsSelector->addItem("CNT", (int32_t) joda::enums::Stats::CNT);
    vlayout->addRow("Statistics:", col.mStatsSelector);
  };

  addColumn();
  addSeparator();
  addColumn();
  addSeparator();
  addColumn();

  {
    auto *mToolbarBottom = new QToolBar();
    mToolbarBottom->setContentsMargins(0, 0, 0, 0);
    auto *spacerBottom = new QWidget();
    spacerBottom->setContentsMargins(0, 0, 0, 0);
    spacerBottom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    auto *mSpaceBottomToolbar = mToolbarBottom->addWidget(spacerBottom);

    auto *saveTemplate = new QAction(generateSvgIcon("document-save-as-template"), "Save as template", mToolbarBottom);
    connect(saveTemplate, &QAction::triggered, [this]() {
      QString templatePath           = joda::templates::TemplateParser::getUsersTemplateDirectory().string().data();
      QString filePathOfSettingsFile = QFileDialog::getSaveFileName(
          this, "Save File", templatePath, "ImageC table template file (*" + QString(fs::EXT_RESULTS_TABLE_TEMPLATE.data()) + ")");
      std::string filename = filePathOfSettingsFile.toStdString();
      if(!filename.empty()) {
        try {
          nlohmann::json json = toTemplateSettings();
          joda::templates::TemplateParser::saveTemplate(json, std::filesystem::path(filename), fs::EXT_RESULTS_TABLE_TEMPLATE);
          accept = false;
          close();
        } catch(...) {
        }
      }
    });
    mToolbarBottom->addAction(saveTemplate);

    mToolbarBottom->addSeparator();

    auto *okayBottom = new QAction(generateSvgIcon("dialog-ok-apply"), "Accept", mToolbarBottom);
    connect(okayBottom, &QAction::triggered, [this]() {
      accept = true;
      close();
    });
    mToolbarBottom->addAction(okayBottom);

    auto *cancelButton = new QAction(generateSvgIcon("window-close"), "Close", mToolbarBottom);
    connect(cancelButton, &QAction::triggered, [this]() {
      accept = false;
      close();
    });
    mToolbarBottom->addAction(cancelButton);

    vlayout->addWidget(mToolbarBottom);
  }
  setLayout(vlayout);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int DialogColumnTemplate::exec()
{
  int n = 0;
  for(const auto &col : mTemplateSettings->columns) {
    QVariantList listMeasure;
    QVariantList listStats;
    if(n < mColumns.size()) {
      for(const auto &ch : col.measureChannels) {
        listMeasure.emplace_back((int32_t) ch);
      }
      mColumns.at(n).mMeasurementSelector->setCheckedItems(listMeasure);

      for(const auto &ch : col.stats) {
        listStats.emplace_back((int32_t) ch);
      }
      mColumns.at(n).mStatsSelector->setCheckedItems(listStats);
      n++;
    }
  }

  accept = false;
  QDialog::exec();

  //
  // Create filter
  //
  if(accept) {
    mTemplateSettings->columns.clear();
    *mTemplateSettings = toTemplateSettings();
    return 0;
  }
  return 1;
}

auto DialogColumnTemplate::toTemplateSettings() const -> settings::ResultsTemplate
{
  settings::ResultsTemplate templateSettings;
  templateSettings.columns.clear();
  for(const auto &col : mColumns) {
    auto &colSet = templateSettings.columns.emplace_back(settings::ResultsTemplate::TemplateEntry{});
    for(const auto &checked : col.mMeasurementSelector->getCheckedItems()) {
      colSet.measureChannels.emplace_back(static_cast<joda::enums::Measurement>(checked.first.toInt()));
    }
    for(const auto &checked : col.mStatsSelector->getCheckedItems()) {
      colSet.stats.emplace_back(static_cast<joda::enums::Stats>(checked.first.toInt()));
    }
  }
  return templateSettings;
}

}    // namespace joda::ui::gui
