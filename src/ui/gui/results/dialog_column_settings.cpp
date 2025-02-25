///
/// \file      dialog_column_settings.cpp
/// \author    Joachim Danmayr
/// \date      2024-10-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
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
#include "ui/gui/container/setting/setting_combobox_multi_classification_unmanaged.hpp"
#include "ui/gui/helper/icon_generator.hpp"

namespace joda::ui::gui {

DialogColumnSettings::DialogColumnSettings(db::QueryFilter *filter, QWidget *parent) : mFilter(filter), QDialog(parent)
{
  setMinimumWidth(300);
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
  mMeasurementSelector->addItem("Origin object ID", (int32_t) joda::enums::Measurement::ORIGIN_OBJECT_ID);
  mMeasurementSelector->addItem("Intensity sum.", (int32_t) joda::enums::Measurement::INTENSITY_SUM);
  mMeasurementSelector->addItem("Intensity avg.", (int32_t) joda::enums::Measurement::INTENSITY_AVG);
  mMeasurementSelector->addItem("Intensity min.", (int32_t) joda::enums::Measurement::INTENSITY_MIN);
  mMeasurementSelector->addItem("Intensity max.", (int32_t) joda::enums::Measurement::INTENSITY_MAX);
  vlayout->addRow("Measurement:", mMeasurementSelector);

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
  //
  mClasssIntersection = new QComboBox();
  mClasssIntersection->setMinimumWidth(150);
  // connect(mClasssIntersection, &QComboBox::currentIndexChanged, this, &DialogColumnSettings::onClassesChanged);
  vlayout->addRow("Intersecting class:", mClasssIntersection);

  //
  //
  mCrossChannelStackC = new QComboBox();
  vlayout->addRow("Channel intensity:", mCrossChannelStackC);

  mZStack = new QSpinBox();
  mZStack->setMinimum(0);
  mZStack->setValue(0);
  vlayout->addRow("Z-Stack:", mZStack);

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
    // auto *mDeleteButton = new QAction(generateIcon("delete"), "Delete", mToolbarBottom);
    // connect(mDeleteButton, &QAction::triggered, [this]() { close(); });
    // mToolbarBottom->addAction(mDeleteButton);

    //

    auto *okayBottom = new QAction(generateIcon("accept"), "Accept", mToolbarBottom);
    connect(okayBottom, &QAction::triggered, [this]() {
      if(mCrossChannelStackC->count() == 0 &&
         db::MeasureType::INTENSITY == db::getType(static_cast<enums::Measurement>(mMeasurementSelector->currentData().toInt()))) {
        return;
      }

      accept = true;
      close();
    });
    mToolbarBottom->addAction(okayBottom);

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

    select(mClasssClassSelector->findData(SettingComboBoxMultiClassificationUnmanaged::toInt(colKey.classs)), mClasssClassSelector);
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
    auto [classsName, className] = getClasssFromCombo();

    mFilter->addColumn(db::QueryFilter::ColumnIdx{.tabIdx = 0, .colIdx = selectedColumn},
                       db::QueryFilter::ColumnKey{
                           .classs              = SettingComboBoxMultiClassificationUnmanaged::fromInt(mClasssClassSelector->currentData().toUInt()),
                           .measureChannel      = static_cast<enums::Measurement>(mMeasurementSelector->currentData().toInt()),
                           .stats               = static_cast<enums::Stats>(mStatsSelector->currentData().toInt()),
                           .crossChannelStacksC = mCrossChannelStackC->currentData().toInt(),
                           .intersectingChannel = SettingComboBoxMultiClassificationUnmanaged::fromInt(mClasssIntersection->currentData().toUInt()),
                           .zStack              = mZStack->value(),
                           .tStack              = mTStack->value()},
                       db::QueryFilter::ColumnName{.className = className});
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
    auto clusters = mDatabase->selectClasses();
    mClasssClassSelector->clear();
    mClasssIntersection->clear();

    std::map<std::string, std::multimap<std::string, enums::ClassId>> orderedClasses;
    for(const auto &[classId, classsName] : clusters) {
      orderedClasses[enums::getPrefixFromClassName(classsName.name)].emplace(classsName.name, classId);
    }

    mClasssIntersection->addItem("-", 0xFFFD);

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

    mClasssClassSelector->blockSignals(false);
    mClasssIntersection->blockSignals(false);
  }
  {
    // Image channels
    mCrossChannelStackC->blockSignals(true);
    auto imageChannels = mDatabase->selectImageChannels();
    mCrossChannelStackC->clear();
    for(const auto &[channelId, channel] : imageChannels) {
      mCrossChannelStackC->addItem("CH" + QString::number(channelId) + " (" + QString(channel.name.data()) + ")", channelId);
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
  className = mClasssClassSelector->currentText();
  return {className.toStdString(), className.toStdString()};
}

}    // namespace joda::ui::gui
