set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# Default compile definitions
add_compile_definitions(
        LOGGER_IP="${LOGGER_IP}"
        LOGGER_PORT=${LOGGER_PORT}
)

# Include nx tools
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/switch/SwitchTools.cmake)

# Required for exlaunch
set(CONFIG_TITLE_ID 0x${TITLE_ID})
add_compile_definitions(EXL_PROGRAM_ID=${CONFIG_TITLE_ID} NNSDK=1)
