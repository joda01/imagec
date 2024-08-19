///
/// \file      panel_image.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "panel_classification.hpp"
#include <qboxlayout.h>
#include <qlineedit.h>
#include <string>
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/file_parser/directory_iterator.hpp"
#include "backend/settings/project_settings/project_class.hpp"
#include "backend/settings/project_settings/project_plates.hpp"
#include "ui/results/panel_results.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui::qt {

PanelClassification::PanelClassification(joda::settings::ProjectSettings &settings, WindowMain *windowMain) :
    mWindowMain(windowMain), mSettings(settings)
{
  auto *layout = new QVBoxLayout();

  auto addSeparator = [&layout]() {
    QFrame *separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    layout->addWidget(separator);
  };

  {
    mClusters = new PlaceholderTableWidget(NR_OF_CLUSTERS, 5);
    mClusters->setPlaceholderText("Add a cluster");
    mClusters->verticalHeader()->setVisible(false);
    mClusters->setHorizontalHeaderLabels({"IdNr", "Id", "Cluster", "Color", "Notes"});
    mClusters->setAlternatingRowColors(true);
    mClusters->setSelectionBehavior(QAbstractItemView::SelectRows);
    mClusters->setColumnHidden(0, true);
    mClusters->setColumnWidth(1, 10);
    mClusters->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    mClusters->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    mClusters->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    layout->addWidget(mClusters);
  }

  {
    mClasses = new PlaceholderTableWidget(NR_OF_CLASSES, 5);
    mClasses->setPlaceholderText("Add a class");
    mClasses->verticalHeader()->setVisible(false);
    mClasses->setHorizontalHeaderLabels({"IdNr", "Id", "Class", "Color", "Notes"});
    mClasses->setAlternatingRowColors(true);
    mClasses->setSelectionBehavior(QAbstractItemView::SelectRows);
    mClasses->setColumnHidden(0, true);
    mClasses->setColumnWidth(1, 10);
    mClasses->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    mClasses->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    mClasses->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    layout->addWidget(mClasses);
  }
  addSeparator();
  setLayout(layout);

  initTable();
  connect(mClasses, &QTableWidget::itemChanged, [&](QTableWidgetItem *item) { onSettingChanged(); });
  connect(mClusters, &QTableWidget::cellDoubleClicked, [&](int row, int column) {});
  connect(mClusters, &QTableWidget::itemChanged, [&](QTableWidgetItem *item) { onSettingChanged(); });
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::initTable()
{
  //
  // Load clusters
  //
  for(int32_t clusterId = 0; clusterId < NR_OF_CLUSTERS; clusterId++) {
    auto *index = new QTableWidgetItem(QString::number(clusterId));
    index->setFlags(index->flags() & ~Qt::ItemIsEditable);
    mClusters->setItem(clusterId, COL_ID, index);

    nlohmann::json classIdStr = static_cast<enums::ClusterId>(clusterId);
    auto *itemEnum            = new QTableWidgetItem(QString(std::string(classIdStr).data()));
    mClusters->setItem(clusterId, COL_ID_ENUM, itemEnum);

    auto *item = new QTableWidgetItem(QString(""));
    mClusters->setItem(clusterId, COL_NAME, item);

    auto *itemColor = new QTableWidgetItem(QString(""));
    mClusters->setItem(clusterId, COL_COLOR, itemColor);

    auto *itemNotes = new QTableWidgetItem(QString(""));
    mClusters->setItem(clusterId, COL_NOTES, itemNotes);
  }

  //
  // Load classes
  //
  for(int32_t classId = 0; classId < NR_OF_CLASSES; classId++) {
    auto *index = new QTableWidgetItem(QString::number(classId));
    index->setFlags(index->flags() & ~Qt::ItemIsEditable);
    mClasses->setItem(classId, COL_ID, index);

    nlohmann::json classIdStr = static_cast<enums::ClassId>(classId);
    auto *itemEnum            = new QTableWidgetItem(QString(std::string(classIdStr).data()));
    mClasses->setItem(classId, COL_ID_ENUM, itemEnum);

    auto *item = new QTableWidgetItem(QString(""));
    mClasses->setItem(classId, COL_NAME, item);

    auto *itemColor = new QTableWidgetItem(QString(""));
    mClasses->setItem(classId, COL_COLOR, itemColor);

    auto *itemNotes = new QTableWidgetItem(QString(""));
    mClasses->setItem(classId, COL_NOTES, itemNotes);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::fromSettings(const joda::settings::ProjectSettings &settings)
{
  //
  // Load clusters
  //
  for(const auto &cluster : settings.clusters) {
    auto clusterId = static_cast<int32_t>(cluster.clusterId);
    auto *index    = new QTableWidgetItem(QString::number(clusterId));
    index->setFlags(index->flags() & ~Qt::ItemIsEditable);
    mClusters->setItem(clusterId, COL_ID, index);

    nlohmann::json classIdStr = cluster.clusterId;
    auto *itemEnum            = new QTableWidgetItem(QString(std::string(classIdStr).data()));
    mClusters->setItem(clusterId, COL_ID_ENUM, itemEnum);

    auto *item = new QTableWidgetItem(cluster.name.data());
    mClusters->setItem(clusterId, COL_NAME, item);

    auto *itemColor = new QTableWidgetItem(cluster.color.data());
    mClusters->setItem(clusterId, COL_COLOR, itemColor);

    auto *itemNotes = new QTableWidgetItem(cluster.notes.data());
    mClusters->setItem(clusterId, COL_NOTES, itemNotes);
  }

  //
  // Load classes
  //
  for(const auto &classs : settings.classes) {
    auto classId = static_cast<int32_t>(classs.classId);
    auto *index  = new QTableWidgetItem(QString::number(classId));
    index->setFlags(index->flags() & ~Qt::ItemIsEditable);
    mClasses->setItem(classId, COL_ID, index);

    nlohmann::json classIdStr = classs.classId;
    auto *itemEnum            = new QTableWidgetItem(QString(std::string(classIdStr).data()));
    mClasses->setItem(classId, COL_ID_ENUM, itemEnum);

    auto *item = new QTableWidgetItem(classs.name.data());
    mClasses->setItem(classId, COL_NAME, item);

    auto *itemColor = new QTableWidgetItem(classs.color.data());
    mClasses->setItem(classId, COL_COLOR, itemColor);

    auto *itemNotes = new QTableWidgetItem(classs.notes.data());
    mClasses->setItem(classId, COL_NOTES, itemNotes);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::toSettings()
{
  //
  // Save clusters
  //
  mSettings.clusters.clear();
  for(int row = 0; row < mClusters->rowCount(); row++) {
    QTableWidgetItem *item = mClusters->item(row, COL_ID);
    if(item == nullptr) {
      continue;
    }
    auto clusterId = static_cast<joda::enums::ClusterId>(item->text().toInt());

    QTableWidgetItem *itemName = mClusters->item(row, COL_NAME);
    if(itemName == nullptr || itemName->text().isEmpty()) {
      continue;
    }
    auto clusterName = itemName->text();

    QTableWidgetItem *itemColor = mClusters->item(row, COL_COLOR);
    QString clusterColor;
    if(itemColor != nullptr && !itemColor->text().isEmpty()) {
      clusterColor = itemColor->text();
    }

    QTableWidgetItem *itemNotes = mClusters->item(row, COL_NOTES);
    QString clusterNotes;
    if(itemNotes != nullptr && !itemNotes->text().isEmpty()) {
      clusterNotes = itemNotes->text();
    }

    mSettings.clusters.emplace_back(joda::settings::Cluster{.clusterId = clusterId,
                                                            .name      = clusterName.toStdString(),
                                                            .color     = clusterColor.toStdString(),
                                                            .notes     = clusterNotes.toStdString()});
  }

  //
  // Save classes
  //
  mSettings.classes.clear();
  for(int row = 0; row < mClasses->rowCount(); row++) {
    QTableWidgetItem *item = mClasses->item(row, COL_ID);
    if(item == nullptr) {
      continue;
    }
    auto classId = static_cast<joda::enums::ClassId>(item->text().toInt());

    QTableWidgetItem *itemName = mClasses->item(row, COL_NAME);
    if(itemName == nullptr || itemName->text().isEmpty()) {
      continue;
    }
    auto className = itemName->text();

    QTableWidgetItem *itemColor = mClasses->item(row, COL_COLOR);
    QString classColor;
    if(itemColor != nullptr && !itemColor->text().isEmpty()) {
      classColor = itemColor->text();
    }

    QTableWidgetItem *itemNotes = mClasses->item(row, COL_NOTES);
    QString classNotes;
    if(itemNotes != nullptr && !itemNotes->text().isEmpty()) {
      classNotes = itemNotes->text();
    }

    mSettings.classes.emplace_back(joda::settings::Class{.classId = classId,
                                                         .name    = className.toStdString(),
                                                         .color   = classColor.toStdString(),
                                                         .notes   = classNotes.toStdString()});
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::onSettingChanged()
{
  toSettings();
  emit settingsChanged();
}

}    // namespace joda::ui::qt
