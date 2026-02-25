#include "ui_walkie_talkie.hpp"
#include "ui_navigation.hpp"

int main() {
    NavigationView nav;
    nav.push<WalkieTalkieView>();
    
    return 0;
}
