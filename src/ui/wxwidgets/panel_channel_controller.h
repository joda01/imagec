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

#include "backend/helper/two_way_map.hpp"
#include "backend/settings/channel_settings.hpp"
#include <nlohmann/json_fwd.hpp>
#include "wxwidget.h"

//// end generated include

namespace joda::ui::wxwidget {

///
/// \class      PanelChannelController
/// \author     Joachim Danmayr
/// \brief      Panel channel controller class
///
class PanelChannelController : public PanelChannel
{
public:
  /////////////////////////////////////////////////////
  explicit PanelChannelController(wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition,
                                  const wxSize &size = wxSize(250, -1), long style = wxTAB_TRAVERSAL,
                                  const wxString &name = wxEmptyString);

  void loadValues(const joda::settings::json::ChannelSettings &);
  nlohmann::json getValues();

private:
  /////////////////////////////////////////////////////
  static auto indexToType(int idx) -> std::string;
  static auto typeToIndex(const std::string &str) -> int;

  static inline joda::helper::TwoWayMap<int, std::string> myMap{
      {{0, "SPOT"}, {1, "NUCLEUS"}, {2, "CELL"}, {3, "BACKGROUND"}}};
};
}    // namespace joda::ui::wxwidget

#endif    // __panel_channel_controller__
