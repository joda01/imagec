///
/// \file      table_model.cpp
/// \author    Joachim Danmayr
/// \date      2025-07-06
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "table_model_roi.hpp"
#include <qbrush.h>
#include <qnamespace.h>
#include <qtableview.h>
#include <QFile>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/base32.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/results_settings/results_settings.hpp"
#include "ui/gui/dialogs/dialog_image_view/panel_image_view.hpp"

namespace joda::ui::gui {

TableModelRoi::TableModelRoi(const PanelImageView *imageView, const joda::settings::Classification *classSettings, QObject *parent) :
    QAbstractTableModel(parent), mPanelImageView(imageView), mClassSettings(classSettings)
{
  if(parent == nullptr) {
    throw std::runtime_error("Parent must not be empty and of type QTableView.");
  }
}

void TableModelRoi::setData(joda::atom::ROI *roi)
{
  if(roi != nullptr) {
    std::lock_guard<std::mutex> lock(mSetDataLock);
    mROI = std::make_unique<joda::atom::ROI>(roi->clone());
  } else {
    std::lock_guard<std::mutex> lock(mSetDataLock);
    mROI.reset();
  }
  refresh();
}

int TableModelRoi::rowCount(const QModelIndex & /*parent*/) const
{
  std::lock_guard<std::mutex> lock(mSetDataLock);
  if(mROI == nullptr) {
    return 0;
  }
  return NR_ROWS;
}

int TableModelRoi::columnCount(const QModelIndex & /*parent*/) const
{
  std::lock_guard<std::mutex> lock(mSetDataLock);
  if(mROI == nullptr) {
    return 0;
  }
  return 2;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
QVariant TableModelRoi::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
  std::lock_guard<std::mutex> lock(mSetDataLock);
  if(mROI == nullptr) {
    return {};
  }
  if(role != Qt::DisplayRole) {
    return {};
  }
  if(section == 0) {
    return {"Name"};
  }
  if(section == 1) {
    return {"Value"};
  }
  return {};
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
QVariant TableModelRoi::data(const QModelIndex &index, int role) const
{
  std::lock_guard<std::mutex> lock(mSetDataLock);
  if(mROI == nullptr) {
    return {};
  }

  if(role == Qt::DisplayRole) {
    if(index.column() == 0) {
      switch(index.row()) {
        case 0:
          return "Object ID";
        case 1:
          return "Category";
        case 2:
          return "Class";
        case 3:
          return "Perimeter";
        case 4:
          return "Area size";
        case 5:
          return "Circularity";
        case 6:
          return "Centroid x";
        case 7:
          return "Centroid y";
        case 8:
          return "Intensity min.";
        case 9:
          return "Intensity max.";
        case 10:
          return "Intensity avg.";
        case 11:
          return "Intensity sum.";
      }
    } else {
      const auto &phSyUnit                 = mPanelImageView->getPhysicalSizeSettings();
      nlohmann::json physicalImageSizeUnit = phSyUnit.pixelSizeUnit;
      const auto &phSy                     = mPanelImageView->getOmeInfo().getPhyiscalSize(mPanelImageView->getSeries());

      const auto intensity =
          mROI->measureIntensityAndAdd({.zProjection = mPanelImageView->getZprojection(), .imagePlane = mPanelImageView->getImagePlane()},
                                       *mPanelImageView->getImage()->getOriginalImage());

      switch(index.row()) {
        case 0:
          return QString(std::to_string(mROI->getObjectId()).data());
        case 1: {
          switch(mROI->getCategory()) {
            case atom::ROI::Category::ANY:
              return QString("Unknown");
            case atom::ROI::Category::AUTO_SEGMENTATION:
              return QString("Pipeline segmented");
            case atom::ROI::Category::MANUAL_SEGMENTATION:
              return QString("Manual segmented");
          }
          break;
        }
        case 2:
          return QString(mClassSettings->getClassFromId(mROI->getClassId()).name.data());
        case 3:
          return QString::number(static_cast<double>(mROI->getPerimeter(phSy, phSyUnit.pixelSizeUnit))) + " " +
                 QString(physicalImageSizeUnit.get<std::string>().data());
        case 4:
          return QString::number(mROI->getAreaSize(phSy, phSyUnit.pixelSizeUnit)) + " " + QString(physicalImageSizeUnit.get<std::string>().data()) +
                 "²";
        case 5:
          return QString::number(static_cast<double>(mROI->getCircularity()));
        case 6:
          return QString::number(mROI->getCentroidReal().x) + " px";
        case 7:
          return QString::number(mROI->getCentroidReal().y) + " px";
        case 8:
          return QString::number(intensity.intensityMin);
        case 9:
          return QString::number(intensity.intensityMax);
        case 10:
          return QString::number(static_cast<double>(intensity.intensityAvg));
        case 11:
          return QString::number(static_cast<double>(intensity.intensitySum));
      }

      //
    }
  }
  return {};
}

void TableModelRoi::refresh()
{
  beginResetModel();
  endResetModel();
}

}    // namespace joda::ui::gui
