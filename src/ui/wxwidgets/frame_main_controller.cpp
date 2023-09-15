#include "frame_main_controller.h"
#include <memory>
#include "ui/wxwidgets/wxwidget.h"

namespace joda::ui::wxwidget {

FrameMainController::FrameMainController(wxWindow *parent) : frameMain(parent)
{
  addChannel();
}

void FrameMainController::addChannel()
{
  auto channel = std::make_shared<PanelChannelController>(mScrollbarChannels, wxID_ANY, wxDefaultPosition,
                                                          wxDefaultSize, wxTAB_TRAVERSAL);
  mSizerChannels->Insert(mChannels.size(), channel.get(), 0, wxEXPAND | wxALL, 5);
  mScrollbarChannels->Layout();
  mSizerChannels->Layout();
  mSizerChannelsScrollbar->Layout();
  this->Layout();
  mChannels.push_back(channel);
}

void FrameMainController::removeChannel()
{
}

void FrameMainController::onAddChannelClicked(wxCommandEvent &event)
{
  addChannel();
}

}    // namespace joda::ui::wxwidget
