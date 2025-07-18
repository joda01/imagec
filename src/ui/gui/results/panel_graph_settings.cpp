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
#include "backend/plot/heatmap/plot_heatmap.hpp"
#include "backend/plot/plot_base.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/settings.hpp"
#include "ui/gui/helper/icon_generator.hpp"

namespace joda::ui::gui {

using namespace joda::db::data;

PanelGraphSettings::PanelGraphSettings()
{
  setWindowTitle("Graph settings");
  setFeatures(features() & ~QDockWidget::DockWidgetClosable);
  auto *centralWidget = new QWidget();
  centralWidget->setContentsMargins(0, 0, 0, 0);
  auto *layout = new QVBoxLayout();
  layout->setContentsMargins(0, 2, 0, 0);
  centralWidget->setLayout(layout);
  setMaximumWidth(350);
  setMinimumWidth(350);

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
    static const std::vector<std::pair<QString, joda::plot::ColormapName>> items = {
        // {"Accent", joda::plot::ColormapName::ACCENT},
        // {"Blues", joda::plot::ColormapName::BLUES},
        // {"BrBG", joda::plot::ColormapName::BRBG},
        // {"BuGn", joda::plot::ColormapName::BUGN},
        // {"BuPu", joda::plot::ColormapName::BUPU},
        // {"ChromaJS", joda::plot::ColormapName::CHROMAJS},
        // {"Dark2", joda::plot::ColormapName::DARK2},
        // {"GnBu", joda::plot::ColormapName::GNBU},
        // {"GnPu", joda::plot::ColormapName::GNPU},
        // {"Greens", joda::plot::ColormapName::GREENS},
        // {"Greys", joda::plot::ColormapName::GREYS},
        {"Inferno", joda::plot::ColormapName::INFERNO},    //
        {"Jet", joda::plot::ColormapName::JET},            //
        {"Turbo", joda::plot::ColormapName::TURBO},        //
        {"Magma", joda::plot::ColormapName::MAGMA},        //
        //{"Oranges", joda::plot::ColormapName::ORANGES},
        //{"OrRd", joda::plot::ColormapName::ORRD},
        //{"Paired", joda::plot::ColormapName::PAIRED},
        {"Parula", joda::plot::ColormapName::PARULA},
        //{"Pastel1", joda::plot::ColormapName::PASTEL1},
        //{"Pastel2", joda::plot::ColormapName::PASTEL2},
        //{"PiYG", joda::plot::ColormapName::PIYG},
        {"Plasma", joda::plot::ColormapName::PLASMA},
        //{"PRGn", joda::plot::ColormapName::PRGN},
        //{"PuBu", joda::plot::ColormapName::PUBU},
        //{"PuBuGn", joda::plot::ColormapName::PUBUGN},
        //{"PuOr", joda::plot::ColormapName::PUOR},
        //{"PuRd", joda::plot::ColormapName::PURD},
        //{"Purples", joda::plot::ColormapName::PURPLES},
        //{"RdBu", joda::plot::ColormapName::RDBU},
        //{"BuRd", joda::plot::ColormapName::BURD},
        //{"RdGy", joda::plot::ColormapName::RDGY},
        //{"RdPu", joda::plot::ColormapName::RDPU},
        //{"RdYlBu", joda::plot::ColormapName::RDYLBU},
        //{"RdYlGn", joda::plot::ColormapName::RDYLGN},
        //{"Reds", joda::plot::ColormapName::REDS},
        //{"Sand", joda::plot::ColormapName::SAND},
        //{"Set1", joda::plot::ColormapName::SET1},
        //{"Set2", joda::plot::ColormapName::SET2},
        //{"Set3", joda::plot::ColormapName::SET3},
        //{"Spectral", joda::plot::ColormapName::SPECTRAL},
        {"Viridis", joda::plot::ColormapName::VIRIDIS},
        //{"WhYlRd", joda::plot::ColormapName::WHYLRD},
        //{"YlGn", joda::plot::ColormapName::YLGN},
        //{"YlGnBu", joda::plot::ColormapName::YLGNBU},
        //{"YlOrBr", joda::plot::ColormapName::YLORBR},
        //{"YlOrRd", joda::plot::ColormapName::YLORRD},
        //{"YlRd", joda::plot::ColormapName::YLRD},
        {"HSV", joda::plot::ColormapName::HSV},          //
        {"Hot", joda::plot::ColormapName::HOT},          //
        {"Cool", joda::plot::ColormapName::COOL},        //
        {"Spring", joda::plot::ColormapName::SPRING},    //
        {"Summer", joda::plot::ColormapName::SUMMER},    //
        {"Autumn", joda::plot::ColormapName::AUTUMN},    //
        {"Winter", joda::plot::ColormapName::WINTER},    //
        //{"Gray", joda::plot::ColormapName::GRAY},
        {"Bone", joda::plot::ColormapName::BONE},
        //{"Copper", joda::plot::ColormapName::COPPER},
        {"Pink", joda::plot::ColormapName::PINK},
        //{"Lines", joda::plot::ColormapName::LINES},
        //{"Colorcube", joda::plot::ColormapName::COLORCUBE},
        //{"Prism", joda::plot::ColormapName::PRISM},
        //{"Flag", joda::plot::ColormapName::FLAG},
        //{"White", joda::plot::ColormapName::WHITE},
        {"Rainbow", joda::plot::ColormapName::RAINBOW},                                 //
        {"Ocean", joda::plot::ColormapName::OCEAN},                                     //
        {"Cividis", joda::plot::ColormapName::COLORMAP_CIVIDIS},                        //
        {"Twighlight", joda::plot::ColormapName::COLORMAP_TWILIGHT},                    //
        {"Twighlight shifted", joda::plot::ColormapName::COLORMAP_TWILIGHT_SHIFTED},    //
        {"Deepgreen", joda::plot::ColormapName::COLORMAP_DEEPGREEN},                    //
                                                                                        //{"Default Map", joda::plot::ColormapName::DEFAULT_MAP},
        //{"Default Colors Map", joda::plot::ColormapName::DEFAULT_COLORS_MAP}
    };

    mColorMaps = new QComboBox();
    for(const auto &[label, value] : items) {
      mColorMaps->addItem(label, static_cast<int>(value));
      if(value == joda::plot::ColormapName::COLORMAP_TWILIGHT) {
        mColorMaps->setCurrentIndex(mColorMaps->count() - 1);
      }
    }
    connect(mColorMaps, &QComboBox::currentIndexChanged, [this](int32_t index) { emit settingsChanged(); });
    formLayout->addRow("Colormap:", mColorMaps);

    //
    // Color map range settings
    //
    mColormapRangeSettings = new QComboBox();
    mColormapRangeSettings->addItem("Automatic", static_cast<int>(joda::plot::ColorMappingMode::AUTO));
    mColormapRangeSettings->addItem("Manual", static_cast<int>(joda::plot::ColorMappingMode::MANUAL));
    connect(mColormapRangeSettings, &QComboBox::currentIndexChanged, [this](int32_t index) {
      if(getColorMapRangeSetting() == joda::plot::ColorMappingMode::AUTO) {
        mColorMapMinValue->setEnabled(false);
        mColorMapMaxValue->setEnabled(false);
      } else {
        mColorMapMinValue->setEnabled(true);
        mColorMapMaxValue->setEnabled(true);
      }

      emit settingsChanged();
    });
    formLayout->addRow("Color range option:", mColormapRangeSettings);

    //
    // Color map range
    //
    auto *colorMapRangeLayout = new QHBoxLayout;
    mColorMapMinValue         = new QLineEdit();
    mColorMapMinValue->setStatusTip("Color map min value.");
    mColorMapMaxValue = new QLineEdit();
    mColorMapMaxValue->setStatusTip("Color map max value.");
    colorMapRangeLayout->addWidget(mColorMapMinValue);
    colorMapRangeLayout->addWidget(mColorMapMaxValue);
    formLayout->addRow(new QLabel(tr("Color range:")), colorMapRangeLayout);
    mColorMapMinValue->setEnabled(false);
    mColorMapMaxValue->setEnabled(false);
    formLayout->setContentsMargins(2, 0, 0, 0);
    layout->addLayout(formLayout);
    layout->addStretch();

    connect(mColorMapMinValue, &QLineEdit::editingFinished, [this]() { emit settingsChanged(); });
    connect(mColorMapMaxValue, &QLineEdit::editingFinished, [this]() { emit settingsChanged(); });
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
joda::plot::ColormapName PanelGraphSettings::getSelectedColorMap() const
{
  return static_cast<joda::plot::ColormapName>(mColorMaps->currentData().toInt());
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto PanelGraphSettings::getColorMapRange() const -> joda::plot::ColorMappingRange
{
  return {mColorMapMinValue->text().toDouble(), mColorMapMaxValue->text().toDouble()};
}
///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelGraphSettings::setColorMapRange(const joda::plot::ColorMappingRange &range)
{
  mColorMapMinValue->blockSignals(true);
  mColorMapMaxValue->blockSignals(true);
  mColorMapMinValue->setText(QString::number(range.min));
  mColorMapMaxValue->setText(QString::number(range.max));
  mColorMapMinValue->blockSignals(false);
  mColorMapMaxValue->blockSignals(false);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto PanelGraphSettings::getColorMapRangeSetting() const -> joda::plot::ColorMappingMode
{
  return static_cast<joda::plot::ColorMappingMode>(mColormapRangeSettings->currentData().toInt());
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
