cmake_minimum_required(VERSION 3.21)
project(subsdk CXX C ASM)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 17)

## Error if not using switch toolchain file
if (NOT SWITCH)
    message(FATAL_ERROR "Not targeting switch, make sure to specify -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain.cmake")
endif ()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules.cmake)

# Apply user config
if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.template ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake)
endif ()
include(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake)

# Logger
add_custom_target(
        logger
        COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/logger_server.py ${LOGGER_PORT}
)
