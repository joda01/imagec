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
    mClusters = new PlaceholderTableWidget(NR_OF_CLUSTERS, 4);
    mClusters->setPlaceholderText("Add a cluster");
    mClusters->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mClusters->verticalHeader()->setVisible(false);
    mClusters->setHorizontalHeaderLabels({"Id", "Name", "Color", "Notes"});
    mClusters->setAlternatingRowColors(true);
    mClusters->setSelectionBehavior(QAbstractItemView::SelectRows);
    // mClusters->setColumnHidden(0, true);
    layout->addWidget(mClusters);
  }

  {
    mClasses = new PlaceholderTableWidget(NR_OF_CLASSES, 4);
    mClasses->setPlaceholderText("Add a class");
    mClasses->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mClasses->verticalHeader()->setVisible(false);
    mClasses->setHorizontalHeaderLabels({"Id", "Name", "Color", "Notes"});
    mClasses->setAlternatingRowColors(true);
    mClasses->setSelectionBehavior(QAbstractItemView::SelectRows);
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
    mClusters->setItem(clusterId, 0, index);

    auto *item = new QTableWidgetItem(QString(""));
    mClusters->setItem(clusterId, 1, item);

    auto *itemColor = new QTableWidgetItem(QString(""));
    mClusters->setItem(clusterId, 2, itemColor);

    auto *itemNotes = new QTableWidgetItem(QString(""));
    mClusters->setItem(clusterId, 3, itemNotes);
  }

  //
  // Load classes
  //
  for(int32_t classId = 0; classId < NR_OF_CLASSES; classId++) {
    auto *index = new QTableWidgetItem(QString::number(classId));
    index->setFlags(index->flags() & ~Qt::ItemIsEditable);
    mClasses->setItem(classId, 0, index);

    auto *item = new QTableWidgetItem(QString(""));
    mClasses->setItem(classId, 1, item);

    auto *itemColor = new QTableWidgetItem(QString(""));
    mClasses->setItem(classId, 2, itemColor);

    auto *itemNotes = new QTableWidgetItem(QString(""));
    mClasses->setItem(classId, 3, itemNotes);
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
    mClusters->setItem(clusterId, 0, index);

    auto *item = new QTableWidgetItem(cluster.name.data());
    mClusters->setItem(clusterId, 1, item);

    auto *itemColor = new QTableWidgetItem(cluster.color.data());
    mClusters->setItem(clusterId, 2, itemColor);

    auto *itemNotes = new QTableWidgetItem(cluster.notes.data());
    mClusters->setItem(clusterId, 3, itemNotes);
  }

  //
  // Load classes
  //
  for(const auto &cluster : settings.classes) {
    auto clusterId = static_cast<int32_t>(cluster.classId);
    auto *index    = new QTableWidgetItem(QString::number(clusterId));
    index->setFlags(index->flags() & ~Qt::ItemIsEditable);
    mClasses->setItem(clusterId, 0, index);

    auto *item = new QTableWidgetItem(cluster.name.data());
    mClasses->setItem(clusterId, 1, item);

    auto *itemColor = new QTableWidgetItem(cluster.color.data());
    mClasses->setItem(clusterId, 2, itemColor);

    auto *itemNotes = new QTableWidgetItem(cluster.notes.data());
    mClasses->setItem(clusterId, 3, itemNotes);
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
    QTableWidgetItem *item = mClusters->item(row, 0);
    if(item == nullptr) {
      continue;
    }
    auto clusterId = static_cast<joda::enums::ClusterId>(item->text().toInt());

    QTableWidgetItem *itemName = mClusters->item(row, 1);
    if(itemName == nullptr || itemName->text().isEmpty()) {
      continue;
    }
    auto clusterName = itemName->text();

    QTableWidgetItem *itemColor = mClusters->item(row, 2);
    QString clusterColor;
    if(itemColor != nullptr && !itemColor->text().isEmpty()) {
      clusterColor = itemColor->text();
    }

    QTableWidgetItem *itemNotes = mClusters->item(row, 3);
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
    QTableWidgetItem *item = mClasses->item(row, 0);
    if(item == nullptr) {
      continue;
    }
    auto classId = static_cast<joda::enums::ClassId>(item->text().toInt());

    QTableWidgetItem *itemName = mClasses->item(row, 1);
    if(itemName == nullptr || itemName->text().isEmpty()) {
      continue;
    }
    auto className = itemName->text();

    QTableWidgetItem *itemColor = mClasses->item(row, 2);
    QString classColor;
    if(itemColor != nullptr && !itemColor->text().isEmpty()) {
      classColor = itemColor->text();
    }

    QTableWidgetItem *itemNotes = mClasses->item(row, 3);
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
}

}    // namespace joda::ui::qt
