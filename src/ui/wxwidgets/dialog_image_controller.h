///
/// \file      dialog_image_controller.h
/// \author    Joachim Danmayr
/// \date      2023-09-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

#ifndef __dialog_image_controller__
#define __dialog_image_controller__

#include <wx/dcclient.h>
#include <wx/gdicmn.h>
#include <wx/toplevel.h>
#include <wx/wx.h>
#include <memory>
#include <mutex>
#include <thread>
#include "backend/image_processing/detection/detection_response.hpp"
#include "wxwidget.h"

namespace joda::ui::wxwidget {

class ImageZoomScrollWidget : public wxScrolledWindow
{
public:
  ImageZoomScrollWidget(wxWindow *parent);

  void updateImage(const wxImage &image);

private:
  /////////////////////////////////////////////////////
  void RenderImage(wxDC &dc);
  void OnPaint(wxPaintEvent &event);
  void OnMouseWheel(wxMouseEvent &event);
  void OnLeftDown(wxMouseEvent &event);
  void OnMouseMove(wxMouseEvent &event);
  void OnLeftUp(wxMouseEvent &event);
  void fitImagePosition(int newX, int newY);

  /////////////////////////////////////////////////////
  wxImage mImage;
  float mZoomFactor;
  float mZoomFactorMin;
  bool mDragging = false;
  wxPoint mDragStartPosition;
  wxPoint mImagePositionOnDragStart;
  wxPoint mActPosition = {0, 0};
};

///
/// \class      DialogImageController
/// \author     Joachim Danmayr
/// \brief
///
class DialogImageController : public DialogImage
{
public:
  /////////////////////////////////////////////////////
  DialogImageController(wxWindow *parent, wxWindowID id = wxID_ANY, const wxString &title = wxEmptyString,
                        const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxSize(424, 244),
                        long style = wxCAPTION | wxCLOSE_BOX);
  ~DialogImageController();

  struct SmallStatistics
  {
    int64_t valid   = 0;
    int64_t invalid = 0;
  };

  void startProgress(int maxTimeMs);
  void updateImage(const wxImage &image, const SmallStatistics &result);

private:
  /////////////////////////////////////////////////////
  void progressThread();
  void stopProgress();

  /////////////////////////////////////////////////////
  ImageZoomScrollWidget *mZoomScrollWidget;
  std::shared_ptr<std::thread> mProgressThread;
  std::mutex mProgressMutex;
  bool mStopped = false;
};
}    // namespace joda::ui::wxwidget
#endif    // __dialog_image_controller__
