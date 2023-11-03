///
/// \file      dialog_image_controller.cpp
/// \author    Joachim Danmayr
/// \date      2023-09-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

#include "dialog_image_controller.h"
#include <wx/dcbuffer.h>
#include <string>

namespace joda::ui::wxwidget {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogImageController::DialogImageController(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos,
                                             const wxSize &size, long style) :
    DialogImage(parent, id, title, pos, size, style),
    mZoomScrollWidget(new ImageZoomScrollWidget(this))
{
  SetMinSize(wxSize{800, 600});
  auto *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(mZoomScrollWidget, 1, wxEXPAND | wxALL, 10);
  SetSizerAndFit(sizer);
  Layout();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageController::updateImage(wxImage &image)
{
  if(nullptr != mZoomScrollWidget) {
    mZoomScrollWidget->updateImage(image);
  }
}

///
/// \class
/// \author
/// \brief
///
ImageZoomScrollWidget::ImageZoomScrollWidget(wxWindow *parent) :
    wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxHSCROLL)
{
  SetMinSize(wxSize{(int) 800, 800});

  Bind(wxEVT_PAINT, &ImageZoomScrollWidget::OnPaint, this);
  Bind(wxEVT_MOUSEWHEEL, &ImageZoomScrollWidget::OnMouseWheel, this);
  Bind(wxEVT_LEFT_DOWN, &ImageZoomScrollWidget::OnLeftDown, this);
  Bind(wxEVT_LEFT_UP, &ImageZoomScrollWidget::OnLeftUp, this);
  Bind(wxEVT_MOTION, &ImageZoomScrollWidget::OnMouseMove, this);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ImageZoomScrollWidget::updateImage(wxImage &image)
{
  if(!mImage.IsOk() || (image.GetWidth() != mImage.GetWidth()) || image.GetHeight() != mImage.GetHeight()) {
    mZoomFactor    = 800.0 / (float) image.GetHeight();
    mZoomFactorMin = mZoomFactor;

    SetScrollbars(20, 20, image.GetWidth() / mZoomFactor, image.GetHeight() / mZoomFactor, 0, 0);
    float ratio = (float) image.GetWidth() / (float) image.GetHeight();
    float width = (float) 800 * ratio;
    SetMinSize(wxSize{(int) width, 800});
  }
  mImage = image;

  Refresh();
}

void ImageZoomScrollWidget::OnPaint(wxPaintEvent &event)
{
  wxPaintDC dc(this);
  RenderImage(dc);
}

void ImageZoomScrollWidget::RenderImage(wxDC &dc)
{
  if(mImage.IsOk()) {
    int width  = mImage.GetWidth() * mZoomFactor;
    int height = mImage.GetHeight() * mZoomFactor;
    if(mImage.Ok()) {
      wxBitmap bmp(mImage.Scale(width, height));
      dc.DrawBitmap(bmp, mActPosition.x, mActPosition.y);
    }
  }
}

void ImageZoomScrollWidget::OnMouseWheel(wxMouseEvent &event)
{
  if(mImage.IsOk()) {
    // int delta = event.GetWheelRotation();
    float delta      = 0.05 * (event.GetWheelRotation() > 0 ? 1 : -1);
    float zoomFactor = mZoomFactor + delta;

    if(zoomFactor < mZoomFactorMin) {
      zoomFactor = mZoomFactorMin;
    }
    if(zoomFactor > 3) {
      zoomFactor = 3;
    }

    mZoomFactor = zoomFactor;
    int width   = mImage.GetWidth() * mZoomFactor;
    int height  = mImage.GetHeight() * mZoomFactor;

    fitImagePosition(mActPosition.x, mActPosition.y);

    SetVirtualSize(width, height);
    Refresh();
  }
}

void ImageZoomScrollWidget::OnLeftDown(wxMouseEvent &event)
{
  if(mImage.IsOk()) {
    if(!mDragging) {
      mDragging                 = true;
      mDragStartPosition        = event.GetPosition();
      mImagePositionOnDragStart = mActPosition;
      CaptureMouse();
    }
  }
}

void ImageZoomScrollWidget::OnMouseMove(wxMouseEvent &event)
{
  if(mImage.IsOk()) {
    if(mDragging) {
      wxPoint currentPosition = event.GetPosition();
      int dx                  = (currentPosition.x - mDragStartPosition.x);    /// mZoomFactor;
      int dy                  = (currentPosition.y - mDragStartPosition.y);    /// mZoomFactor;
      // Scroll(dx, dy);
      int newX = mImagePositionOnDragStart.x + dx;
      int newY = mImagePositionOnDragStart.y + dy;
      fitImagePosition(newX, newY);

      // m_dragStartPosition = currentPosition;
      Refresh();
    }
  }
}

void ImageZoomScrollWidget::fitImagePosition(int newX, int newY)
{
  if(mImage.IsOk()) {
    if(newX > 0) {
      newX = 0;
    }
    int minX = GetSize().GetWidth() - mImage.GetWidth() * mZoomFactor;
    if(minX > 0) {
      minX = 0;
    }
    if(newX < minX) {
      newX = minX;
    }

    if(newY > 0) {
      newY = 0;
    }
    int minY = GetSize().GetHeight() - mImage.GetHeight() * mZoomFactor;
    if(minY > 0) {
      minY = 0;
    }
    if(newY < minY) {
      newY = minY;
    }

    mActPosition.x = newX;
    mActPosition.y = newY;
  }
}

void ImageZoomScrollWidget::OnLeftUp(wxMouseEvent &event)
{
  if(mImage.IsOk()) {
    if(mDragging) {
      mDragging = false;
      ReleaseMouse();
    }
  }
}

}    // namespace joda::ui::wxwidget
