## Physical layer
set(SOURCE_Layer1
    friendly_talker.cpp
)

## Application layer
set(SOURCE_Layer2
    ui_friendly_talker.cpp
)

## Actual source files used in project
set(SOURCE
    ${SOURCE_Layer1}
    ${SOURCE_Layer2}
)

## Module name
set(COMPONENT friendly_talker)

## Include sources
add_library(${COMPONENT} ${SOURCE})

## Include headers
target_include_directories(${COMPONENT} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

## Link component to main firmware
target_link_libraries(${COMPONENT} PRIVATE app core)

# Register with the main application
if(NOT TARGET application)
    message(FATAL_ERROR "Application target not found")
endif()

target_sources(application PRIVATE ${SOURCE})
target_include_directories(application PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
