///
/// \file      panel_pipeline_compile_log.hpp
/// \author    Joachim Danmayr
/// \date      2024-09-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qtablewidget.h>
#include <qwidget.h>
#include <mutex>

namespace joda::settings {
class AnalyzeSettings;
}

namespace joda::ui::gui {

class WindowMain;

///
/// \class
/// \author
/// \brief
///
class PanelCompilerLog
{
public:
  /////////////////////////////////////////////////////
  PanelCompilerLog(WindowMain *parent);
  void updateCompilerLog(const joda::settings::AnalyzeSettings &);
  void showDialog();
  int32_t getNumberOfErrors() const
  {
    return mNrOfErrors;
  }

private:
  /////////////////////////////////////////////////////
  QTableWidget *mLogOutput;
  QDialog *mDialog;
  int32_t mNrOfErrors = 0;
  std::mutex mWriteMutex;
};

}    // namespace joda::ui::gui
