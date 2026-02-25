/*
 * HackRF PortaPack - Walkie-Talkie with Earphone Button PTT
 * File: walkie_talkie_app.cpp
 */

#include "ui.hpp"
#include "ui_navigation.hpp"
#include "audio.hpp"
#include "message.hpp"
#include "portapack.hpp"
#include "baseband_api.hpp"
#include "audio_output.hpp"
#include "audio_input.hpp"

using namespace portapack;

namespace ui {

class WalkieTalkieView : public View {
public:
    WalkieTalkieView(NavigationView& nav);
    ~WalkieTalkieView();

private:
    // UI Elements
    Text txFrequencyLabel{
        {0 * 8, 0 * 16, 12 * 8, 16},
        "TX:"
    };
    
    FrequencyField txFrequencyField{
        {4 * 8, 0 * 16}
    };
    
    Text rxFrequencyLabel{
        {0 * 8, 2 * 16, 12 * 8, 16},
        "RX:"
    };
    
    FrequencyField rxFrequencyField{
        {4 * 8, 2 * 16}
    };
    
    Checkbox sameFrequencyCheckbox{
        {4 * 8, 4 * 16},
        9,
        "Same as TX"
    };
    
    OptionsField squelchField{
        {4 * 8, 6 * 16},
        6,
        {
            {"Off", 0},
            {"1", 1},
            {"2", 2},
            {"3", 3},
            {"4", 4},
            {"5", 5}
        }
    };
    
    Labels volumeLabel{
        {0 * 8, 8 * 16},
        "Volume:",
        Color::grey()
    };
    
    ProgressBar volumeProgress{
        {6 * 8, 8 * 16},
        14 * 8,
        8
    };
    
    Button buttonVolumeUp{
        {20 * 8, 8 * 16, 3 * 8, 16},
        "+"
    };
    
    Button buttonVolumeDown{
        {23 * 8, 8 * 16, 3 * 8, 16},
        "-"
    };
    
    Labels statusLabel{
        {0 * 8, 10 * 16},
        "Status:",
        Color::grey()
    };
    
    Text statusText{
        {7 * 8, 10 * 16, 20 * 8, 16},
        "Rx Mode"
    };
    
    Button buttonPTT{
        {5 * 8, 12 * 16, 15 * 8, 40},
        "PTT (Hold)"
    };
    
    // Audio processing
    AudioOutput audio_output{};
    AudioInput audio_input{};
    
    // State variables
    bool rx_enabled = true;
    bool tx_enabled = false;
    uint8_t volume_level = 5;
    uint8_t squelch_level = 2;
    bool earphone_ptt_state = false;
    
    // Navigation callback
    NavigationView& nav_;
    
    // Message handler registration
    std::unique_ptr<MessageHandlerRegistration> message_handler_rxdata;
    
    // Methods
    void update_status();
    void set_rx_frequency(uint64_t freq);
    void set_tx_frequency(uint64_t freq);
    void start_rx();
    void stop_rx();
    void start_tx();
    void stop_tx();
    void on_ptt_pressed();
    void on_ptt_released();
    void on_earphone_button_change(bool pressed);
    void update_volume_display();
    
