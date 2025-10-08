///
/// \file      table_view.cpp
/// \author    Joachim Danmayr
/// \date      2025-10-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "table_view.hpp"

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PlaceholderTableView::paintEvent(QPaintEvent *event)
{
  QTableView::paintEvent(event);

  if(model()->rowCount() == 0) {
    QPainter painter(viewport());
    painter.setPen(Qt::gray);
    painter.drawText(QRect(0, 0, width(), height()), Qt::AlignCenter, mPlaceholderText);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PlaceholderTableView::keyPressEvent(QKeyEvent *event)
{
  if(event->key() == Qt::Key_Delete) {
    emit deletePressed();
    // return;
  }
  QTableView::keyPressEvent(event);
}
