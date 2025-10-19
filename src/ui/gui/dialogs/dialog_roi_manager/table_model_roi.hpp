///
/// \file      table_model_roi.hpp
/// \author    Joachim Danmayr
/// \date      2025-10-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include <qabstractitemmodel.h>
#include <memory>
#include <mutex>
#include "backend/artifacts/roi/roi.hpp"
#include "backend/helper/table/table.hpp"
#include "backend/processor/initializer/pipeline_settings.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include "backend/settings/project_settings/project_classification.hpp"

namespace joda::ui::gui {

class PanelImageView;

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class TableModelRoi : public QAbstractTableModel
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  TableModelRoi(const PanelImageView *, const joda::settings::Classification *, QObject *parent = nullptr);
  void setData(joda::atom::ROI *roi);
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  void refresh();
  auto getActRoi() -> atom::ROI *
  {
    if(mROI) {
      return mROI.get();
    }
    return nullptr;
  }

private:
  static inline const int32_t NR_ROWS = 12;

  /////////////////////////////////////////////////////
  mutable std::mutex mSetDataLock;
  const PanelImageView *mPanelImageView;
  std::unique_ptr<joda::atom::ROI> mROI;
  const joda::settings::Classification *mClassSettings;
};

}    // namespace joda::ui::gui
