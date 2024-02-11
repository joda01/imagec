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
#include <wx/gdicmn.h>
#include <memory>
#include <string>
#include <thread>

namespace joda::ui::wxwidget {

using namespace std::chrono_literals;

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
    mZoomScrollWidget(new ImageZoomScrollWidget(this)), mProgressMutex()
{
  mProgressThread = std::make_shared<std::thread>(&DialogImageController::progressThread, this);
  SetMinSize(wxSize{500, 500});
  SetMaxSize(wxSize{2000, 2000});
  mSizer->Add(mZoomScrollWidget, 1, wxEXPAND | wxALL, 10);
  // mSizer->Insert(0, mZoomScrollWidget, 1, wxEXPAND | wxALL, 10);
  Fit();
  Layout();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogImageController::~DialogImageController()
{
  mStopped = true;
  std::lock_guard<std::mutex> lock(mProgressMutex);    // Lock the mutex
  if(mProgressThread && mProgressThread->joinable()) {
    mProgressThread->join();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageController::updateImage(const wxImage &image, const std::string &imageFileName, int tileNr,
                                        const SmallStatistics &result)
{
  if(nullptr != mZoomScrollWidget) {
    CallAfter([this, imageFileName, result, tileNr]() {
      mImagePath->SetLabel(imageFileName + " | Tile: " + std::to_string(tileNr) + " | Valid: " +
                           std::to_string(result.valid) + " | Invalid: " + std::to_string(result.invalid));
    });

    mZoomScrollWidget->updateImage(image);
    stopProgress();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageController::progressThread()
{
  while(!mStopped) {
    uint32_t actValue = 0;
    uint32_t maxValue = 0;
    {
      std::lock_guard<std::mutex> lock(mProgressMutex);    // Lock the mutex
      actValue = mImageDisplayProgress->GetValue();
      maxValue = mImageDisplayProgress->GetRange();
    }

    actValue += 1;
    if(actValue < maxValue) {
      CallAfter([this, actValue]() { mImageDisplayProgress->SetValue(actValue); });

      std::this_thread::sleep_for(10ms);
    } else {
      std::this_thread::sleep_for(100ms);
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageController::startProgress(int maxTimeMs)
{
  std::lock_guard<std::mutex> lock(mProgressMutex);    // Lock the mutex
  CallAfter([this, maxTimeMs]() {
    mImageDisplayProgress->SetValue(0);
    mImageDisplayProgress->SetRange(maxTimeMs);
    mImageDisplayProgress->Show(true);
  });
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageController::stopProgress()
{
  mImageDisplayProgress->Show(false);
}

///
/// \class
/// \author
/// \brief
///
ImageZoomScrollWidget::ImageZoomScrollWidget(wxWindow *parent) :
    wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxHSCROLL)
{
  SetMinSize(wxSize{500, 500});
  SetMaxSize(wxSize{2000, 2000});
  Bind(wxEVT_PAINT, &ImageZoomScrollWidget::OnPaint, this);
  Bind(wxEVT_MOUSEWHEEL, &ImageZoomScrollWidget::OnMouseWheel, this);
  Bind(wxEVT_LEFT_DOWN, &ImageZoomScrollWidget::OnLeftDown, this);
  Bind(wxEVT_LEFT_UP, &ImageZoomScrollWidget::OnLeftUp, this);
  Bind(wxEVT_MOTION, &ImageZoomScrollWidget::OnMouseMove, this);
  ShowScrollbars(wxScrollbarVisibility::wxSHOW_SB_NEVER, wxScrollbarVisibility::wxSHOW_SB_NEVER);
  wxSetCursor(wxCURSOR_HAND);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ImageZoomScrollWidget::updateImage(const wxImage &image)
{
  if(!mImage.IsOk() || (image.GetWidth() != mImage.GetWidth()) || image.GetHeight() != mImage.GetHeight()) {
    mZoomFactor    = (float) GetSize().GetHeight() / (float) image.GetHeight();
    mZoomFactorMin = mZoomFactor;

    SetScrollbars(20, 20, image.GetWidth() / mZoomFactor, image.GetHeight() / mZoomFactor, 0, 0);
    float ratio = (float) image.GetWidth() / (float) image.GetHeight();
    float width = (float) GetSize().GetWidth() * ratio;
    SetMinSize(wxSize{(int) width, GetSize().GetHeight()});
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
    mZoomFactorMin = (float) GetSize().GetHeight() / (float) mImage.GetHeight();
    if(mZoomFactor < mZoomFactorMin) {
      mZoomFactor = mZoomFactorMin;
    }

    int scaledImageWidth  = mImage.GetWidth() * mZoomFactor;
    int scaledImageHeight = mImage.GetHeight() * mZoomFactor;

    int windowWidth  = GetSize().GetWidth();
    int windowHeight = GetSize().GetHeight();

    if(mImage.Ok()) {
      centerImage();
      wxBitmap bmp(mImage.Scale(scaledImageWidth, scaledImageHeight));
      dc.DrawBitmap(bmp, mActPosition.x + mPositionOffsetToCenter.x, mActPosition.y + mPositionOffsetToCenter.y);

      int rectWidth = 100;
      int rectPosX  = 20 + mPositionOffsetToCenter.x;
      int rectPosY  = 20 + mPositionOffsetToCenter.y;

      //
      // Help for orientation
      //
      // Draw a rectangle with blue borders which should display the whole image
      dc.SetPen(wxPen(wxColour(0, 0, 255), 2));                                   // Blue pen with width 2
      dc.SetBrush(wxBrush(wxColour(255, 255, 255), wxBRUSHSTYLE_TRANSPARENT));    // White brush (filled rectangle)
      wxRect rect(rectPosX, rectPosY, rectWidth, rectWidth);
      dc.DrawRectangle(rect);

      float widthScaleFactor = (float) windowWidth / (float) scaledImageWidth;
      if(widthScaleFactor > 1) {
        widthScaleFactor = 1;
      }

      float heightScaleFactor = (float) windowHeight / (float) scaledImageHeight;
      if(heightScaleFactor > 1) {
        heightScaleFactor = 1;
      }

      int posX = (mActPosition.x * rectWidth) / scaledImageWidth;
      int posY = (mActPosition.y * rectWidth) / scaledImageHeight;

      // Draw a rectangle with blue borders which should display the image part which is displayed
      dc.SetPen(wxPen(wxColour(0, 0, 255), 2));                                   // Blue pen with width 2
      dc.SetBrush(wxBrush(wxColour(255, 255, 255), wxBRUSHSTYLE_TRANSPARENT));    // White brush (filled rectangle)
      wxRect rectPart(rectPosX - posX, rectPosY - posY, rectWidth * widthScaleFactor, rectWidth * heightScaleFactor);
      dc.DrawRectangle(rectPart);
    }
  }
}

void ImageZoomScrollWidget::OnMouseWheel(wxMouseEvent &event)
{
  if(mImage.IsOk()) {
    // int delta = event.GetWheelRotation();
    float delta      = 0.01 * (event.GetWheelRotation() > 0 ? 1 : -1);
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
      CallAfter([this]() { Refresh(); });
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

void ImageZoomScrollWidget::centerImage()
{
  if(mImage.IsOk()) {
    int deltaX = GetSize().GetWidth() - mImage.GetWidth() * mZoomFactor;
    if(deltaX > 0) {
      mPositionOffsetToCenter.x = deltaX / 2;
    } else {
      mPositionOffsetToCenter.x = 0;
    }
    int deltaY = GetSize().GetHeight() - mImage.GetHeight() * mZoomFactor;
    if(deltaY > 0) {
      mPositionOffsetToCenter.y = deltaY / 2;
    } else {
      mPositionOffsetToCenter.y = 0;
    }
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
