///
/// \file      dialog_roi_manager.hpp
/// \author    Joachim Danmayr
/// \date      2025-08-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qcombobox.h>
#include <qdialog.h>
#include <qwidget.h>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "controller/controller.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox_classes_out.hpp"
#include "ui/gui/helper/table_view.hpp"
#include "ui/gui/helper/table_widget.hpp"

namespace joda::ui::gui {

class WindowMain;
class PanelImageView;
class TableModelPaintedPolygon;

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class DialogRoiManager : public QDialog
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogRoiManager(PanelImageView *imagePanel, QWidget *parent);

signals:
  void dialogDisappeared();    // custom signal

protected:
  void hideEvent(QHideEvent *event) override;
  void closeEvent(QCloseEvent *event) override;

private:
  /////////////////////////////////////////////////////
  PanelImageView *mImagePanel;
  PlaceholderTableView *mPolygonsTable;
  TableModelPaintedPolygon *mTableModel;

  // PAINTING///////////////////////////////////////////////////
  QAction *mMoveAction;
  QAction *mSelectAction;
};

}    // namespace joda::ui::gui
