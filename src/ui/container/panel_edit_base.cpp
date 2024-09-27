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

///

#include "panel_edit_base.hpp"
#include <qaction.h>
#include <qpushbutton.h>
#include "backend/helper/logger/console_logger.hpp"
#include "ui/helper/icon_generator.hpp"
#include "ui/helper/template_parser/template_parser.hpp"
#include "ui/window_main/window_main.hpp"
#include <nlohmann/json_fwd.hpp>
#include "container_base.hpp"

namespace joda::ui {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
PanelEdit::PanelEdit(WindowMain *wm, ContainerBase *containerBase, bool withExtraButtons) :
    mWindowMain(wm), mContainerBase(containerBase), mLayout(this, withExtraButtons)
{
  if(withExtraButtons) {
    auto *mSaveAsTemplate = new QAction(generateIcon("add-to-favorites"), "Save as template");
    mLayout.addItemToTopToolbar(mSaveAsTemplate);
    connect(mSaveAsTemplate, &QAction::triggered, this, &PanelEdit::onSaveAsTemplate);

    auto *copyChannel = new QAction(generateIcon("copy"), "Copy channel");
    mLayout.addItemToTopToolbar(copyChannel);
    connect(copyChannel, &QAction::triggered, this, &PanelEdit::onCopyChannel);
  }
}

///
/// \brief       Save as template
/// \author      Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PanelEdit::onSaveAsTemplate()
{
  if(mContainerBase != nullptr) {
    QString templatePath      = joda::templates::TemplateParser::getUsersTemplateDirectory().string().data();
    QString pathToStoreFileIn = QFileDialog::getSaveFileName(this, "Save File", templatePath,
                                                             "ImageC template files (*" + QString(joda::fs::EXT_PIPELINE_TEMPLATE.data()) + ")");

    if(pathToStoreFileIn.isEmpty()) {
      return;
    }
    if(!pathToStoreFileIn.startsWith(templatePath)) {
      joda::log::logError("Templates must be stored in >" + templatePath.toStdString() + "< directory.");
      QMessageBox messageBox(this);
      messageBox.setIconPixmap(generateIcon("warning-yellow").pixmap(48, 48));
      messageBox.setWindowTitle("Could not save template!");
      messageBox.setText("Templates must be stored in >" + templatePath + "< directory.");
      messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
      auto reply = messageBox.exec();
      return;
    }
    auto json = mContainerBase->toJson("");
    joda::templates::TemplateParser::saveTemplate(json, std::filesystem::path(pathToStoreFileIn.toStdString()));
  }
}

///
/// \brief       Save as template
/// \author      Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PanelEdit::onCopyChannel()
{
  nlohmann::json json = mContainerBase->toJson(" (copy)");
  getWindowMain()->getPanelPipeline()->addChannel(json);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelEdit::onValueChanged()
{
  valueChangedEvent();
}
}    // namespace joda::ui
