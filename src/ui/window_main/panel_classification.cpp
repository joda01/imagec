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
#include <exception>
#include <string>
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/file_parser/directory_iterator.hpp"
#include "backend/settings/project_settings/project_class.hpp"
#include "backend/settings/project_settings/project_cluster_classes.hpp"
#include "backend/settings/project_settings/project_plates.hpp"
#include "ui/helper/icon_generator.hpp"
#include "ui/results/panel_results.hpp"
#include "ui/window_main/window_main.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui {

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
    auto *templateSelection = new QHBoxLayout();
    mTemplateSelection      = new QComboBox();
    templateSelection->addWidget(mTemplateSelection);

    auto *bookMark = new QPushButton(generateIcon("bookmark"), "");
    bookMark->setEnabled(false);
    bookMark->setToolTip("Run pipeline!");
    templateSelection->addWidget(bookMark);

    templateSelection->setStretch(0, 1);
    layout->addLayout(templateSelection);

    loadTemplates();
    connect(mTemplateSelection, &QComboBox::currentIndexChanged, this, &PanelClassification::onloadPreset);
  }

  {
    mClusters = new PlaceholderTableWidget(NR_OF_CLUSTERS, 5);
    mClusters->setPlaceholderText("Add a cluster");
    mClusters->verticalHeader()->setVisible(false);
    mClusters->setHorizontalHeaderLabels({"IdNr", "Id", "Cluster", "Color", "Notes"});
    mClusters->setAlternatingRowColors(true);
    mClusters->setSelectionBehavior(QAbstractItemView::SelectRows);
    mClusters->setColumnHidden(COL_ID, true);
    mClusters->setColumnHidden(COL_COLOR, true);
    mClusters->setColumnWidth(COL_ID_ENUM, 10);
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
    mClasses->setColumnHidden(COL_ID, true);
    mClasses->setColumnHidden(COL_COLOR, true);
    mClasses->setColumnWidth(COL_ID_ENUM, 10);
    mClasses->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    mClasses->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    mClasses->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    layout->addWidget(mClasses);
  }
  addSeparator();
  setLayout(layout);

  initTable();
  connect(mClasses, &QTableWidget::itemChanged, [&](QTableWidgetItem *item) { onSettingChanged(); });
  connect(mClusters, &QTableWidget::itemChanged, [&](QTableWidgetItem *item) { onSettingChanged(); });
  connect(mClusters, &QTableWidget::cellDoubleClicked, [&](int row, int column) {});
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
  mClasses->blockSignals(true);
  mClusters->blockSignals(true);
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
  mClasses->blockSignals(false);
  mClusters->blockSignals(false);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::fromSettings(const joda::settings::ClusterClasses &settings)
{
  mClasses->blockSignals(true);
  mClusters->blockSignals(true);

  initTable();

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

  mClasses->blockSignals(false);
  mClusters->blockSignals(false);
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
  mSettings.classification.clusters.clear();
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

    mSettings.classification.clusters.emplace_back(joda::settings::Cluster{
        .clusterId = clusterId, .name = clusterName.toStdString(), .color = clusterColor.toStdString(), .notes = clusterNotes.toStdString()});
  }

  //
  // Save classes
  //
  mSettings.classification.classes.clear();
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

    mSettings.classification.classes.emplace_back(joda::settings::Class{
        .classId = classId, .name = className.toStdString(), .color = classColor.toStdString(), .notes = classNotes.toStdString()});
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] auto PanelClassification::getClustersAndClasses() const
    -> std::tuple<std::map<enums::ClusterIdIn, QString>, std::map<enums::ClassIdIn, QString>>
{
  std::map<enums::ClusterIdIn, QString> clusters;
  std::map<enums::ClassIdIn, QString> classes;

  clusters.emplace(static_cast<enums::ClusterIdIn>(enums::ClusterIdIn::$), QString("Default"));
  clusters.emplace(static_cast<enums::ClusterIdIn>(enums::ClusterIdIn::NONE), QString("None"));
  clusters.emplace(static_cast<enums::ClusterIdIn>(enums::ClusterIdIn::UNDEFINED), QString("Undefined"));

  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::$), QString("Default"));
  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::NONE), QString("None"));
  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::UNDEFINED), QString("Undefined"));

  for(const auto &cluster : mSettings.classification.clusters) {
    clusters.emplace(static_cast<enums::ClusterIdIn>(cluster.clusterId), QString(cluster.name.data()));
  }

  for(const auto &classs : mSettings.classification.classes) {
    classes.emplace(static_cast<enums::ClassIdIn>(classs.classId), QString(classs.name.data()));
  }

  return {clusters, classes};
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
  mWindowMain->checkForSettingsChanged();
  emit settingsChanged();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::loadTemplates()
{
  auto foundTemplates = joda::templates::TemplateParser::findTemplates(
      {{"templates/classification", joda::templates::TemplateParser::Category::BASIC},
       {joda::templates::TemplateParser::getUsersTemplateDirectory().string(), joda::templates::TemplateParser::Category::USER}},
      joda::fs::EXT_CLUSTER_CLASS_TEMPLATE);

  mTemplateSelection->clear();
  mTemplateSelection->addItem("Load preset ...", "");
  mTemplateSelection->insertSeparator(mTemplateSelection->count());

  joda::templates::TemplateParser::Category actCategory = joda::templates::TemplateParser::Category::BASIC;
  for(const auto &[category, dataInCategory] : foundTemplates) {
    for(const auto &[_, data] : dataInCategory) {
      // Now the user templates start, add an addition separator
      if(category != actCategory) {
        actCategory = category;
        mTemplateSelection->insertSeparator(mTemplateSelection->count());
      }
      if(!data.icon.isNull()) {
        mTemplateSelection->addItem(QIcon(data.icon.scaled(28, 28)), data.title.data(), data.path.data());
      } else {
        mTemplateSelection->addItem(generateIcon("favorite"), data.title.data(), data.path.data());
      }
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::onloadPreset()
{
  auto selection = mTemplateSelection->currentData().toString();
  if(selection == "") {
  } else {
    try {
      joda::settings::ClusterClasses settings =
          joda::templates::TemplateParser::loadTemplate(std::filesystem::path(mTemplateSelection->currentData().toString().toStdString()));
      mWindowMain->mutableSettings().projectSettings.classification = settings;
      fromSettings(settings);
    } catch(const std::exception &ex) {
    }
  }
  mTemplateSelection->blockSignals(true);
  mTemplateSelection->setCurrentIndex(0);
  mTemplateSelection->blockSignals(false);
}

}    // namespace joda::ui
