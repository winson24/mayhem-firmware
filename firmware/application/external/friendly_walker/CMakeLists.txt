# CMakeLists.txt for Friendly Talker External App
cmake_minimum_required(VERSION 3.10)
project(friendly_talker)

set(APP_NAME "friendly_talker")

# Include the source files
set(SOURCES
    src/friendly_talker.cpp
    src/friendly_talker.hpp
)

# Create the .ppma shared library
add_library(${APP_NAME} SHARED ${SOURCES})
set_target_properties(${APP_NAME} PROPERTIES PREFIX "" SUFFIX ".ppma")

# Link with Mayhem firmware libraries
target_link_libraries(${APP_NAME}
    portapack
    ui
    audio
)

# Add include directories
target_include_directories(${APP_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)