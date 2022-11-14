cmake_minimum_required(VERSION 3.21)
project(subsdk CXX C ASM)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 17)

## Error if not using switch toolchain file
if (NOT SWITCH)
    message(FATAL_ERROR "Not targeting switch, make sure to specify -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain.cmake")
endif ()

# Apply user config
if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.template ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake)
endif ()
include(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake)

# Set up CMake
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/common.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/functions.cmake)

# Set up each target
add_subsdk_target(
        subsdk9
        ${MOD_NAME}
        ${TITLE_ID}
        ${GAME_VERSION}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/common/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/common/lib/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/common/nn/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/subsdk9/
)

# Logger
add_custom_target(
        logger
        COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/logger_server.py ${LOGGER_PORT}
)
