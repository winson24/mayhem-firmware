#include "ui_friendly_talker.hpp"
#include "friendly_talker.hpp"
#include "portapack.hpp"
#include "audio.hpp"
#include "gpio.hpp"

using namespace portapack;

namespace ui::external_app::friendly_talker {

WalkieTalkieView::WalkieTalkieView(NavigationView& nav) {
    add_children({
        &text_title,
        &label_tx,
        &field_tx_freq,
        &label_rx,
        &field_rx_freq,
        &checkbox_same,
        &label_volume,
        &progress_volume,
        &button_vol_up,
        &button_vol_down,
        &button_ptt
    });
    
    // Set default values
    field_tx_freq.set_value(446006250);
    field_rx_freq.set_value(446006250);
    
    // Button handlers
    button_ptt.on_select = [this](Button&) {
        friendly_talker::set_tx_mode();
        button_ptt.set_style(&Styles::red);
    };
    
    button_ptt.on_highlight = [this](Button&) {
        friendly_talker::set_rx_mode();
        button_ptt.set_style(&Styles::white);
    };
    
    // Same frequency checkbox
    checkbox_same.on_select = [this](Checkbox&, bool v) {
        if (v) {
            field_rx_freq.set_value(field_tx_freq.value());
            field_rx_freq.set_focusable(false);
        } else {
            field_rx_freq.set_focusable(true);
        }
    };
    
    // Volume buttons
    button_vol_up.on_select = [this](Button&) {
        if (friendly_talker::volume < 10) {
            friendly_talker::set_volume(friendly_talker::volume + 1);
            progress_volume.set_value(friendly_talker::volume * 10);
        }
    };
    
    button_vol_down.on_select = [this](Button&) {
        if (friendly_talker::volume > 0) {
            friendly_talker::set_volume(friendly_talker::volume - 1);
            progress_volume.set_value(friendly_talker::volume * 10);
        }
    };
    
    // Earphone button GPIO setup
    gpio_mode(GPIO_PIN_EARPHONE_BTN, GPIO_MODE_INPUT);
    gpio_pull_up_down(GPIO_PIN_EARPHONE_BTN, GPIO_PULL_UP);
    
    // Initialize walkie-talkie
    friendly_talker::init();
    
    // Set initial volume display
    progress_volume.set_value(friendly_talker::volume * 10);
}

void WalkieTalkieView::on_earphone_button() {
    bool pressed = !gpio_read(GPIO_PIN_EARPHONE_BTN);  // Active low
    
    if (pressed) {
        friendly_talker::set_tx_mode();
    } else {
        friendly_talker::set_rx_mode();
    }
}

} // namespace
