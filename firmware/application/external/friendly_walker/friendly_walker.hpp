#ifndef __FRIENDLY_WALKER_H__
#define __FRIENDLY_WALKER_H__

#include "ui.hpp"
#include "ui_navigation.hpp"
#include "ui_freq_field.hpp"
#include "baseband_api.hpp"
#include "audio.hpp"
#include "portapack.hpp"

namespace ui::external_app {
namespace friendly_walker {

class FriendlyWalkerHeadsetView : public View {
public:
    FriendlyWalkerHeadsetView(NavigationView& nav);
    ~FriendlyWalkerHeadsetView();

    void focus() override;
    void paint(Painter& painter) override;

    // Called by the app framework on every UI tick (~50 ms)
    void frame_sync();

private:
    // ── UI Elements ────────────────────────────────────────────
    Text title_text{
        {0, 0, 240, 16},
        "FRIENDLY WALKER"
    };

    Text headset_status{
        {20, 18, 200, 16},
        "Headset: Not Detected"
    };

    FrequencyField field_frequency{
        {40, 38, 160, 32}
    };

    OptionsField options_step{
        {40, 74},
        10,
        {
            {"1 kHz",    1000},
            {"5 kHz",    5000},
            {"6.25 kHz", 6250},
            {"10 kHz",  10000},
            {"12.5 kHz",12500},
            {"25 kHz",  25000},
        }
    };

    Button button_2m{
        {20, 96, 58, 26},
        "2m"
    };
    Button button_70cm{
        {90, 96, 58, 26},
        "70cm"
    };
    Button button_custom{
        {160, 96, 58, 26},
        "PMR"
    };

    OptionsField input_source{
        {40, 130},
        12,
        {
            {"Built-in Mic", 0},
            {"Headset Mic",  1},
        }
    };

    OptionsField ptt_mode{
        {40, 150},
        14,
        {
            {"Screen PTT",     0},
            {"Headset Button", 1},
        }
    };

    ProgressBar progress_rssi{
        {40, 174, 160, 12}
    };

    Text text_signal{
        {40, 188, 160, 14},
        "Signal: -- dBm"
    };

    Text text_status{
        {20, 208, 100, 16},
        "Status: RX"
    };

    Text text_mode{
        {130, 208, 90, 16},
        "FM Narrow"
    };

    Button button_ptt{
        {20, 230, 200, 50},
        ">> PTT (Push to Talk)"
    };

    Button button_exit{
        {80, 288, 80, 26},
        "Exit"
    };

    // ── Internal state ─────────────────────────────────────────
    bool is_transmitting        = false;
    bool headset_detected       = false;
    bool headset_button_pressed = false;
    rf::Frequency current_frequency = 145000000;
    int32_t rssi                = -120;

    // ── Debounce ───────────────────────────────────────────────
    uint32_t last_button_state = 0;
    uint32_t debounce_ticks    = 0;
    static constexpr uint32_t DEBOUNCE_TICKS = 1;  // 1 tick × 50 ms = 50 ms

    // ── Tick counter (for periodic updates driven by frame_sync) ──
    uint32_t tick_counter = 0;

    // ── Private methods ────────────────────────────────────────
    void on_ptt_toggle();
    void on_headset_button_change(bool pressed);
    void start_tx();
    void stop_tx();
    void update_rssi();
    void set_frequency(rf::Frequency f);
    void check_headset_status();
    void setup_audio_routing();
    void poll_headset_button();
};

} // namespace friendly_walker
} // namespace ui::external_app

#endif // __FRIENDLY_WALKER_H__