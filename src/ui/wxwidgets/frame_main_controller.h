#ifndef __frame_main_controller__
#define __frame_main_controller__

/**
@file
Subclass of frameMain, which is generated by wxFormBuilder.
*/

#include <memory>
#include <vector>
#include "wxwidget.h"

//// end generated include

namespace joda {
namespace ui {
namespace wxwidget {

class FrameMainController : public frameMain
{
public:
  /////////////////////////////////////////////////////
  FrameMainController(wxWindow *parent);
  void addChannel();
  void removeChannel();

private:
  /////////////////////////////////////////////////////
  void onAddChannelClicked(wxCommandEvent &event) override;

  /////////////////////////////////////////////////////
  std::vector<std::shared_ptr<PanelChannel>> mChannels;
};
}    // namespace wxwidget
}    // namespace ui
}    // namespace joda
#endif    // __frame_main_controller__