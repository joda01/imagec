///
/// \file      table_model.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-06
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include <qabstractitemmodel.h>
#include "backend/helper/table/table.hpp"
#include "backend/processor/initializer/pipeline_settings.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "ui/gui/dialogs/dialog_image_view/painted_roi_properties.hpp"

namespace joda::ui::gui {

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class TableModelPaintedPolygon : public QAbstractTableModel
{
  Q_OBJECT

public:
  static constexpr int32_t CLASS_ROLE = 0x101;

  /////////////////////////////////////////////////////
  TableModelPaintedPolygon(QObject *parent = nullptr);
  void setData(std::vector<PaintedRoiProperties> *polygons);
  auto getCell(int row) -> PaintedRoiProperties *;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  void refresh();

private:
  std::vector<PaintedRoiProperties> *mPolygons = nullptr;
};

}    // namespace joda::ui::gui
