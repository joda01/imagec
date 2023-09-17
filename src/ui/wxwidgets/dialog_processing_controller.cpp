#include "dialog_processing_controller.h"

namespace joda::ui::wxwidget {

DialogProcessingController::DialogProcessingController(wxWindow *parent, const joda::settings::json::AnalyzeSettings &,
                                                       wxWindowID id, const wxString &title, const wxPoint &pos,
                                                       const wxSize &size, long style) :
    DialogProcessing(parent, id, title, pos, size, style)
{
}

}    // namespace joda::ui::wxwidget
