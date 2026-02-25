#include "ui_friendly_talker.hpp" 
#include "friendly_talker.hpp"
#include "portapack.hpp"
#include "audio.hpp"
#include "gpio.hpp"

using namespace portapack;

namespace ui::external_app::friendly_talker {

FriendlyTalkerView::FriendlyTalkerView(NavigationView& nav) {
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
        &button_ptt,
        &label_status,
        &text_status
    });
    
    // Set default values
    field_tx_freq.set_value(446006250);
    field_rx_freq.set_value(446006250);
    field_tx_freq.set_step(6250);
    field_rx_freq.set_step(6250);
    
    progress_volume.set_value(friendly_talker::volume);
    
    // Button handlers
    button_ptt.on_select = [this](Button&) {
        friendly_talker::set_tx_mode();
        button_ptt.set_style(&Styles::red);
        text_status.set("TX");
    };
    
    button_ptt.on_highlight = [this](Button&) {
        friendly_talker::set_rx_mode();
        button_ptt.set_style(&Styles::white);
        text_status.set("RX");
    };
    
    // Same frequency checkbox
    checkbox_same.on_select = [this](Checkbox&, bool v) {
        if (v) {
            field_rx_freq.set_value(field_tx_freq.value());
            field_rx_freq.set_focusable(false);
            friendly_talker::set_frequencies(field_tx_freq.value(), field_tx_freq.value());
        } else {
            field_rx_freq.set_focusable(true);
            friendly_talker::set_frequencies(field_tx_freq.value(), field_rx_freq.value());
        }
    };
    
    // Volume controls
    button_vol_up.on_select = [this](Button&) {
        if (friendly_talker::volume < 10) {
            friendly_talker::volume++;
            friendly_talker::set_volume(friendly_talker::volume);
            progress_volume.set_value(friendly_talker::volume);
        }
    };
    
    button_vol_down.on_select = [this](Button&) {
        if (friendly_talker::volume > 0) {
            friendly_talker::volume--;
            friendly_talker::set_volume(friendly_talker::volume);
            progress_volume.set_value(friendly_talker::volume);
        }
    };
    
    // Frequency change handlers
    field_tx_freq.on_change = [this]() {
        if (checkbox_same.value()) {
            field_rx_freq.set_value(field_tx_freq.value());
            friendly_talker::set_frequencies(field_tx_freq.value(), field_tx_freq.value());
        } else {
            friendly_talker::set_frequencies(field_tx_freq.value(), field_rx_freq.value());
        }
    };
    
    field_rx_freq.on_change = [this]() {
        if (!checkbox_same.value()) {
            friendly_talker::set_frequencies(field_tx_freq.value(), field_rx_freq.value());
        }
    };
    
    // Earphone button GPIO setup
    gpio_mode(GPIO_PIN_EARPHONE_BTN, GPIO_MODE_INPUT);
    gpio_pull_up_down(GPIO_PIN_EARPHONE_BTN, GPIO_PULL_UP);
    
    // Initialize friendly talker
    friendly_talker::init();
    text_status.set("RX");
}

void FriendlyTalkerView::on_earphone_button() {
    bool pressed = !gpio_read(GPIO_PIN_EARPHONE_BTN);  // Active low
    
    if (pressed) {
        friendly_talker::set_tx_mode();
        button_ptt.set_style(&Styles::red);
        text_status.set("TX");
    } else {
        friendly_talker::set_rx_mode();
        button_ptt.set_style(&Styles::white);
        text_status.set("RX");
    }
}

} // namespace
