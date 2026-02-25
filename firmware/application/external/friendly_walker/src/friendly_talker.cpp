#include "friendly_talker.hpp"
#include "portapack.hpp"
#include "audio.hpp"
#include "string_format.hpp"

using namespace portapack;

namespace ui
{

    FriendlyTalkerView::FriendlyTalkerView(NavigationView &nav)
    {
        add_children({&labels,
                      &field_frequency,
                      &progress_signal,
                      &button_ptt,
                      &text_status});

        field_frequency.on_change = [this](rf::Frequency f)
        {
            this->on_frequency_changed(f);
        };

        button_ptt.on_select = [this](Button &)
        {
            this->set_tx(!this->transmitting);
        };

        // Initial setup
        on_frequency_changed(field_frequency.value());
        set_tx(false);
    }

    FriendlyTalkerView::~FriendlyTalkerView()
    {
        set_tx(false);
        receiver_model.disable();
        transmitter_model.disable();
    }

    void FriendlyTalkerView::focus()
    {
        field_frequency.focus();
    }

    void FriendlyTalkerView::on_frequency_changed(rf::Frequency f)
    {
        receiver_model.set_target_frequency(f);
        transmitter_model.set_target_frequency(f);
    }

    void FriendlyTalkerView::set_tx(bool tx)
    {
        if (tx)
        {
            // Switch to Transmit
            receiver_model.disable();

            transmitter_model.set_sampling_rate(sampling_rate);
            transmitter_model.set_baseband_bandwidth(1750000);
            transmitter_model.set_rf_amp(true);
            transmitter_model.set_lna(40);
            transmitter_model.set_vga(40);

            // Microphone and Audio setup
            audio::input::start();

            transmitter_model.enable();

            text_status.set("TX MODE");
            text_status.set_style(&style_text_red);
            button_ptt.set_text("RELEASE");
        }
        else
        {
            // Switch to Receive
            transmitter_model.disable();
            audio::input::stop();

            receiver_model.set_sampling_rate(sampling_rate);
            receiver_model.set_baseband_bandwidth(1750000);
            receiver_model.set_modulation(ReceiverModel::Mode::NarrowbandFM);
            receiver_model.set_squelch(20);

            receiver_model.enable();

            text_status.set("RX MODE");
            text_status.set_style(&style_text_green);
            button_ptt.set_text("PTT");
        }
        transmitting = tx;
    }

    void FriendlyTalkerView::update_vumeter()
    {
        if (!transmitting)
        {
            float rssi = receiver_model.level();
            progress_signal.set_value(rssi * 100);
        }
    }

} /* namespace ui */