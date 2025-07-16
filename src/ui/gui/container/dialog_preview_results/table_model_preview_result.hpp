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
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/table/table.hpp"
#include "backend/processor/initializer/pipeline_settings.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "controller/controller.hpp"

namespace joda::ui::gui {

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class TableModelPreviewResult : public QAbstractTableModel
{
  Q_OBJECT

public:
  static constexpr int32_t CLASS_ROLE = 0x101;

  /////////////////////////////////////////////////////
  TableModelPreviewResult(const joda::settings::Classification &, QObject *parent = nullptr);
  void setData(joda::ctrl::Preview::PreviewResults *pipelines);
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  void refresh();
  void setHiddenFlag(enums::ClassId classs, bool);

private:
  /////////////////////////////////////////////////////
  joda::ctrl::Preview::PreviewResults *mPreviewResult = nullptr;
  const joda::settings::Classification &mClassSettings;
  QString base64IconName;
  QString base64IconHash;
  QColor ALTERNATE;
  QColor BASE;
};

}    // namespace joda::ui::gui
