#include "ui.hpp"
#include "friendly_walker.hpp"
#include "ui_navigation.hpp"
#include "external_app.hpp"

namespace ui::external_app::friendly_walker {
void initialize_app(ui::NavigationView& nav) {
    nav.push<FriendlyWalkerHeadsetView>();
}
}  // namespace ui::external_app::friendly_walker

extern "C" {

__attribute__((section(".external_app.app_friendly_walker.application_information"), used)) application_information_t _application_information_friendly_walker = {
    /*.memory_location = */ (uint8_t*)0x00000000,
    /*.externalAppEntry = */ ui::external_app::friendly_walker::initialize_app,
    /*.header_version = */ CURRENT_HEADER_VERSION,
    /*.app_version = */ VERSION_MD5,

    /*.app_name = */ "Friendly Walker",
    /*.bitmap_data = */ {
        0x00, 0x00, 0x00, 0x00,
        0xC0, 0x03, 0xE0, 0x07,
        0xF0, 0x0F, 0xF8, 0x1F,
        0xFC, 0x3F, 0xFE, 0x7F,
        0xFE, 0x7F, 0xFC, 0x3F,
        0xF8, 0x1F, 0xF0, 0x0F,
        0xE0, 0x07, 0xC0, 0x03,
        0x00, 0x00, 0x00, 0x00,
    },
    /*.icon_color = */ ui::Color::green().v,
    /*.menu_location = */ app_location_t::TX,
    /*.desired_menu_position = */ -1,

    /*.m4_app_tag = */ {'P', 'N', 'F', 'M'},
    /*.m4_app_offset = */ 0x00000000,  // will be filled at compile time
};
}