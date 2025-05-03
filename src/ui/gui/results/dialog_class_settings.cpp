///
/// \file      dialog_class_settings.cpp
/// \author    Joachim Danmayr
/// \date      2025-04-30
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "dialog_class_settings.hpp"
#include <qboxlayout.h>
#include <qcheckbox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qmenu.h>
#include <qtoolbutton.h>
#include <qwidgetaction.h>
#include "backend/enums/enum_measurements.hpp"
#include "backend/settings/project_settings/project_class.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/showed_menu.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
DialogClassSettings::DialogClassSettings(QWidget *parent) : QDialog(parent)
{
  setWindowTitle("Class editor");
  auto *layout     = new QVBoxLayout();
  mDialogClassName = new QComboBox();
  mDialogClassName->setEditable(true);
  mDialogClassName->setPlaceholderText("e.g. cy5@spot");
  mDialogClassName->lineEdit()->setPlaceholderText("Class name, e.g. cy5@spot");
  //
  // Predefined selections
  //
  mDialogClassName->addItem("cy3@spot", "cy3@spot");
  mDialogClassName->addItem("cy3@spot-in-cell", "cy3@spot-in-cell");
  mDialogClassName->addItem("cy3@background", "cy3@background");
  mDialogClassName->addItem("cy5@spot", "cy5@spot");
  mDialogClassName->addItem("cy5@spot-in-cell", "cy5@spot-in-cell");
  mDialogClassName->addItem("cy5@background", "cy5@background");
  mDialogClassName->addItem("cy7@spot", "cy7@spot");
  mDialogClassName->addItem("cy7@spot-in-cell", "cy7@spot-in-cell");
  mDialogClassName->addItem("cy7@background", "cy7@background");
  mDialogClassName->addItem("gfp@spot", "gfp@spot");
  mDialogClassName->addItem("gfp@background", "gfp@background");
  mDialogClassName->addItem("fitc@spot", "fitc@spot");
  mDialogClassName->addItem("fitc@background", "fitc@background");
  mDialogClassName->insertSeparator(mDialogClassName->count());
  mDialogClassName->addItem("tetraspeck@spot", "tetraspeck@spot");
  mDialogClassName->addItem("tetraspeck@background", "tetraspeck@background");
  mDialogClassName->insertSeparator(mDialogClassName->count());
  mDialogClassName->addItem("dapi@nucleus", "dapi@nucleus");
  mDialogClassName->addItem("dapi@background", "dapi@background");
  mDialogClassName->insertSeparator(mDialogClassName->count());
  mDialogClassName->addItem("brightfield@cell-area", "brightfield@cell-area");
  mDialogClassName->addItem("brightfield@cell", "brightfield@cell");
  mDialogClassName->addItem("brightfield@background", "brightfield@background");
  mDialogClassName->insertSeparator(mDialogClassName->count());
  mDialogClassName->addItem("coloc@spots", "coloc@spots");
  mDialogClassName->addItem("coloc@cy3cy7", "coloc@cy5cy7");
  mDialogClassName->addItem("coloc@cy5cy7", "coloc@cy5cy7");
  mDialogClassName->addItem("coloc@cy3cy5", "coloc@cy3cy5");
  mDialogClassName->addItem("coloc@cy3", "coloc@cy3");
  mDialogClassName->addItem("coloc@cy5", "coloc@cy5");
  mDialogClassName->addItem("coloc@cy7", "coloc@cy7");
  mDialogClassName->addItem("coloc@fitc", "coloc@fitc");
  mDialogClassName->addItem("coloc@gfp", "coloc@gfp");

  mDialogColorCombo = new ColorComboBox();
  auto *model       = qobject_cast<QStandardItemModel *>(mDialogColorCombo->model());

  // Set the custom delegate
  for(const auto &color : settings::COLORS) {
    QString colorStr = color.data();
    auto *item       = new QStandardItem(colorStr);
    item->setBackground(QColor(colorStr));                  // Set the background color
    item->setData(QColor(colorStr), Qt::BackgroundRole);    // Store background color for use in the delegate
    model->appendRow(item);
  }

  // Create buttons
  auto *okButton = new QPushButton("OK", this);
  okButton->setDefault(true);
  connect(okButton, &QPushButton::pressed, this, &DialogClassSettings::onOkayPressed);
  auto *cancelButton = new QPushButton("Cancel", this);
  connect(cancelButton, &QPushButton::pressed, [&]() {
    mOkayPressed = -1;
    close();
  });

  //
  // Grid Layout for the measurements
  //
  auto *measureLayout = new QGridLayout();
  auto addMeasure     = [&](const QString &text, int32_t row, int32_t col, enums::Measurement meas, const std::set<enums::Stats> &stats,
                        const std::set<enums::Stats> &allowedStat) {
    auto *menu = new QMenu();
    std::map<enums::Stats, std::pair<QAction *, bool>> statsMenu;
    auto addStats = [&](enums::Stats stat) {
      auto *submenu = menu->addAction(enums::toString(stat).data());
      submenu->setCheckable(true);
      if(stats.contains(stat)) {
        submenu->setChecked(true);
      }
      if(stat == enums::Stats::OFF) {
        submenu->setVisible(false);
      }
      // Store menu and default setting
      statsMenu.emplace(stat, std::pair<QAction *, bool>{submenu, stats.contains(stat)});
    };
    for(const auto stat : allowedStat) {
      addStats(stat);
    }

    auto *button = new QAction(text);
    if(menu->actions().size() > 1) {
      button->setMenu(menu);
    } else if(menu->actions().size() == 1) {
      menu->actions().at(0)->setChecked(true);
    }
    button->setCheckable(true);

    // Wrap it in a QToolButton
    auto *toolButton = new QToolButton();
    toolButton->setMaximumWidth(150);
    toolButton->setMinimumWidth(150);
    toolButton->setDefaultAction(button);    // This binds text, icon, and triggered slot

    measureLayout->addWidget(toolButton, row, col);
    mMeasurements.emplace(meas, std::pair<QAction *, std::map<enums::Stats, std::pair<QAction *, bool>>>{button, statsMenu});
  };

  auto addSeparator = [&](int32_t row) {
    auto *separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    measureLayout->addWidget(separator, row, 0, 1, 2);
  };

  addMeasure("Count", 0, 0, enums::Measurement::COUNT, {enums::Stats::OFF}, {enums::Stats::OFF});
  addMeasure("Nr. of intersecting objects", 0, 1, enums::Measurement::INTERSECTING, {enums::Stats::OFF}, {enums::Stats::OFF});

  addSeparator(1);
  addMeasure("Area size", 2, 0, enums::Measurement::AREA_SIZE, {enums::Stats::AVG, enums::Stats::SUM},
             {enums::Stats::AVG, enums::Stats::SUM, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});
  addMeasure("Perimeter", 2, 1, enums::Measurement::PERIMETER, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::SUM, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});
  addMeasure("Circularity", 3, 0, enums::Measurement::CIRCULARITY, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::SUM, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});

  addSeparator(4);
  addMeasure("Intensity min", 5, 0, enums::Measurement::INTENSITY_MIN, {enums::Stats::AVG, enums::Stats::SUM},
             {enums::Stats::AVG, enums::Stats::SUM, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});
  addMeasure("Intensity max", 5, 1, enums::Measurement::INTENSITY_MAX, {enums::Stats::AVG, enums::Stats::SUM},
             {enums::Stats::AVG, enums::Stats::SUM, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});
  addMeasure("Intensity avg", 6, 0, enums::Measurement::INTENSITY_AVG, {enums::Stats::AVG, enums::Stats::SUM},
             {enums::Stats::AVG, enums::Stats::SUM, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});
  addMeasure("Intensity sum", 6, 1, enums::Measurement::INTENSITY_SUM, {enums::Stats::AVG, enums::Stats::SUM},
             {enums::Stats::AVG, enums::Stats::SUM, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});

  addSeparator(7);
  addMeasure("Position", 8, 0, enums::Measurement::CENTEROID_X, {enums::Stats::OFF}, {enums::Stats::OFF});
  addMeasure("Distance to surface min", 8, 1, enums::Measurement::DISTANCE_CENTER_TO_CENTER, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});
  addMeasure("Distance to surface max", 9, 0, enums::Measurement::DISTANCE_CENTER_TO_SURFACE_MIN, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});
  addMeasure("Distance to center min", 9, 1, enums::Measurement::DISTANCE_CENTER_TO_SURFACE_MAX, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});
  addMeasure("Distance to center max", 10, 0, enums::Measurement::DISTANCE_SURFACE_TO_SURFACE_MIN, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});
  addMeasure("Distance to center max", 10, 1, enums::Measurement::DISTANCE_SURFACE_TO_SURFACE_MAX, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});

  addSeparator(11);
  addMeasure("Object ID", 12, 0, enums::Measurement::OBJECT_ID, {enums::Stats::OFF}, {enums::Stats::OFF});
  addMeasure("Parent object ID", 12, 1, enums::Measurement::PARENT_OBJECT_ID, {enums::Stats::OFF}, {enums::Stats::OFF});
  addMeasure("Origin object ID", 13, 0, enums::Measurement::ORIGIN_OBJECT_ID, {enums::Stats::OFF}, {enums::Stats::OFF});
  addMeasure("Tracking ID", 13, 1, enums::Measurement::TRACKING_ID, {enums::Stats::OFF}, {enums::Stats::OFF});

  //
  // Create a horizontal layout for the buttons
  //
  auto *buttonLayout = new QHBoxLayout;
  buttonLayout->addStretch();
  buttonLayout->addWidget(cancelButton);
  buttonLayout->addWidget(okButton);

  auto addLayoutSeparator = [&]() {
    auto *separator = new QFrame;
    separator->setFixedHeight(15);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    layout->addWidget(separator);
  };
  //
  // Add to final layout
  //
  layout->addWidget(mDialogClassName);
  layout->addWidget(mDialogColorCombo);
  addLayoutSeparator();
  layout->addWidget(new QLabel("<b>Measurements:</b>"));
  layout->addLayout(measureLayout);
  addLayoutSeparator();
  layout->addLayout(buttonLayout);

  setLayout(layout);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogClassSettings::onOkayPressed()
{
  mOkayPressed = 0;
  close();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
int DialogClassSettings::exec(joda::settings::Class &classs)
{
  mOkayPressed = -1;
  fromSettings(classs);
  int ret = QDialog::exec();
  if(mOkayPressed == 0) {
    toSettings(classs);
  }

  return mOkayPressed;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DialogClassSettings::fromSettings(const joda::settings::Class &classs)
{
  mDialogClassName->setCurrentText(classs.name.data());
  auto colorIdx = mDialogColorCombo->findData(QColor(classs.color.data()), Qt::BackgroundRole);
  if(colorIdx >= 0) {
    mDialogColorCombo->setCurrentIndex(colorIdx);
  }

  for(auto &[_, settings] : mMeasurements) {
    settings.first->setChecked(false);
    for(auto &[_, stat] : settings.second) {
      stat.first->setChecked(false);
    }
  }

  for(const auto &measure : classs.defaultMeasurements) {
    if(mMeasurements.contains(measure.measureChannel)) {
      mMeasurements.at(measure.measureChannel).first->setChecked(true);
      for(const auto stat : measure.stats) {
        mMeasurements.at(measure.measureChannel).second.at(stat).first->setChecked(true);
      }
    }
  }

  //
  // Restore default settings for unchecked
  //
  for(auto &[_, settings] : mMeasurements) {
    if(!settings.first->isChecked()) {
      for(auto &[_, setting] : settings.second) {
        setting.first->setChecked(setting.second);
      }
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DialogClassSettings::toSettings(joda::settings::Class &classs)
{
  classs.name  = mDialogClassName->currentText().toStdString();
  classs.color = mDialogColorCombo->currentText().toStdString();
  classs.defaultMeasurements.clear();

  for(auto &[measure, settings] : mMeasurements) {
    if(settings.first->isChecked()) {
      std::set<enums::Stats> stats;
      for(auto &[stat, setting] : settings.second) {
        if(setting.first->isChecked()) {
          stats.emplace(stat);
        }
      }

      classs.defaultMeasurements.emplace_back(joda::settings::ResultsTemplate{.measureChannel = measure, .stats = stats});
      if(measure == enums::Measurement::CENTEROID_X) {
        classs.defaultMeasurements.emplace_back(joda::settings::ResultsTemplate{.measureChannel = enums::Measurement::CENTEROID_Y, .stats = stats});
      }
    }
  }
}

}    // namespace joda::ui::gui
