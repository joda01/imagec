///
/// \file      panel_pipeline_compile_log.hpp
/// \author    Joachim Danmayr
/// \date      2024-09-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
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
  void hideDialog();
  int32_t getNumberOfErrors() const
  {
    return mNrOfErrors;
  }
  QDialog *getDialog()
  {
    return mDialog;
  }

private:
  /////////////////////////////////////////////////////
  QTableWidget *mLogOutput;
  QDialog *mDialog;
  int32_t mNrOfErrors = 0;
  std::mutex mWriteMutex;
};

}    // namespace joda::ui::gui
