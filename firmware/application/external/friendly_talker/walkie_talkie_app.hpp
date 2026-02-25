/*
 * walkie_talkie_app.hpp
 */

#ifndef __WALKIE_TALKIE_APP_H__
#define __WALKIE_TALKIE_APP_H__

#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "ui_receiver.hpp"
#include "ui_transmitter.hpp"

namespace ui {

class WalkieTalkieView : public View {
public:
    WalkieTalkieView(NavigationView& nav);
    ~WalkieTalkieView();
    
    void focus() override {
        txFrequencyField.focus();
    }
    
    std::string title() const override {
        return "Walkie Talkie";
    }
};

} // namespace ui

#endif/*__WALKIE_TALKIE_APP_H__*/