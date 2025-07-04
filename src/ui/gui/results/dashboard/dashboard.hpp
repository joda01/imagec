///
/// \file      dashboard.hpp
/// \author    Joachim Danmayr
/// \date      2025-06-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qmdiarea.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <set>
#include <vector>
#include "backend/database/exporter/exportable.hpp"
#include "backend/enums/enums_classes.hpp"
#include "ui/gui/helper/table_widget.hpp"

namespace joda::table {
class Table;
}    // namespace joda::table

namespace joda::ui::gui {

class DashboardElement;
class WindowMain;
class PanelResults;

class Dashboard : public QMdiArea
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  Dashboard(PanelResults *panelResults, WindowMain *mainWindow);
  void reset();
  void tableToQWidgetTable(const joda::table::Table &tableIn, const std::set<std::set<enums::ClassId>> &classesWithSameTrackingId, bool isImageView);
  void copyToClipboard() const;
  auto getExportables() const -> std::vector<const exporter::Exportable *>;

private:
  enum class DashboardType
  {
    UNKNOWN,
    NORMAL,
    INTERSECTION,
    DISTANCE,
    COLOC
  };

  /////////////////////////////////////////////////////
  void copyTableToClipboard(QTableWidget *table) const;
  void clearLayout();

  struct MidiWindowKey
  {
    DashboardType type = DashboardType::UNKNOWN;
    uint32_t key       = 0;
    bool operator<(const MidiWindowKey &in) const
    {
      return ((static_cast<uint64_t>(type) << 32) | key) < ((static_cast<uint64_t>(in.type) << 32) | in.key);
    }
  };

  /////////////////////////////////////////////////////
  PanelResults *mPanelResults;
  WindowMain *mMainWindow;
  int32_t mSelectedTableColumnIdx = -1;
  int32_t mSelectedTableRow       = -1;
  std::map<MidiWindowKey, DashboardElement *> mMidiWindows;
};

}    // namespace joda::ui::gui
