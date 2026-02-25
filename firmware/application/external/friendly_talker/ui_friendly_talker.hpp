#ifndef FRIENDLY_TALKER_HPP
#define FRIENDLY_TALKER_HPP

#include "ui.hpp"
#include "ui_widget.hpp"
#include "ui_navigation.hpp"

namespace ui::external_app::friendly_talker {

class WalkieTalkieView : public View {
public:
    WalkieTalkieView(NavigationView& nav);
    
    void focus() override {
        field_tx_freq.focus();
    }
    
private:
    // UI Elements
    Text text_title {
        { 0 * 8, 0, 30 * 8, 16 },
        "WALKIE TALKIE"
    };
    
    Label label_tx {
        { 0 * 8, 2 * 16 },
        "TX:"
    };
    
    FrequencyField field_tx_freq {
        { 4 * 8, 2 * 16 }
    };
    
    Label label_rx {
        { 0 * 8, 4 * 16 },
        "RX:"
    };
    
    FrequencyField field_rx_freq {
        { 4 * 8, 4 * 16 }
    };
    
    Checkbox checkbox_same {
        { 4 * 8, 6 * 16 },
        9,
        "Same as TX"
    };
    
    Label label_volume {
        { 0 * 8, 8 * 16 },
        "Volume:"
    };
    
    ProgressBar progress_volume {
        { 7 * 8, 8 * 16 },
        10 * 8,
        8
    };
    
    Button button_vol_up {
        { 20 * 8, 8 * 16, 3 * 8, 16 },
        "+"
    };
    
    Button button_vol_down {
        { 23 * 8, 8 * 16, 3 * 8, 16 },
        "-"
    };
    
    Button button_ptt {
        { 8 * 8, 11 * 16, 12 * 8, 32 },
        "PTT"
    };
    
    void on_earphone_button();
    
    // GPIO pin for earphone button (adjust based on your hardware)
    static constexpr gpio_pin_t GPIO_PIN_EARPHONE_BTN = gpio_pin_t::GPIO0;
};

} // namespace

#endif
