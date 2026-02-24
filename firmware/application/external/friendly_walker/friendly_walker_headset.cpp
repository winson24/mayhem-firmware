/*
 * Friendly Walker - With Headset PTT Button & Mic Support
 * For PortaPack Mayhem Firmware (External App)
 *
 * Features:
 * - Use headset button as PTT (Push-to-Talk)
 * - Automatic headset detection
 * - Audio routing to/from headset
 * - Debounced button handling
 * - Visual feedback for headset mode
 */

#include "friendly_walker.hpp"
#include "ui_freq_field.hpp"
#include "baseband_api.hpp"
#include "audio.hpp"
#include "portapack_shared_memory.hpp"
#include "portapack_persistent_memory.hpp"
#include "portapack.hpp"
#include "string_format.hpp"

using namespace portapack;

namespace ui::external_app {
namespace friendly_walker {

// ─── Headset button GPIO ──────────────────────────────────────────────────────
//
// On PortaPack hardware the 3.5 mm TRRS headset button shorts the ring to
// ground.  The ring is connected to GPIO1[6] (P8 in the gpio_temp/gpio_dir
// arrays).  We read the raw LPC43xx GPIO port.
//
// If your board uses a different pin, change GPIO_PORT / GPIO_PIN below.
//
#include "lpc43xx_cpp.hpp"
using namespace lpc43xx;

static constexpr gpio::GPIO HEADSET_BTN_GPIO = gpio::GPIO{1, 6};

// ─── Constructor ──────────────────────────────────────────────────────────────

FriendlyWalkerHeadsetView::FriendlyWalkerHeadsetView(NavigationView& nav) {
    add_children({
        &title_text,
        &headset_status,
        &field_frequency,
        &options_step,
        &button_2m,
        &button_70cm,
        &button_custom,
        &input_source,
        &ptt_mode,
        &progress_rssi,
        &text_signal,
        &text_status,
        &text_mode,
        &button_ptt,
        &button_exit
    });

    // Style the title
    title_text.set_style(Theme::getInstance()->title);

    // Configure frequency field
    field_frequency.set_step(5000);
    field_frequency.set_value(current_frequency);

    // Step size selector
    options_step.on_change = [this](size_t, int32_t value) {
        field_frequency.set_step(value);
    };

    // Band preset buttons
    button_2m.on_select = [this](Button&) {
        set_frequency(145000000);   // 145.000 MHz – 2m amateur
    };

    button_70cm.on_select = [this](Button&) {
        set_frequency(433920000);   // 433.920 MHz – 70cm
    };

    button_custom.on_select = [this](Button&) {
        set_frequency(446006250);   // 446.006 MHz – PMR446
    };

    // Input source selector
    input_source.on_change = [this](size_t, int32_t value) {
        setup_audio_routing();
        headset_status.set(value == 1 ? "Headset: Active" : "Headset: Not Detected");
    };

    // PTT mode selector
    ptt_mode.on_change = [this](size_t, int32_t value) {
        if (value == 1) {
            button_ptt.hidden(true);
            headset_status.set("Headset: Use Button for PTT");
        } else {
            button_ptt.hidden(false);
            headset_status.set("Headset: Not Detected");
        }
    };

    // Screen PTT button
    button_ptt.on_select = [this](Button&) {
        if (ptt_mode.selected_index() == 0) {
            on_ptt_toggle();
        }
    };

    // Exit button
    button_exit.on_select = [&nav](Button&) {
        nav.pop();
    };

    // Configure headset GPIO pin as input (direction = 0)
    HEADSET_BTN_GPIO.mode(gpio::InputMode::Passive);

    // Start in receive mode (Narrowband FM)
    receiver_model.set_target_frequency(current_frequency);
    receiver_model.set_modulation(ReceiverModel::Mode::NarrowbandFMAudio);
    receiver_model.set_nbfm_configuration(1);   // 12.5 kHz bandwidth
    receiver_model.enable();

    audio::output::start();
    setup_audio_routing();
}

// ─── Destructor ───────────────────────────────────────────────────────────────

FriendlyWalkerHeadsetView::~FriendlyWalkerHeadsetView() {
    stop_tx();
    audio::output::stop();
    receiver_model.disable();
}

// ─── Focus ────────────────────────────────────────────────────────────────────

void FriendlyWalkerHeadsetView::focus() {
    field_frequency.focus();
}

// ─── Paint (background fill) ──────────────────────────────────────────────────

void FriendlyWalkerHeadsetView::paint(Painter& painter) {
    painter.fill_rectangle(
        {0, 0, 240, 320},
        Theme::getInstance()->bg_dark->background
    );
}

// ─── frame_sync ───────────────────────────────────────────────────────────────
//
// Called by the UI framework on every display refresh (~60 Hz).
// We only need ~20 Hz for RSSI and button polling so we divide down.

void FriendlyWalkerHeadsetView::frame_sync() {
    tick_counter++;
    if (tick_counter < 3) return;   // ~50 ms @ 60 fps
    tick_counter = 0;

    check_headset_status();
    update_rssi();

    if (ptt_mode.selected_index() == 1) {
        poll_headset_button();
    }
}

// ─── Headset detection ────────────────────────────────────────────────────────

void FriendlyWalkerHeadsetView::check_headset_status() {
    bool detected = (input_source.selected_index() == 1);

    if (detected != headset_detected) {
        headset_detected = detected;
        headset_status.set_style(
            detected ? &Theme::getInstance()->fg_green
                     : &Theme::getInstance()->fg_red
        );
    }
}

// ─── GPIO headset button polling ─────────────────────────────────────────────

void FriendlyWalkerHeadsetView::poll_headset_button() {
    // Button pressed = pin pulled LOW
    uint32_t current_state = HEADSET_BTN_GPIO.read() ? 0u : 1u;

    if (current_state != last_button_state) {
        // State changed – restart debounce
        last_button_state = current_state;
        debounce_ticks    = 0;
    } else {
        if (debounce_ticks <= DEBOUNCE_TICKS) {
            debounce_ticks++;
        }

        // Stable for long enough → act on it
        if (debounce_ticks == DEBOUNCE_TICKS) {
            bool pressed = (current_state == 1);
            if (pressed != headset_button_pressed) {
                headset_button_pressed = pressed;
                on_headset_button_change(pressed);
            }
        }
    }
}

// ─── Headset button state change ─────────────────────────────────────────────

void FriendlyWalkerHeadsetView::on_headset_button_change(bool pressed) {
    if (pressed) {
        // Button down → begin transmitting
        if (!is_transmitting) start_tx();
    } else {
        // Button released → stop transmitting
        if (is_transmitting) stop_tx();
    }
}

// ─── Audio routing ────────────────────────────────────────────────────────────

void FriendlyWalkerHeadsetView::setup_audio_routing() {
    if (input_source.selected_index() == 1) {
        // Headset mic
        audio::headphone::set_volume(volume_t::decibel(0));
        audio::set_rate(audio::Rate::Rate_48000);
    } else {
        // Built-in mic (default)
        audio::set_rate(audio::Rate::Rate_48000);
    }
}

// ─── PTT toggle ───────────────────────────────────────────────────────────────

void FriendlyWalkerHeadsetView::on_ptt_toggle() {
    if (is_transmitting) stop_tx();
    else                 start_tx();
}

// ─── Start TX ─────────────────────────────────────────────────────────────────

void FriendlyWalkerHeadsetView::start_tx() {
    is_transmitting = true;

    if (ptt_mode.selected_index() == 0) {
        button_ptt.set_text(">> TRANSMITTING...");
        button_ptt.set_style(&Theme::getInstance()->warning);
    }

    text_status.set("Status: TX");
    text_status.set_style(&Theme::getInstance()->warning);

    // Switch from RX to TX
    audio::output::stop();
    receiver_model.disable();

    current_frequency = field_frequency.value();
    transmitter_model.set_target_frequency(current_frequency);

    // Correct Mayhem enum for narrowband FM TX
    transmitter_model.set_modulation(TransmitterModel::Mode::NarrowbandFMAudio);
    transmitter_model.set_rf_amp(false);   // No amp – keep it legal
    transmitter_model.enable();

    audio::input::start();
}

// ─── Stop TX ──────────────────────────────────────────────────────────────────

void FriendlyWalkerHeadsetView::stop_tx() {
    if (!is_transmitting) return;   // Guard: don't call disable() when already RX
    is_transmitting = false;

    if (ptt_mode.selected_index() == 0) {
        button_ptt.set_text(">> PTT (Push to Talk)");
        button_ptt.set_style(&Theme::getInstance()->primary);
    }

    text_status.set("Status: RX");
    text_status.set_style(&Theme::getInstance()->fg_green);

    // Switch back to RX
    audio::input::stop();
    transmitter_model.disable();

    receiver_model.set_target_frequency(current_frequency);
    receiver_model.enable();
    audio::output::start();
}

// ─── RSSI update ──────────────────────────────────────────────────────────────
//
// In Mayhem, RSSI is exposed via shared_memory not via receiver_model.rssi().
// The value is stored as a raw 10-bit ADC count; we convert to approx. dBm.

void FriendlyWalkerHeadsetView::update_rssi() {
    if (is_transmitting) return;

    // Read from baseband shared memory (correct Mayhem API)
    const auto rssi_raw = shared_memory.bb_data.trig.rssi;

    // Map raw 0–1023 → approximately -120 dBm … -20 dBm
    rssi = -120 + static_cast<int32_t>(rssi_raw) * 100 / 1023;

    // Scale to 0…100 % for the progress bar
    int32_t scaled = (rssi + 120) * 100 / 100;
    if (scaled < 0)   scaled = 0;
    if (scaled > 100) scaled = 100;

    progress_rssi.set_value(scaled);
    text_signal.set("Signal: " + to_string_dec_int(rssi) + " dBm");

    if (rssi > -70)       text_signal.set_style(&Theme::getInstance()->fg_green);
    else if (rssi > -90)  text_signal.set_style(&Theme::getInstance()->fg_yellow);
    else                  text_signal.set_style(&Theme::getInstance()->fg_red);
}

// ─── Set frequency ────────────────────────────────────────────────────────────

void FriendlyWalkerHeadsetView::set_frequency(rf::Frequency f) {
    current_frequency = f;
    field_frequency.set_value(f);

    if (!is_transmitting) {
        receiver_model.set_target_frequency(f);
    }
}

} // namespace friendly_walker
} // namespace ui::external_app