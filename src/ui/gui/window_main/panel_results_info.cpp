///
/// \file      panel_image.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "panel_results_info.hpp"
#include <qboxlayout.h>
#include <qlineedit.h>
#include <string>
#include "backend/helper/file_parser/directory_iterator.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"
#include "backend/settings/project_settings/project_plates.hpp"
#include "ui/gui/results/panel_results.hpp"
#include "ui/gui/window_main/window_main.hpp"
#include "panel_results_table_generator.hpp"

namespace joda::ui::gui {

PanelResultsInfo::PanelResultsInfo(WindowMain *windowMain) : mWindowMain(windowMain)
{
  auto *layout = new QVBoxLayout();

  auto addSeparator = [&layout]() {
    QFrame *separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    layout->addWidget(separator);
  };

  // layout->setContentsMargins(0, 0, 0, 0);
  {
      // auto *mSearchField = new QLineEdit();
      // mSearchField->setPlaceholderText("Search ...");
      // layout->addWidget(mSearchField);
      // connect(mSearchField, &QLineEdit::editingFinished, this, &PanelResultsInfo::filterResults);
  }

  // addSeparator();
  {
    mResultsTemplate = new PanelResultsTableGenerator(mWindowMain, &mWindowMain->mutableSettings());
    layout->addWidget(mResultsTemplate);
  }

  setLayout(layout);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResultsInfo::filterResults()
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResultsInfo::fromSettings(const joda::settings::AnalyzeSettings &settings)
{
  mWindowMain->mutableSettings().resultsSettings = settings.resultsSettings;
  mResultsTemplate->refreshView();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResultsInfo::refreshTableView()
{
  mResultsTemplate->refreshView();
}

}    // namespace joda::ui::gui
