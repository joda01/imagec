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
#include "ui/gui/helper/debugging.hpp"
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
  int32_t row         = 0;
  auto addMeasure     = [&](const QString &text, const QString &description, enums::Measurement meas, const std::set<enums::Stats> &stats,
                        const std::set<enums::Stats> &allowedStat, int32_t rowSpan = 1) {
    auto *menu = new QMenu();
    std::map<enums::Stats, std::pair<QAction *, bool>> statsMenu;
    auto addStats = [&](enums::Stats stat) {
      auto *submenu = menu->addAction(enums::toString(stat).data());
      submenu->setCheckable(true);
      if(stats.contains(stat)) {
        submenu->setChecked(true);
      }
      if(stat == enums::Stats::OFF) {
        CHECK_GUI_THREAD(submenu)
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
    toolButton->setMaximumWidth(250);
    toolButton->setMinimumWidth(250);
    toolButton->setDefaultAction(button);    // This binds text, icon, and triggered slot

    if(!description.isEmpty()) {
      auto *help = new QLabel(description);
      help->setMaximumWidth(300);
      help->setMinimumWidth(300);
      help->setWordWrap(true);
      measureLayout->addWidget(help, row, 0, rowSpan, 1);
    }
    measureLayout->addWidget(toolButton, row, 1, 1, 1);
    mMeasurements.emplace(meas, std::pair<QAction *, std::map<enums::Stats, std::pair<QAction *, bool>>>{button, statsMenu});
    row++;
  };

  auto addSeparator = [&]() {
    auto *separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    measureLayout->addWidget(separator, row, 0, 1, 2);
    row++;
  };

  addMeasure("Count", "Number of objects of this class:", enums::Measurement::COUNT, {enums::Stats::OFF}, {enums::Stats::OFF});
  addMeasure("Intersecting objects", "Number of objects intersecting with objects of this class determined by the <b>Reclassify</b> command.",
             enums::Measurement::INTERSECTING, {enums::Stats::AVG}, {enums::Stats::AVG, enums::Stats::SUM});

  addSeparator();
  addMeasure("Area size", "Area size of the objects of this class:", enums::Measurement::AREA_SIZE, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::SUM, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});
  addMeasure("Perimeter", "Perimeter size of the objects of this class:", enums::Measurement::PERIMETER, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::SUM, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});
  addMeasure("Circularity", "Circularity [0-1] size of the objects of this class:", enums::Measurement::CIRCULARITY, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::SUM, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});

  addSeparator();
  addMeasure("Intensity min",
             "The measured min/max/sum or avg intensity of the signals measured within an object. Measurement is always done in the unprocessed "
             "image using the image channel [CH0, CH1, ...]"
             "specified in the <b>Measure intensity</b> command.",
             enums::Measurement::INTENSITY_MIN, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::SUM, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV}, 4);
  addMeasure("Intensity max", "", enums::Measurement::INTENSITY_MAX, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::SUM, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});
  addMeasure("Intensity avg", "", enums::Measurement::INTENSITY_AVG, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::SUM, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});
  addMeasure("Intensity sum", "", enums::Measurement::INTENSITY_SUM, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::SUM, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});

  addSeparator();
  addMeasure("Position", "The [x,y] position of the object in the image.", enums::Measurement::CENTEROID_X, {enums::Stats::OFF}, {enums::Stats::OFF});

  addSeparator();
  addMeasure("Distance center to center",
             "The distance measured from the objects of this class to the objects specified in the <b>Measure distance</b> command. For distance "
             "measurement the centroid of both objects is used.",
             enums::Measurement::DISTANCE_CENTER_TO_CENTER, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV}, 5);

  addMeasure("Distance center to center min.", "", enums::Measurement::DISTANCE_CENTER_TO_SURFACE_MIN, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});
  addMeasure("Distance center to surface max.", "", enums::Measurement::DISTANCE_CENTER_TO_SURFACE_MAX, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});
  addMeasure("Distance surface to surface min.", "", enums::Measurement::DISTANCE_SURFACE_TO_SURFACE_MIN, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});
  addMeasure("Distance surface to surface max.", "", enums::Measurement::DISTANCE_SURFACE_TO_SURFACE_MAX, {enums::Stats::AVG},
             {enums::Stats::AVG, enums::Stats::MIN, enums::Stats::MAX, enums::Stats::MEDIAN, enums::Stats::STDDEV});

  /*addSeparator();
  addMeasure("Object ID", "A unique ID which identifies the object.", enums::Measurement::OBJECT_ID, {enums::Stats::OFF}, {enums::Stats::OFF});
  addMeasure("Parent object ID",
             "The object ID of the object with which this object is intersecting, as determined by the <b>Reclassify</b> command.",
             enums::Measurement::PARENT_OBJECT_ID, {enums::Stats::OFF}, {enums::Stats::OFF});
  addMeasure("Origin object ID", "If the <b>Reclassify Copy</b> option was used, this is the object ID of the object that was copied.",
             enums::Measurement::ORIGIN_OBJECT_ID, {enums::Stats::OFF}, {enums::Stats::OFF});
  addMeasure("Tracking ID",
             "The same tracking ID is assigned to all objects that are identified as the same instance by either the <b>Colocalization</b> or "
             "<b>Tracking</b> command.",
             enums::Measurement::TRACKING_ID, {enums::Stats::OFF}, {enums::Stats::OFF});*/

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
void DialogClassSettings::setEditable(bool editable)
{
  CHECK_GUI_THREAD(mDialogClassName)
  mDialogClassName->setEnabled(editable);
  CHECK_GUI_THREAD(mDialogColorCombo)
  mDialogColorCombo->setEnabled(editable);
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
  QDialog::exec();
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
    for(auto &[_1, stat] : settings.second) {
      stat.first->setChecked(false);
    }
  }

  for(const auto &measure : classs.defaultMeasurements) {
    if(mMeasurements.contains(measure.measureChannel)) {
      mMeasurements.at(measure.measureChannel).first->setChecked(true);
      for(const auto stat : measure.stats) {
        try {
          mMeasurements.at(measure.measureChannel).second.at(stat).first->setChecked(true);
        } catch(...) {
        }
      }
    }
  }

  //
  // Restore default settings for unchecked
  //
  for(auto &[_, settings] : mMeasurements) {
    if(!settings.first->isChecked()) {
      for(auto &[_1, setting] : settings.second) {
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
  std::set<enums::Measurement> stillAddedMeasurements;

  for(auto &[measure, settings] : mMeasurements) {
    if(settings.first->isChecked()) {
      std::set<enums::Stats> stats;
      for(auto &[stat, setting] : settings.second) {
        if(setting.first->isChecked()) {
          stats.emplace(stat);
        }
      }

      if(!stillAddedMeasurements.contains(measure)) {
        classs.defaultMeasurements.emplace_back(joda::settings::ResultsTemplate{.measureChannel = measure, .stats = stats});
        stillAddedMeasurements.emplace(measure);
      }

      if(measure == enums::Measurement::CENTEROID_X) {
        if(!stillAddedMeasurements.contains(enums::Measurement::CENTEROID_Y)) {
          classs.defaultMeasurements.emplace_back(joda::settings::ResultsTemplate{.measureChannel = enums::Measurement::CENTEROID_Y, .stats = stats});
          stillAddedMeasurements.emplace(enums::Measurement::CENTEROID_Y);
        }
      }
    }
  }
}

}    // namespace joda::ui::gui
