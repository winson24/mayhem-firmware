## Physical layer
set(SOURCE_Layer1
    walkie_talkie.cpp
)

## Application layer
set(SOURCE_Layer2
    ui_walkie_talkie.cpp
)

## Actual source files used in project
set(SOURCE
    ${SOURCE_Layer1}
    ${SOURCE_Layer2}
)

## Module name
set(COMPONENT walkie_talkie)

## Include sources
add_library(${COMPONENT} ${SOURCE})

## Include headers
target_include_directories(${COMPONENT} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

## Link component to main firmware
target_link_libraries(${COMPONENT} app)
