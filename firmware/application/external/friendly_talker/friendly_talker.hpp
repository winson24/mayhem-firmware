#ifndef FRIENDLY_TALKER_HPP
#define FRIENDLY_TALKER_HPP

#include <cstdint>

namespace friendly_talker {

// Function declarations
void init();
void set_tx_mode();
void set_rx_mode();
void set_volume(uint8_t level);
void set_frequencies(uint64_t tx, uint64_t rx);

// Global state variables (defined in .cpp file)
extern bool tx_active;
extern bool rx_active;
extern uint8_t volume;
extern uint64_t tx_freq;
extern uint64_t rx_freq;

} // namespace friendly_talker

#endif
