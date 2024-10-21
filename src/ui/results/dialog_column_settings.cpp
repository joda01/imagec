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
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/database/plugins/filter.hpp"
#include "ui/container/setting/setting_combobox_multi_classification_unmanaged.hpp"
#include "ui/helper/icon_generator.hpp"

namespace joda::ui {

DialogColumnSettings::DialogColumnSettings(db::QueryFilter *filter, QWidget *parent) : mFilter(filter), QDialog(parent)
{
  setMinimumWidth(300);
  auto *vlayout = new QVBoxLayout();

  //
  mClusterClassSelector = new QComboBox();
  mClusterClassSelector->setMinimumWidth(150);
  connect(mClusterClassSelector, &QComboBox::currentIndexChanged, this, &DialogColumnSettings::onClusterAndClassesChanged);
  vlayout->addWidget(mClusterClassSelector);

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
  vlayout->addWidget(mMeasurementSelector);

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
  vlayout->addWidget(mStatsSelector);

  mCrossChannelStackC = new QComboBox();
  vlayout->addWidget(mCrossChannelStackC);

  vlayout->addStretch();

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
    mClusterClassSelector->blockSignals(true);

    auto colKey = mFilter->getColumn({.tabIdx = 0, .colIdx = selectedColumn});

    auto select = [](int idx, QComboBox *combo) {
      if(idx >= 0) {
        combo->setCurrentIndex(idx);
      }
    };

    select(mClusterClassSelector->findData(SettingComboBoxMultiClassificationUnmanaged::toInt(colKey.clusterClass)), mClusterClassSelector);
    onClusterAndClassesChanged();
    select(mMeasurementSelector->findData(static_cast<int32_t>(colKey.measureChannel)), mMeasurementSelector);
    select(mStatsSelector->findData(static_cast<int32_t>(colKey.stats)), mStatsSelector);
    select(mCrossChannelStackC->findData(colKey.crossChannelStacksC), mCrossChannelStackC);

    mClusterClassSelector->blockSignals(false);
  }

  accept          = false;
  mSelectedColumn = selectedColumn;
  QDialog::exec();
  if(accept) {
    auto [clusterName, className] = getClusterAndClassFromCombo();

    mFilter->addColumn(db::QueryFilter::ColumnIdx{.tabIdx = 0, .colIdx = selectedColumn},
                       db::QueryFilter::ColumnKey{
                           .clusterClass        = SettingComboBoxMultiClassificationUnmanaged::fromInt(mClusterClassSelector->currentData().toUInt()),
                           .measureChannel      = static_cast<enums::Measurement>(mMeasurementSelector->currentData().toInt()),
                           .stats               = static_cast<enums::Stats>(mStatsSelector->currentData().toInt()),
                           .crossChannelStacksC = mCrossChannelStackC->currentData().toInt(),
                           .zStack              = 0,
                           .tStack              = 0},
                       db::QueryFilter::ColumnName{.clusterName = clusterName, .className = className});
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogColumnSettings::onClusterAndClassesChanged()
{
  if(mDatabase == nullptr) {
    return;
  }
  auto clusterClassSelected = SettingComboBoxMultiClassificationUnmanaged::fromInt(mClusterClassSelector->currentData().toUInt());

  //
  // Select cross channel intensity
  //
  {
    auto imageChannels  = mDatabase->selectImageChannels();
    auto currentChannel = mCrossChannelStackC->currentData().toInt();
    auto channels       = mDatabase->selectMeasurementChannelsForClusterAndClass(static_cast<enums::ClusterId>(clusterClassSelected.clusterId),
                                                                                 static_cast<enums::ClassId>(clusterClassSelected.classId));
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
void DialogColumnSettings::updateClustersAndClasses(db::Database *database)
{
  if(database == nullptr) {
    return;
  }
  mDatabase = database;
  {
    // Clusters/Class
    mClusterClassSelector->blockSignals(true);
    auto clusters = mDatabase->selectClassesForClusters();
    mClusterClassSelector->clear();
    for(const auto &[clusterId, cluster] : clusters) {
      for(const auto &[classId, classsName] : cluster.second) {
        std::string name = cluster.first + "@" + classsName;
        mClusterClassSelector->addItem(name.data(), SettingComboBoxMultiClassificationUnmanaged::toInt({clusterId, classId}));
      }
      mClusterClassSelector->insertSeparator(mClusterClassSelector->count());
    }
    mClusterClassSelector->blockSignals(false);
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
auto DialogColumnSettings::getClusterAndClassFromCombo() const -> std::pair<std::string, std::string>
{
  QString clusterName;
  QString className;
  className = mClusterClassSelector->currentText();
  if(!className.isEmpty()) {
    auto splited = className.split("@");
    if(splited.size() > 1) {
      clusterName = splited[0];
      className   = splited[1];
    }
  }

  return {clusterName.toStdString(), className.toStdString()};
}

}    // namespace joda::ui
