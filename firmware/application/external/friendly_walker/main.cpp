#include "ui.hpp"
#include "friendly_walker.hpp"
#include "external_app.hpp"

namespace ui::external_app::friendly_walker {
void initialize_app(ui::NavigationView& nav) {
    nav.push<FriendlyWalkerHeadsetView>();
}
}  // namespace ui::external_app::friendly_walker

// THIS LINE IS CRITICAL for .ppma generation
APPLICATION_EXTERNAL(friendly_walker)
