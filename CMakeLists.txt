cmake_minimum_required(VERSION 3.12)

include_directories(${CMAKE_CURRENT_LIST_DIR})

# Set project name
set(NAME PicoSync)

# Include pico sdk
include(pico_sdk_import.cmake)

# Set project
project(${NAME} C CXX)

# Set C/C++ Language revisions
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)


# Set compile flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ffunction-sections -fdata-sections -Wpadded")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -ffunction-sections -fdata-sections -Wpadded ")

set(CMAKE_CXX_FLAGS_RELEASE " -O2 -ffunction-sections -fdata-sections -mcpu=cortex-m0plus")
set(CMAKE_C_FLAGS_RELEASE " -O2 -ffunction-sections -fdata-sections -mcpu=cortex-m0plus")

# Initialize the SDK
pico_sdk_init()
include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
)

add_executable(${NAME}
    # Main
    ${CMAKE_CURRENT_LIST_DIR}/src/main.c
    ${CMAKE_CURRENT_LIST_DIR}/src/CSyncInjection/CSyncInjection.c
)

pico_generate_pio_header(${NAME} ${CMAKE_CURRENT_LIST_DIR}/src/CSync.pio)

# Link Libraries
target_link_libraries(${NAME}
    pico_stdio
    pico_stdlib
    hardware_pio
    hardware_gpio
    pico_multicore
)

# Add link flags
target_link_options(${NAME} PRIVATE -Wl,--gc-sections )

# Set
pico_enable_stdio_usb(${NAME}  1)
pico_enable_stdio_uart(${NAME} 0)

pico_set_binary_type(${NAME} copy_to_ram)

# Create Bin/Hex files
pico_add_extra_outputs(${NAME})
find_program(SIZE_COMMAND NAMES size)

# Check if the size command has been found
if(NOT SIZE_COMMAND)
  message(FATAL_ERROR "Could not find size command")
endif()

# Add a post-build command to print memory usage statistics
add_custom_command(TARGET ${NAME} POST_BUILD 
    COMMAND ${SIZE_COMMAND} $<TARGET_FILE:${NAME}> --format=berkeley)
