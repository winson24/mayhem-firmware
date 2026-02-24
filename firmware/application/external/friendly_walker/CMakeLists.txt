set(SOURCES
    main.cpp
    friendly_walker_headset.cpp
)

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_external_app(friendly_walker
    SOURCES ${SOURCES}
    INCLUDE_DIRS ${INCLUDE_DIRS}
)