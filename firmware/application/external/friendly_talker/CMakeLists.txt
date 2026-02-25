## Source files
set(SOURCE
    main.cpp
    friendly_talker.cpp
    ui_friendly_talker.cpp
)

## Create executable (this makes the .ppma file)
add_executable(friendly_talker ${SOURCE})

## Link with required PortaPack libraries
target_link_libraries(friendly_talker
    app        # Main application framework
    ui         # UI widgets
    portapack  # Core functionality
    hardware   # Hardware access
    audio      # Audio support
    baseband   # Baseband processing
)

## Set output properties for standalone app
set_target_properties(friendly_talker PROPERTIES
    OUTPUT_NAME "friendly_talker"
    SUFFIX ".ppma"
    PREFIX ""
)

## Include directories
target_include_directories(friendly_talker PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

## Install to standalone directory
install(TARGETS friendly_talker
    RUNTIME DESTINATION standalone
)
