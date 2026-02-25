#include "ui_friendly_talker.hpp"
#include "ui_navigation.hpp"

int main() {
    NavigationView nav;
    nav.push<ui::external_app::friendly_talker::FriendlyTalkerView>();
    
    return 0;
}
