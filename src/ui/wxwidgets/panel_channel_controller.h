///
/// \file      panel_channel_controller.h
/// \author    Joachim Danmayr
/// \date      2023-09-14
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#ifndef __panel_channel_controller__
#define __panel_channel_controller__

#include <map>
#include <memory>
#include "backend/helper/two_way_map.hpp"
#include "backend/settings/channel_settings.hpp"
#include "ui/wxwidgets/dialog_image_controller.h"
#include <nlohmann/json_fwd.hpp>
#include "wxwidget.h"

//// end generated include

namespace joda::ui::wxwidget {

class FrameMainController;

///
/// \class      PanelChannelController
/// \author     Joachim Danmayr
/// \brief      Panel channel controller class
///
class PanelChannelController : public PanelChannel
{
public:
  /////////////////////////////////////////////////////
  explicit PanelChannelController(FrameMainController *mainFrame, wxWindow *parent, wxWindowID id = wxID_ANY,
                                  const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxSize(250, -1),
                                  long style = wxTAB_TRAVERSAL, const wxString &name = wxEmptyString);

  void loadValues(const joda::settings::json::ChannelSettings &);
  nlohmann::json getValues();

private:
  /////////////////////////////////////////////////////
  static auto indexToType(int idx) -> std::string;
  static auto typeToIndex(const std::string &str) -> int;

  static auto indexToZProjection(int idx) -> std::string;
  static auto zProjectionToIndex(const std::string &str) -> int;

  static auto indexToThreshold(int idx) -> std::string;
  static auto thresholdToIndex(const std::string &str) -> int;

  static auto indexToFilterKernel(int idx) -> int16_t;
  static auto filterKernelToIndex(int16_t kernel) -> int;

  static auto splitAndConvert(const std::string &input, char delimiter) -> std::tuple<int, int>;

  static inline joda::helper::TwoWayMap<int, std::string> CHANNEL_TYPES{
      {{0, "SPOT"}, {1, "SPOT_REFERENCE"}, {2, "NUCLEUS"}, {3, "CELL"}, {4, "BACKGROUND"}}};

  static inline joda::helper::TwoWayMap<int, std::string> Z_STACK_PROJECTION{
      {{0, "NONE"}, {1, "PROJECT_MAX_INTENSITY"}, {2, "PROJECT_3D"}}};

  static inline joda::helper::TwoWayMap<int, std::string> THRESHOLD_METHOD{
      {{0, "MANUAL"}, {1, "LI"}, {2, "MIN_ERROR"}, {3, "TRIANGLE"}, {4, "MOMENTS"}}};

  static inline joda::helper::TwoWayMap<int, int16_t> GAUSSIAN_BLUR{{{0, 0}, {1, 3}, {2, 5}, {3, 7}, {4, 11}, {5, 13}}};

private:
  /////////////////////////////////////////////////////
  void onRemoveClicked(wxCommandEvent &event) override;
  void onPreviewClicked(wxCommandEvent &event) override;
  void onChannelTypeChanged(wxCommandEvent &event) override;
  void onAiCheckBox(wxCommandEvent &event) override;

  FrameMainController *mMainFrame;
  std::multimap<int, std::shared_ptr<DialogImageController>> mPreviewDialogs;
};
}    // namespace joda::ui::wxwidget

#endif    // __panel_channel_controller__