    // GPIO interrupt handler for earphone button
    static void earphone_button_isr(void* context);
};

// GPIO Pin definitions for earphone button (adjust based on your hardware)
constexpr gpio_pin_t EARPHONE_BUTTON_PIN = gpio_pin_t::GPIO0;  // Example pin

WalkieTalkieView::WalkieTalkieView(NavigationView& nav) : nav_(nav) {
    add_children({
        &txFrequencyLabel,
        &txFrequencyField,
        &rxFrequencyLabel,
        &rxFrequencyField,
        &sameFrequencyCheckbox,
        &squelchField,
        &volumeLabel,
        &volumeProgress,
        &buttonVolumeUp,
        &buttonVolumeDown,
        &statusLabel,
        &statusText,
        &buttonPTT
    });
    
    // Initialize frequencies
    txFrequencyField.set_value(446006250); // Default PMR frequency
    rxFrequencyField.set_value(446006250);
    
    // Set frequency step
    txFrequencyField.set_step(6250);
    rxFrequencyField.set_step(6250);
    
    // Initialize squelch
    squelchField.set_selected_index(2);
    
    // Update UI
    update_volume_display();
    start_rx();
    
    // Button handlers
    sameFrequencyCheckbox.on_select = [this](Checkbox&, bool value) {
        if (value) {
            rxFrequencyField.set_value(txFrequencyField.value());
            rxFrequencyField.set_focusable(false);
        } else {
            rxFrequencyField.set_focusable(true);
        }
    };
    
    buttonVolumeUp.on_select = [this](Button&) {
        if (volume_level < 10) {
            volume_level++;
            update_volume_display();
            audio_output.set_volume(volume_level * 10);
        }
    };
    
    buttonVolumeDown.on_select = [this](Button&) {
        if (volume_level > 0) {
            volume_level--;
            update_volume_display();
            audio_output.set_volume(volume_level * 10);
        }
    };
    
    buttonPTT.on_select = [this](Button&) {
        on_ptt_pressed();
    };
    
    buttonPTT.on_highlight = [this](Button&) {
        on_ptt_released();
    };
    
    // Initialize earphone button
    gpio_mode(EARPHONE_BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_pull_up_down(EARPHONE_BUTTON_PIN, GPIO_PULL_UP);
    
    // Configure interrupt for earphone button
    gpio_interrupt_config(EARPHONE_BUTTON_PIN, GPIO_INTERRUPT_FALLING | GPIO_INTERRUPT_RISING);
    gpio_interrupt_callback(EARPHONE_BUTTON_PIN, earphone_button_isr, this);
    
    // Message handler for received audio
    message_handler_rxdata = std::make_unique<MessageHandlerRegistration>(
        Message::ID::AudioData,
        [this](Message* msg) {
            auto audio_msg = static_cast<AudioDataMessage*>(msg);
            if (rx_enabled && !tx_enabled) {
                audio_output.write(audio_msg->samples, audio_msg->count);
            }
        }
    );
}

WalkieTalkieView::~WalkieTalkieView() {
    stop_rx();
    stop_tx();
    gpio_interrupt_disable(EARPHONE_BUTTON_PIN);
}

void WalkieTalkieView::earphone_button_isr(void* context) {
    auto view = static_cast<WalkieTalkieView*>(context);
    bool button_pressed = !gpio_read(EARPHONE_BUTTON_PIN); // Active low
    
    // Use portapack's event queue for thread safety
    EventDispatcher::call_sync([view, button_pressed]() {
        view->on_earphone_button_change(button_pressed);
    });
}

void WalkieTalkieView::on_earphone_button_change(bool pressed) {
    if (pressed && !earphone_ptt_state) {
        // Button pressed
        earphone_ptt_state = true;
        on_ptt_pressed();
    } else if (!pressed && earphone_ptt_state) {
        // Button released
        earphone_ptt_state = false;
        on_ptt_released();
    }
}

void WalkieTalkieView::on_ptt_pressed() {
    if (!tx_enabled) {
        stop_rx();
        start_tx();
        statusText.set("Tx Mode (PTT)");
        buttonPTT.set_style(&Styles::red);
    }
}

void WalkieTalkieView::on_ptt_released() {
    if (tx_enabled) {
        stop_tx();
        start_rx();
        statusText.set("Rx Mode");
        buttonPTT.set_style(&Styles::white);
    }
}

void WalkieTalkieView::start_rx() {
    if (rx_enabled) return;
    
    rx_enabled = true;
    
    // Configure receiver
    baseband::set_mode(baseband::Mode::Capture);
    baseband::set_frequency(rxFrequencyField.value());
    
    // Configure audio routing
    audio_output.start();
    audio_output.set_volume(volume_level * 10);
    
    // Configure squelch
    baseband::set_squelch_level(squelch_level);
}

void WalkieTalkieView::stop_rx() {
    if (!rx_enabled) return;
    
    rx_enabled = false;
    audio_output.stop();
    baseband::set_mode(baseband::Mode::Idle);
}

void WalkieTalkieView::start_tx() {
    if (tx_enabled) return;
    
    tx_enabled = true;
    
    // Configure transmitter
    baseband::set_mode(baseband::Mode::Transmit);
    baseband::set_frequency(txFrequencyField.value());
    
    // Start audio input for microphone
    audio_input.start();
}

void WalkieTalkieView::stop_tx() {
    if (!tx_enabled) return;
    
    tx_enabled = false;
    audio_input.stop();
    baseband::set_mode(baseband::Mode::Idle);
}

void WalkieTalkieView::update_volume_display() {
    volumeProgress.set_value(volume_level);
}

void WalkieTalkieView::update_status() {
    // Update status display
}

void WalkieTalkieView::set_rx_frequency(uint64_t freq) {
    rxFrequencyField.set_value(freq);
    if (rx_enabled && !tx_enabled) {
        baseband::set_frequency(freq);
    }
}

void WalkieTalkieView::set_tx_frequency(uint64_t freq) {
    txFrequencyField.set_value(freq);
    if (sameFrequencyCheckbox.value()) {
        set_rx_frequency(freq);
    }
}

} // namespace ui

// Main application entry point
int main() {
    EventDispatcher event_dispatcher;
    
    // Initialize hardware
    portapack::init();
    
    // Create and show the walkie talkie view
    NavigationView nav;
    nav.push<WalkieTalkieView>();
    
    // Run the application
    event_dispatcher.run();
    
    return 0;
}