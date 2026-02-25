#include "friendly_talker.hpp"
#include "audio.hpp"
#include "portapack.hpp"

namespace friendly_talker {

bool tx_active = false;
bool rx_active = true;
uint8_t volume = 5;
uint64_t tx_freq = 446006250;  // PMR channel 1
uint64_t rx_freq = 446006250;

void init() {
    audio::init();
    set_rx_mode();
}

void set_tx_mode() {
    if (rx_active) {
        audio::stop();
        rx_active = false;
    }
    if (!tx_active) {
        // Set to transmit frequency
        portapack::set_frequency(tx_freq);
        audio::start_tx();
        tx_active = true;
    }
}

void set_rx_mode() {
    if (tx_active) {
        audio::stop_tx();
        tx_active = false;
    }
    if (!rx_active) {
        // Set to receive frequency
        portapack::set_frequency(rx_freq);
        audio::start_rx();
        rx_active = true;
    }
}

void set_volume(uint8_t level) {
    volume = level;
    audio::set_volume(volume * 10);
}

void set_frequencies(uint64_t tx, uint64_t rx) {
    tx_freq = tx;
    rx_freq = rx;
}

} // namespace
