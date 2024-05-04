///
/// \file      panel_channel.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <qtmetamacros.h>
#include <QtWidgets>
#include <memory>
#include <mutex>
#include "ui/helper/waitingspinnerwidget.hpp"
#include "ui/panel_preview.hpp"

namespace joda::ui::qt {

class WindowMain;

class PanelReporting : public QWidget
{
  Q_OBJECT

signals:
  void updatePreviewStarted();
  void updatePreviewFinished();
  void setActualSelectedResultsFolder(const QString &folder);

public:
  PanelReporting(WindowMain *wm);
  ~PanelReporting();

  void setActive(bool setActive)
  {
    if(!mIsActiveShown && setActive) {
      mIsActiveShown = true;
    }
    if(!setActive) {
      mIsActiveShown = false;
    }
  }

private:
  /////////////////////////////////////////////////////
  QHBoxLayout *createLayout();
  std::tuple<QVBoxLayout *, QWidget *> addVerticalPanel(QLayout *horizontalLayout, const QString &bgColor,
                                                        int margin = 16, bool enableScrolling = false,
                                                        int maxWidth = 250, int spacing = 4) const;

  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  QLabel *createTitle(const QString &);
  bool mIsActiveShown = false;
  QString mActualSelectedResultsFolder;

private slots:
  void onExportToXlsxClicked();
};

}    // namespace joda::ui::qt
