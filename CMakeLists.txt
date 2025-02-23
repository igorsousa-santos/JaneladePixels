# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(JanelaDePixels C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(JanelaDePixels 
    JanelaDePixels.c
    libs/ssd1306/ssd1306_i2c.c
    modules/OLEDDisplay.c
    modules/updateTime.c
    modules/ws2812b.c
)

pico_set_program_name(JanelaDePixels "JanelaDePixels")
pico_set_program_version(JanelaDePixels "0.1")

# Generate PIO header
pico_generate_pio_header(JanelaDePixels ${CMAKE_CURRENT_LIST_DIR}/modules/ws2812b.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(JanelaDePixels 0)
pico_enable_stdio_usb(JanelaDePixels 1)

# Add the standard library to the build
target_link_libraries(JanelaDePixels
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(JanelaDePixels PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/libs/ssd1306
    ${CMAKE_CURRENT_LIST_DIR}/modules
    ${CMAKE_CURRENT_LIST_DIR}/images
)

# Add any user requested libraries
target_link_libraries(JanelaDePixels 
    pico_stdlib
    hardware_i2c
    hardware_rtc 
    hardware_pio
    hardware_clocks
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip
    pico_lwip_sntp
)


add_definitions(
    # SNTP Configurations
    -DSNTP_SERVER_DNS=1
    -DSNTP_SERVER_ADDRESS="pool.ntp.org"
    -DSNTP_SET_SYSTEM_TIME=setSystemTimeCallback
    -DSNTP_STARTUP_DELAY=0
)

pico_add_extra_outputs(JanelaDePixels)

