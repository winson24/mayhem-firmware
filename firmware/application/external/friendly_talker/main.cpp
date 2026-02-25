#include "ui_friendly_talker.hpp"
#include "ui_navigation.hpp"

int main() {
    NavigationView nav;
    nav.push<WalkieTalkieView>();
    
    return 0;
}
