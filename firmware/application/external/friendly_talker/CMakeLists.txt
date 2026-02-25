# Add to firmware/application/CMakeLists.txt

add_executable(walkie_talkie.elf
    walkie_talkie_app.cpp
    ui_navigation.cpp
    ui_receiver.cpp
    ui_transmitter.cpp
    audio_output.cpp
    audio_input.cpp
)

target_link_libraries(walkie_talkie.elf
    portapack
    baseband
    hardware
    lpc43xx
)

# Install to SD card
install(TARGETS walkie_talkie.elf
    RUNTIME DESTINATION /apps
    CONFIGURATIONS Release
)