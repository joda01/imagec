///
/// \file      panel_image.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qcombobox.h>
#include <qwidget.h>
#include <QtWidgets>
#include <filesystem>
#include <optional>
#include <utility>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/settings/project_settings/project_settings.hpp"
#include "ui/gui/helper/color_combo/color_combo.hpp"
#include "ui/gui/helper/table_widget.hpp"
#include "ui/gui/results/dialog_class_settings.hpp"

class PlaceholderTableView;

namespace joda::ui::gui {

class WindowMain;
class DialogImageViewer;
class TableModelRoi;
class TableModelClasses;

///
/// \class
/// \author
/// \brief
///
class PanelClassification : public QWidget
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  explicit PanelClassification(const std::shared_ptr<atom::ObjectList> &objectMap, joda::settings::Classification *settings, WindowMain *windowMain,
                               DialogImageViewer *imageView);
  void fromSettings(const joda::settings::Classification &settings);
  [[nodiscard]] auto getClasses() const -> std::map<enums::ClassIdIn, QString>;
  auto getSelectedClass() const -> enums::ClassId;

  [[nodiscard]] QSize sizeHint() const override
  {
    return QSize(300, QWidget::sizeHint().height());
  }

signals:
  void settingsChanged();

private:
  /////////////////////////////////////////////////////
  void loadTemplates();
  void newTemplate();
  void saveAsNewTemplate();
  void openEditDialog(joda::settings::Class *, int32_t row);
  void openTemplate(const QString &path);
  void populateClassesFromImage();
  void addClass();
  void moveClassToPosition(int32_t fromPos, int32_t newPos);
  auto findNextFreeClassId() -> enums::ClassId;

  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  joda::settings::Classification *mSettings;
  PlaceholderTableView *mTableClasses;
  TableModelClasses *mTableModelClasses;

  /////////////////////////////////////////////////////
  QAction *mActionHideClass;

  /// ROI DETAILS ///////////////////////////////////
  PlaceholderTableView *mTableRoiDetails;
  TableModelRoi *mTableModelRoi;

  /// DIALOG //////////////////////////////////////////////////
  DialogClassSettings *mClassSettingsDialog;

  /// Actions //////////////////////////////////////////////////
  QMenu *mTemplateMenu;
  const std::shared_ptr<atom::ObjectList> mObjectMap;
  DialogImageViewer *mDialogImageView;

  /// TEMPLATE //////////////////////////////////////////////////
  bool askForChangeTemplateIndex();
  bool askForDeleteClass();
  bool mDontAsk = false;

private slots:
  void onSettingChanged();
  void moveUp();
  void moveDown();
};
}    // namespace joda::ui::gui
