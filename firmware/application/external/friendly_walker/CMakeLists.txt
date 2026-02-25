# CMakeLists.txt for Mayhem External App (Friendly Talker)
cmake_minimum_required(VERSION 3.10)
project(friendly_talker)

set(APP_NAME "FriendlyTalker")

# Include the source files
set(SOURCES
    src/friendly_talker.cpp
    src/friendly_talker.hpp
)

# This assumes the Mayhem build environment provides the necessary includes
# and the 'add_external_app' or equivalent macro.
# For standalone builds outside the full firmware, we follow the MDK pattern.

add_library(${APP_NAME} SHARED ${SOURCES})
set_target_properties(${APP_NAME} PROPERTIES SUFFIX ".ppma")

# Add any required defines or include paths
target_include_directories(${APP_NAME} PRIVATE src)
