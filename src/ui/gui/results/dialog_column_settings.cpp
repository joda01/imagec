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

#include "dialog_column_settings.hpp"
#include <qaction.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qspinbox.h>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/database/plugins/filter.hpp"
#include "backend/settings/analze_settings.hpp"
#include "ui/gui/container/setting/setting_combobox_multi_classification_unmanaged.hpp"
#include "ui/gui/helper/icon_generator.hpp"

namespace joda::ui::gui {

DialogColumnSettings::DialogColumnSettings(settings::ResultsSettings *filter, QWidget *parent) : mFilter(filter), QDialog(parent)
{
  setWindowTitle("Add column");
  setMinimumWidth(500);
  auto *vlayout = new QFormLayout();

  //
  mClasssClassSelector = new QComboBox();
  mClasssClassSelector->setMinimumWidth(150);
  connect(mClasssClassSelector, &QComboBox::currentIndexChanged, this, &DialogColumnSettings::onClassesChanged);
  vlayout->addRow("Classs:", mClasssClassSelector);

  //
  //
  mMeasurementSelector = new QComboBox();
  mMeasurementSelector->addItem("Count", (int32_t) joda::enums::Measurement::COUNT);
  mMeasurementSelector->addItem("Confidence", (int32_t) joda::enums::Measurement::CONFIDENCE);
  mMeasurementSelector->addItem("Area size", (int32_t) joda::enums::Measurement::AREA_SIZE);
  mMeasurementSelector->addItem("Perimeter", (int32_t) joda::enums::Measurement::PERIMETER);
  mMeasurementSelector->addItem("Circularity", (int32_t) joda::enums::Measurement::CIRCULARITY);
  mMeasurementSelector->insertSeparator(mMeasurementSelector->count());
  mMeasurementSelector->addItem("Center of mass X", (int32_t) joda::enums::Measurement::CENTER_OF_MASS_X);
  mMeasurementSelector->addItem("Center of mass Y", (int32_t) joda::enums::Measurement::CENTER_OF_MASS_Y);
  mMeasurementSelector->insertSeparator(mMeasurementSelector->count());
  mMeasurementSelector->addItem("Intensity sum.", (int32_t) joda::enums::Measurement::INTENSITY_SUM);
  mMeasurementSelector->addItem("Intensity avg.", (int32_t) joda::enums::Measurement::INTENSITY_AVG);
  mMeasurementSelector->addItem("Intensity min.", (int32_t) joda::enums::Measurement::INTENSITY_MIN);
  mMeasurementSelector->addItem("Intensity max.", (int32_t) joda::enums::Measurement::INTENSITY_MAX);
  mMeasurementSelector->insertSeparator(mMeasurementSelector->count());
  mMeasurementSelector->addItem("Intersection", (int32_t) joda::enums::Measurement::INTERSECTING);
  mMeasurementSelector->insertSeparator(mMeasurementSelector->count());
  mMeasurementSelector->addItem("Object ID", (int32_t) joda::enums::Measurement::OBJECT_ID);
  mMeasurementSelector->addItem("Origin object ID", (int32_t) joda::enums::Measurement::ORIGIN_OBJECT_ID);
  mMeasurementSelector->addItem("Parent object ID", (int32_t) joda::enums::Measurement::PARENT_OBJECT_ID);
  vlayout->addRow("Measurement:", mMeasurementSelector);

  //
  //
  //
  mClasssIntersection = new QComboBox();
  mClasssIntersection->setMinimumWidth(150);
  // connect(mClasssIntersection, &QComboBox::currentIndexChanged, this, &DialogColumnSettings::onClassesChanged);
  vlayout->addRow("Intersection class:", mClasssIntersection);

  //
  //
  mCrossChannelStackC = new QComboBox();
  vlayout->addRow("Channel intensity:", mCrossChannelStackC);

  //
  //
  mStatsSelector = new QComboBox();
  mStatsSelector->addItem("AVG", (int32_t) joda::enums::Stats::AVG);
  mStatsSelector->addItem("MEDIAN", (int32_t) joda::enums::Stats::MEDIAN);
  mStatsSelector->addItem("MIN", (int32_t) joda::enums::Stats::MIN);
  mStatsSelector->addItem("MAX", (int32_t) joda::enums::Stats::MAX);
  mStatsSelector->addItem("STDDEV", (int32_t) joda::enums::Stats::STDDEV);
  mStatsSelector->addItem("SUM", (int32_t) joda::enums::Stats::SUM);
  mStatsSelector->addItem("CNT", (int32_t) joda::enums::Stats::CNT);
  vlayout->addRow("Statistics:", mStatsSelector);

  //
  //
  mZStack = new QSpinBox();
  mZStack->setMinimum(0);
  mZStack->setValue(0);
  vlayout->addRow("Z-Stack:", mZStack);

  //
  //
  mTStack = new QSpinBox();
  mTStack->setMinimum(0);
  mTStack->setValue(0);
  vlayout->addRow("T-Stack:", mTStack);

  {
    auto *mToolbarBottom = new QToolBar();
    mToolbarBottom->setContentsMargins(0, 0, 0, 0);
    auto *spacerBottom = new QWidget();
    spacerBottom->setContentsMargins(0, 0, 0, 0);
    spacerBottom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    auto *mSpaceBottomToolbar = mToolbarBottom->addWidget(spacerBottom);

    //
    // auto *mDeleteButton = new QAction(generateSvgIcon("edit-delete"), "Delete", mToolbarBottom);
    // connect(mDeleteButton, &QAction::triggered, [this]() { close(); });
    // mToolbarBottom->addAction(mDeleteButton);

    //

    auto *okayBottom = new QAction(generateSvgIcon("dialog-ok-apply"), "Accept", mToolbarBottom);
    connect(okayBottom, &QAction::triggered, [this]() {
      if(mCrossChannelStackC->count() == 0 &&
         settings::ResultsSettings::MeasureType::INTENSITY ==
             settings::ResultsSettings::getType(static_cast<enums::Measurement>(mMeasurementSelector->currentData().toInt()))) {
        return;
      }

      accept = true;
      close();
    });
    mToolbarBottom->addAction(okayBottom);

    vlayout->addWidget(mToolbarBottom);
  }
  checkForIntersecting();
  connect(mMeasurementSelector, &QComboBox::currentIndexChanged, [&]() { checkForIntersecting(); });
  setLayout(vlayout);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogColumnSettings::checkForIntersecting()
{
  if(mMeasurementSelector->currentData().toInt() != (int32_t) joda::enums::Measurement::INTERSECTING) {
    mClasssIntersection->setEnabled(false);
  } else {
    mClasssIntersection->setEnabled(true);
  }

  if(mMeasurementSelector->currentData().toInt() != (int32_t) joda::enums::Measurement::INTENSITY_SUM &&
     mMeasurementSelector->currentData().toInt() != (int32_t) joda::enums::Measurement::INTENSITY_AVG &&
     mMeasurementSelector->currentData().toInt() != (int32_t) joda::enums::Measurement::INTENSITY_MIN &&
     mMeasurementSelector->currentData().toInt() != (int32_t) joda::enums::Measurement::INTENSITY_MAX) {
    mCrossChannelStackC->setEnabled(false);
  } else {
    mCrossChannelStackC->setEnabled(true);
  }

  if(mMeasurementSelector->currentData().toInt() != (int32_t) joda::enums::Measurement::OBJECT_ID &&
     mMeasurementSelector->currentData().toInt() != (int32_t) joda::enums::Measurement::ORIGIN_OBJECT_ID &&
     mMeasurementSelector->currentData().toInt() != (int32_t) joda::enums::Measurement::PARENT_OBJECT_ID) {
    mStatsSelector->setEnabled(true);
  } else {
    mStatsSelector->setEnabled(false);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogColumnSettings::exec(int32_t selectedColumn)
{
  if(mFilter->containsColumn({.tabIdx = 0, .colIdx = selectedColumn})) {
    mClasssClassSelector->blockSignals(true);

    auto colKey = mFilter->getColumn({.tabIdx = 0, .colIdx = selectedColumn});

    auto select = [](int idx, QComboBox *combo) {
      if(idx >= 0) {
        combo->setCurrentIndex(idx);
      }
    };

    select(mClasssClassSelector->findData(SettingComboBoxMultiClassificationUnmanaged::toInt(colKey.classId)), mClasssClassSelector);
    select(mClasssIntersection->findData(SettingComboBoxMultiClassificationUnmanaged::toInt(colKey.intersectingChannel)), mClasssIntersection);
    onClassesChanged();
    select(mMeasurementSelector->findData(static_cast<int32_t>(colKey.measureChannel)), mMeasurementSelector);
    select(mStatsSelector->findData(static_cast<int32_t>(colKey.stats)), mStatsSelector);
    select(mCrossChannelStackC->findData(colKey.crossChannelStacksC), mCrossChannelStackC);

    mZStack->setValue(colKey.zStack);
    mTStack->setValue(colKey.tStack);

    mClasssClassSelector->blockSignals(false);
  }

  accept          = false;
  mSelectedColumn = selectedColumn;
  QDialog::exec();

  //
  // Create filter
  //
  if(accept) {
    auto [className, intersectingName] = getClasssFromCombo();

    mFilter->addColumn(settings::ResultsSettings::ColumnIdx{.tabIdx = 0, .colIdx = selectedColumn},
                       settings::ResultsSettings::ColumnKey{
                           .classId             = SettingComboBoxMultiClassificationUnmanaged::fromInt(mClasssClassSelector->currentData().toUInt()),
                           .measureChannel      = static_cast<enums::Measurement>(mMeasurementSelector->currentData().toInt()),
                           .stats               = static_cast<enums::Stats>(mStatsSelector->currentData().toInt()),
                           .crossChannelStacksC = mCrossChannelStackC->currentData().toInt(),
                           .intersectingChannel = SettingComboBoxMultiClassificationUnmanaged::fromInt(mClasssIntersection->currentData().toUInt()),
                           .zStack              = mZStack->value(),
                           .tStack              = mTStack->value()},
                       settings::ResultsSettings::ColumnName{.className = className, .intersectingName = intersectingName});
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogColumnSettings::onClassesChanged()
{
  if(mDatabase == nullptr) {
    return;
  }
  auto classsSelected = SettingComboBoxMultiClassificationUnmanaged::fromInt(mClasssClassSelector->currentData().toUInt());

  //
  // Select cross channel intensity
  //
  {
    auto imageChannels  = mDatabase->selectImageChannels();
    auto currentChannel = mCrossChannelStackC->currentData().toInt();
    auto channels       = mDatabase->selectMeasurementChannelsForClasss(static_cast<enums::ClassId>(classsSelected));
    mCrossChannelStackC->blockSignals(true);
    mCrossChannelStackC->clear();
    for(const auto channelId : channels) {
      mCrossChannelStackC->addItem("CH" + QString::number(channelId) + " (" + QString(imageChannels.at(channelId).name.data()) + ")", channelId);
    }
    auto idx = mCrossChannelStackC->findData(currentChannel);
    if(idx >= 0) {
      mCrossChannelStackC->setCurrentIndex(idx);
    }
    mCrossChannelStackC->blockSignals(false);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogColumnSettings::updateClassesAndClasses(db::Database *database)
{
  if(database == nullptr) {
    return;
  }
  mDatabase = database;
  {
    // Clusters/Class
    mClasssClassSelector->blockSignals(true);
    mClasssIntersection->blockSignals(true);
    auto clusters          = mDatabase->selectClasses();
    auto selectedClass     = SettingComboBoxMultiClassificationUnmanaged::fromInt(mClasssClassSelector->currentData().toInt());
    auto selectedIntersect = SettingComboBoxMultiClassificationUnmanaged::fromInt(mClasssIntersection->currentData().toInt());
    mClasssClassSelector->clear();
    mClasssIntersection->clear();

    std::map<std::string, std::multimap<std::string, enums::ClassId>> orderedClasses;
    for(const auto &[classId, classsName] : clusters) {
      orderedClasses[enums::getPrefixFromClassName(classsName.name)].emplace(classsName.name, classId);
    }

    for(const auto &[prefix, group] : orderedClasses) {
      for(const auto &[className, id] : group) {
        QVariant variant;
        mClasssClassSelector->addItem(className.data(), SettingComboBoxMultiClassificationUnmanaged::toInt(id));
        mClasssIntersection->addItem(className.data(), SettingComboBoxMultiClassificationUnmanaged::toInt(id));
      }
      mClasssClassSelector->insertSeparator(mClasssClassSelector->count());
      mClasssIntersection->insertSeparator(mClasssClassSelector->count());
    }
    auto removeLastSeparator = [this]() {
      int lastIndex          = mClasssClassSelector->count() - 1;
      int lastIndexIntersect = mClasssIntersection->count() - 1;
      if(lastIndex >= 0) {
        mClasssClassSelector->removeItem(lastIndex);
        mClasssIntersection->removeItem(lastIndexIntersect);
      }
    };
    removeLastSeparator();

    auto foundIdx = mClasssClassSelector->findData(SettingComboBoxMultiClassificationUnmanaged::toInt(selectedClass));
    if(foundIdx >= 0) {
      mClasssClassSelector->setCurrentIndex(foundIdx);
    }

    foundIdx = mClasssIntersection->findData(SettingComboBoxMultiClassificationUnmanaged::toInt(selectedIntersect));
    if(foundIdx >= 0) {
      mClasssIntersection->setCurrentIndex(foundIdx);
    }

    mClasssClassSelector->blockSignals(false);
    mClasssIntersection->blockSignals(false);
  }
  {
    // Image channels
    mCrossChannelStackC->blockSignals(true);
    auto selectedCh    = mCrossChannelStackC->currentData().toInt();
    auto imageChannels = mDatabase->selectImageChannels();
    mCrossChannelStackC->clear();
    for(const auto &[channelId, channel] : imageChannels) {
      mCrossChannelStackC->addItem("CH" + QString::number(channelId) + " (" + QString(channel.name.data()) + ")", channelId);
    }
    auto foundIdx = mCrossChannelStackC->findData(selectedCh);
    if(foundIdx >= 0) {
      mCrossChannelStackC->setCurrentIndex(foundIdx);
    }
    mCrossChannelStackC->blockSignals(false);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogColumnSettings::updateClassesAndClasses(const joda::settings::AnalyzeSettings &settings)
{
  {
    // Clusters/Class
    mClasssClassSelector->blockSignals(true);
    mClasssIntersection->blockSignals(true);
    auto clusters          = settings.projectSettings.classification.classes;
    auto selectedClass     = SettingComboBoxMultiClassificationUnmanaged::fromInt(mClasssClassSelector->currentData().toInt());
    auto selectedIntersect = SettingComboBoxMultiClassificationUnmanaged::fromInt(mClasssIntersection->currentData().toInt());
    mClasssClassSelector->clear();
    mClasssIntersection->clear();

    std::map<std::string, std::multimap<std::string, enums::ClassId>> orderedClasses;
    for(const auto &classs : clusters) {
      orderedClasses[enums::getPrefixFromClassName(classs.name)].emplace(classs.name, classs.classId);
    }

    for(const auto &[prefix, group] : orderedClasses) {
      for(const auto &[className, id] : group) {
        QVariant variant;
        mClasssClassSelector->addItem(className.data(), SettingComboBoxMultiClassificationUnmanaged::toInt(id));
        mClasssIntersection->addItem(className.data(), SettingComboBoxMultiClassificationUnmanaged::toInt(id));
      }
      mClasssClassSelector->insertSeparator(mClasssClassSelector->count());
      mClasssIntersection->insertSeparator(mClasssClassSelector->count());
    }
    auto removeLastSeparator = [this]() {
      int lastIndex          = mClasssClassSelector->count() - 1;
      int lastIndexIntersect = mClasssIntersection->count() - 1;
      if(lastIndex >= 0) {
        mClasssClassSelector->removeItem(lastIndex);
        mClasssIntersection->removeItem(lastIndexIntersect);
      }
    };
    removeLastSeparator();

    auto foundIdx = mClasssClassSelector->findData(SettingComboBoxMultiClassificationUnmanaged::toInt(selectedClass));
    if(foundIdx >= 0) {
      mClasssClassSelector->setCurrentIndex(foundIdx);
    }

    foundIdx = mClasssIntersection->findData(SettingComboBoxMultiClassificationUnmanaged::toInt(selectedIntersect));
    if(foundIdx >= 0) {
      mClasssIntersection->setCurrentIndex(foundIdx);
    }

    mClasssClassSelector->blockSignals(false);
    mClasssIntersection->blockSignals(false);
  }
  {
    // Image channels
    mCrossChannelStackC->blockSignals(true);
    auto selectedCh = mCrossChannelStackC->currentData().toInt();

    auto imageChannels = std::map<int32_t, std::string>{{0, ""}, {1, ""}, {2, ""}, {3, ""}, {4, ""}, {5, ""}, {6, ""}, {7, ""}, {8, ""}, {9, ""}};
    mCrossChannelStackC->clear();
    for(const auto &[channelId, channelName] : imageChannels) {
      mCrossChannelStackC->addItem("CH" + QString::number(channelId) + " (" + QString(channelName.data()) + ")", channelId);
    }
    auto foundIdx = mCrossChannelStackC->findData(selectedCh);
    if(foundIdx >= 0) {
      mCrossChannelStackC->setCurrentIndex(foundIdx);
    }

    mCrossChannelStackC->blockSignals(false);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto DialogColumnSettings::getClasssFromCombo() const -> std::pair<std::string, std::string>
{
  QString className;
  QString intersectingName;
  className        = mClasssClassSelector->currentText();
  intersectingName = mClasssIntersection->currentText();
  return {className.toStdString(), intersectingName.toStdString()};
}

}    // namespace joda::ui::gui
