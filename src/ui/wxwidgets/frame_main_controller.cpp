#include "frame_main_controller.h"
#include <memory>
#include "ui/wxwidgets/wxwidget.h"

namespace joda {
namespace ui {
namespace wxwidget {

FrameMainController::FrameMainController(wxWindow *parent) : frameMain(parent)
{
  addChannel();
  addChannel();
}

void FrameMainController::addChannel()
{
  auto channel =
      std::make_shared<PanelChannel>(mScrollbarChannels, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

  channel->mScrolledChannel->SetMinSize(wxSize(250, 9999));
  mChannels.push_back(channel);
  mSizerChannels->Insert(mChannels.size() - 1, channel.get(), 0, wxEXPAND | wxALL, 5);
  mSizerChannelsScrollbar->Layout();
  mScrollbarChannels->Layout();
  mSizerChannels->Layout();
}
void FrameMainController::removeChannel()
{
}

void FrameMainController::onAddChannelClicked(wxCommandEvent &event)
{
  addChannel();
}

}    // namespace wxwidget
}    // namespace ui
}    // namespace joda
