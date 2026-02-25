#ifndef FRIENDLY_TALKER_HPP
#define FRIENDLY_TALKER_HPP

#include <cstdint>

namespace friendly_talker {

void init();
void set_tx_mode();
void set_rx_mode();
void set_volume(uint8_t level);
void set_frequencies(uint64_t tx, uint64_t rx);

extern bool tx_active;
extern bool rx_active;

} // namespace

#endif

