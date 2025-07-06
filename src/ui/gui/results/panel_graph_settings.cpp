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

#include "panel_graph_settings.hpp"
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
#include "backend/database/data/heatmap/data_heatmap.hpp"
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/file_parser/directory_iterator.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/settings.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/window_main/window_main.hpp"

namespace joda::ui::gui {

using namespace joda::db::data;

PanelGraphSettings::PanelGraphSettings(WindowMain *windowMain) : mWindowMain(windowMain)
{
  setWindowTitle("Graph settings");
  setFeatures(features() & ~QDockWidget::DockWidgetClosable);
  auto *centralWidget = new QWidget();
  centralWidget->setContentsMargins(0, 0, 0, 0);
  auto *layout = new QVBoxLayout();
  layout->setContentsMargins(0, 2, 0, 0);
  centralWidget->setLayout(layout);
  setMaximumWidth(250);
  setMinimumWidth(250);

  {
    auto *toolbar = new QToolBar();

    layout->addWidget(toolbar);
  }

  {
    auto *formLayout = new QFormLayout;

    //
    // Column selector
    //
    mColumn = new QComboBox();
    connect(mColumn, &QComboBox::currentIndexChanged, [this]() { emit settingsChanged(); });
    formLayout->addRow("Column:", mColumn);

    //
    // Well order matrix
    //
    mWellOrderMatrix = new QLineEdit("[[1,2,3,4],[5,6,7,8],[9,10,11,12],[13,14,15,16]]");
    formLayout->addRow("Well order matrix:", mWellOrderMatrix);
    connect(mWellOrderMatrix, &QLineEdit::editingFinished, [this]() { emit settingsChanged(); });

    //
    // Plate size
    //
    mPlateSize = new QComboBox();
    mPlateSize->addItem("1", 1);
    mPlateSize->addItem("2 x 3", 203);
    mPlateSize->addItem("3 x 4", 304);
    mPlateSize->addItem("4 x 6", 406);
    mPlateSize->addItem("6 x 8", 608);
    mPlateSize->addItem("8 x 12", 812);
    mPlateSize->addItem("16 x 24", 1624);
    mPlateSize->addItem("32 x 48", 3248);
    mPlateSize->addItem("48 x 72", 4872);
    formLayout->addRow("Plate size:", mPlateSize);
    connect(mPlateSize, &QComboBox::currentIndexChanged, [this](int32_t index) { emit settingsChanged(); });

    //
    // Density map
    //
    mDensityMapSize = new QComboBox();
    mDensityMapSize->addItem("50", 50);
    mDensityMapSize->addItem(std::to_string(4096 / 64).data(), 4096 / 64);
    mDensityMapSize->addItem("100", 100);
    mDensityMapSize->addItem(std::to_string(4096 / 32).data(), 4096 / 32);
    mDensityMapSize->addItem("150", 150);
    mDensityMapSize->addItem("200", 200);
    mDensityMapSize->addItem("250", 250);
    mDensityMapSize->addItem(std::to_string(4096 / 16).data(), 4096 / 16);
    mDensityMapSize->addItem("300", 300);
    mDensityMapSize->addItem("350", 350);
    mDensityMapSize->addItem("400", 400);
    mDensityMapSize->addItem("450", 450);
    mDensityMapSize->addItem("450", 450);
    mDensityMapSize->addItem(std::to_string(4096 / 8).data(), 4096 / 8);
    mDensityMapSize->addItem("1000", 1000);
    mDensityMapSize->addItem(std::to_string(4096 / 4).data(), 4096 / 4);
    mDensityMapSize->addItem("2000", 2000);
    mDensityMapSize->addItem(std::to_string(4096 / 2).data(), 4096 / 2);
    mDensityMapSize->addItem("3000", 3000);
    mDensityMapSize->addItem("4000", 4000);
    mDensityMapSize->addItem(std::to_string(4096).data(), 4096);
    mDensityMapSize->setCurrentIndex(mDensityMapSize->count() - 1);
    connect(mDensityMapSize, &QComboBox::currentIndexChanged, [this](int32_t index) { emit settingsChanged(); });
    formLayout->addRow("Density map size:", mDensityMapSize);

    //
    // Color map
    //
    static const std::vector<std::pair<QString, joda::db::data::ColormapName>> items = {
        {"Accent", joda::db::data::ColormapName::ACCENT},
        {"Blues", joda::db::data::ColormapName::BLUES},
        {"BrBG", joda::db::data::ColormapName::BRBG},
        {"BuGn", joda::db::data::ColormapName::BUGN},
        {"BuPu", joda::db::data::ColormapName::BUPU},
        {"ChromaJS", joda::db::data::ColormapName::CHROMAJS},
        {"Dark2", joda::db::data::ColormapName::DARK2},
        {"GnBu", joda::db::data::ColormapName::GNBU},
        {"GnPu", joda::db::data::ColormapName::GNPU},
        {"Greens", joda::db::data::ColormapName::GREENS},
        {"Greys", joda::db::data::ColormapName::GREYS},
        {"Inferno", joda::db::data::ColormapName::INFERNO},
        {"Jet", joda::db::data::ColormapName::JET},
        {"Turbo", joda::db::data::ColormapName::TURBO},
        {"Magma", joda::db::data::ColormapName::MAGMA},
        {"Oranges", joda::db::data::ColormapName::ORANGES},
        {"OrRd", joda::db::data::ColormapName::ORRD},
        {"Paired", joda::db::data::ColormapName::PAIRED},
        {"Parula", joda::db::data::ColormapName::PARULA},
        {"Pastel1", joda::db::data::ColormapName::PASTEL1},
        {"Pastel2", joda::db::data::ColormapName::PASTEL2},
        {"PiYG", joda::db::data::ColormapName::PIYG},
        {"Plasma", joda::db::data::ColormapName::PLASMA},
        {"PRGn", joda::db::data::ColormapName::PRGN},
        {"PuBu", joda::db::data::ColormapName::PUBU},
        {"PuBuGn", joda::db::data::ColormapName::PUBUGN},
        {"PuOr", joda::db::data::ColormapName::PUOR},
        {"PuRd", joda::db::data::ColormapName::PURD},
        {"Purples", joda::db::data::ColormapName::PURPLES},
        {"RdBu", joda::db::data::ColormapName::RDBU},
        {"BuRd", joda::db::data::ColormapName::BURD},
        {"RdGy", joda::db::data::ColormapName::RDGY},
        {"RdPu", joda::db::data::ColormapName::RDPU},
        {"RdYlBu", joda::db::data::ColormapName::RDYLBU},
        {"RdYlGn", joda::db::data::ColormapName::RDYLGN},
        {"Reds", joda::db::data::ColormapName::REDS},
        {"Sand", joda::db::data::ColormapName::SAND},
        {"Set1", joda::db::data::ColormapName::SET1},
        {"Set2", joda::db::data::ColormapName::SET2},
        {"Set3", joda::db::data::ColormapName::SET3},
        {"Spectral", joda::db::data::ColormapName::SPECTRAL},
        {"Viridis", joda::db::data::ColormapName::VIRIDIS},
        {"WhYlRd", joda::db::data::ColormapName::WHYLRD},
        {"YlGn", joda::db::data::ColormapName::YLGN},
        {"YlGnBu", joda::db::data::ColormapName::YLGNBU},
        {"YlOrBr", joda::db::data::ColormapName::YLORBR},
        {"YlOrRd", joda::db::data::ColormapName::YLORRD},
        {"YlRd", joda::db::data::ColormapName::YLRD},
        {"HSV", joda::db::data::ColormapName::HSV},
        {"Hot", joda::db::data::ColormapName::HOT},
        {"Cool", joda::db::data::ColormapName::COOL},
        {"Spring", joda::db::data::ColormapName::SPRING},
        {"Summer", joda::db::data::ColormapName::SUMMER},
        {"Autumn", joda::db::data::ColormapName::AUTUMN},
        {"Winter", joda::db::data::ColormapName::WINTER},
        {"Gray", joda::db::data::ColormapName::GRAY},
        {"Bone", joda::db::data::ColormapName::BONE},
        {"Copper", joda::db::data::ColormapName::COPPER},
        {"Pink", joda::db::data::ColormapName::PINK},
        {"Lines", joda::db::data::ColormapName::LINES},
        {"Colorcube", joda::db::data::ColormapName::COLORCUBE},
        {"Prism", joda::db::data::ColormapName::PRISM},
        {"Flag", joda::db::data::ColormapName::FLAG},
        {"White", joda::db::data::ColormapName::WHITE},
        {"Default Map", joda::db::data::ColormapName::DEFAULT_MAP},
        {"Default Colors Map", joda::db::data::ColormapName::DEFAULT_COLORS_MAP}};

    mColorMaps = new QComboBox();
    for(const auto &[label, value] : items) {
      mColorMaps->addItem(label, static_cast<int>(value));
    }
    mColorMaps->setCurrentIndex(30);
    connect(mColorMaps, &QComboBox::currentIndexChanged, [this](int32_t index) { emit settingsChanged(); });
    formLayout->addRow("Colormap:", mColorMaps);
    formLayout->setContentsMargins(2, 0, 0, 0);
    layout->addLayout(formLayout);
    layout->addStretch();
  }

  setWidget(centralWidget);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
joda::db::data::ColormapName PanelGraphSettings::getSelectedColorMap() const
{
  return static_cast<joda::db::data::ColormapName>(mColorMaps->currentData().toInt());
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] auto PanelGraphSettings::getWellOrder() const -> std::vector<std::vector<int32_t>>
{
  return joda::settings::stringToVector(mWellOrderMatrix->text().toStdString());
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] auto PanelGraphSettings::getPlateSize() const -> QSize
{
  auto value = mPlateSize->currentData().toUInt();
  QSize size;
  size.setWidth(value % 100);
  size.setHeight(value / 100);
  return size;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] auto PanelGraphSettings::getDensityMapSize() const -> uint32_t
{
  return mDensityMapSize->currentData().toUInt();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PanelGraphSettings::setColumns(const std::map<joda::settings::ResultsSettings::ColumnIdx, joda::settings::ResultsSettings::ColumnKey> &columns)
{
  //
  // Update heatmap column selector
  //
  mColumn->blockSignals(true);
  auto actData = mColumn->currentData();
  mColumn->clear();
  for(const auto &[key, value] : columns) {
    QString headerText = value.createHeader().data();
    mColumn->addItem(headerText, key.colIdx);
  }
  auto idx = mColumn->findData(actData);
  if(idx >= 0) {
    mColumn->setCurrentIndex(idx);
  }
  mColumn->blockSignals(false);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PanelGraphSettings::fromSettings(const std::vector<std::vector<int32_t>> &wellOrder, const QSize &plateSize, uint32_t densityMapSize)
{
  {
    mWellOrderMatrix->blockSignals(true);
    mWellOrderMatrix->setText(joda::settings::vectorToString(wellOrder).data());
    mWellOrderMatrix->blockSignals(false);
  }
  {
    mPlateSize->blockSignals(true);
    uint32_t plateSizeCoded = (plateSize.height() * 100) + plateSize.width();
    auto idx                = mPlateSize->findData(plateSizeCoded);
    if(idx >= 0) {
      mPlateSize->setCurrentIndex(idx);
    }
    mPlateSize->blockSignals(false);
  }
  {
    mDensityMapSize->blockSignals(true);
    auto idx = mDensityMapSize->findData(densityMapSize);
    if(idx >= 0) {
      mDensityMapSize->setCurrentIndex(idx);
    }
    mDensityMapSize->blockSignals(false);
  }
}

}    // namespace joda::ui::gui
