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
#include "backend/artifacts/roi/roi.hpp"
#include "backend/helper/table/table.hpp"
#include "backend/processor/initializer/pipeline_settings.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include "backend/settings/project_settings/project_classification.hpp"

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
  TableModelPaintedPolygon(const joda::settings::Classification *classification, const std::shared_ptr<atom::ObjectList> &polygons,
                           QObject *parent = nullptr);
  auto getCell(int row) -> const atom::ROI *;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  void refresh();
  int32_t indexFor(atom::ROI *) const;

private:
  const joda::settings::Classification *mClassification = nullptr;
  std::shared_ptr<atom::ObjectList> mObjectMap          = nullptr;
};

}    // namespace joda::ui::gui
