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

#include "panel_edit_base.hpp"
#include <qaction.h>
#include <qpushbutton.h>
#include "backend/helper/template_parser/template_parser.hpp"
#include "container_base.hpp"

namespace joda::ui::qt {

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
    auto *mSaveAsTemplate = new QAction(QIcon(":/icons/outlined/icons8-add-to-favorites-50.png"), "Save as template");
    mLayout.addItemToTopToolbar(mSaveAsTemplate);
    connect(mSaveAsTemplate, &QAction::triggered, this, &PanelEdit::onSaveAsTemplate);

    auto *copyChannel = new QAction(QIcon(":/icons/outlined/icons8-copy-50.png"), "Copy channel");
    mLayout.addItemToTopToolbar(copyChannel);
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
    QString templatePath      = joda::helper::templates::TemplateParser::getUsersTemplateDirectory().string().data();
    QString pathToStoreFileIn = QFileDialog::getSaveFileName(
        this, "Save File", templatePath,
        "ImageC template files (*" + QString(joda::helper::templates::TemplateParser::TEMPLATE_ENDIAN.data()) + ")");

    if(pathToStoreFileIn.isEmpty()) {
      return;
    }
    if(!pathToStoreFileIn.startsWith(templatePath)) {
      joda::log::logError("Templates must be stored in >" + templatePath.toStdString() + "< directory.");
      QMessageBox messageBox(this);
      auto *icon = new QIcon(":/icons/outlined/icons8-warning-50.png");
      messageBox.setIconPixmap(icon->pixmap(42, 42));
      messageBox.setWindowTitle("Could not save template!");
      messageBox.setText("Templates must be stored in >" + templatePath + "< directory.");
      messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
      auto reply = messageBox.exec();
      return;
    }
    auto json = mContainerBase->toJson();
    joda::helper::templates::TemplateParser::saveTemplate(json, std::filesystem::path(pathToStoreFileIn.toStdString()));
  }
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
}    // namespace joda::ui::qt
