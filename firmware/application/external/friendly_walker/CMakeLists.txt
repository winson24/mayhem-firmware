# For external app compilation as .ppma
cmake_minimum_required(VERSION 3.15)

# This must match your app name
set(APP_NAME friendly_walker)

set(SOURCES
    main.cpp
    friendly_walker_headset.cpp
)

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../common
)

# This macro creates both the external app and .ppma target
add_external_app(${APP_NAME}
    SOURCES ${SOURCES}
    INCLUDE_DIRS ${INCLUDE_DIRS}
)
