#ifndef __FRIENDLY_TALKER_HPP__
#define __FRIENDLY_TALKER_HPP__

#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "ui_receiver.hpp"
#include "ui_transmitter.hpp"
#include "rf_path.hpp"

namespace ui {

class FriendlyTalkerView : public View {
public:
    FriendlyTalkerView(NavigationView& nav);
    ~FriendlyTalkerView();

    void focus() override;

    std::string title() const override { return "Friendly Talkie"; }

private:
    void on_frequency_changed(rf::Frequency f);
    void set_tx(bool tx);
    void update_vumeter();

    // UI Components
    Labels labels {
        { { 16, 16 }, "VFO FREQUENCY:", Color::light_grey() },
        { { 16, 88 }, "SIGNAL STRENGTH", Color::light_grey() }
    };

    BigFrequency field_frequency {
        { 16, 32 },
        rf::Frequency(446006250) // Default PMR446 Ch1
    };

    ProgressBar progress_signal {
        { 16, 112, 208, 24 }
    };

    Button button_ptt {
        { 60, 200, 120, 60 },
        "PTT"
    };

    Text text_status {
        { 80, 270, 80, 20 },
        "IDLE",
        Color::grey()
    };

    // Radio logic
    bool transmitting = false;
    uint32_t sampling_rate = 1536000;
};

} /* namespace ui */

#endif /* __FRIENDLY_TALKER_HPP__ */
