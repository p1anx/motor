cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME motor_cmake)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
        BSP/eeprom/eeprom.c
        BSP/eeprom/eeprom.h
        BSP/eeprom/at24c02.c
        BSP/eeprom/at24c02.h
        BSP/tests/test_main.c
        BSP/tests/test_main.h
        BSP/tests/test_at24c02.c
        BSP/tests/test_at24c02.h
        BSP/common/common.c
        BSP/common/common.h
        BSP/tests/test_mt6835.c
        BSP/key/key.c
        BSP/key/key.h
        BSP/tests/test_motor.c
        BSP/tests/test_lis3mdl.c
        BSP/uart/uart.c
        BSP/uart/uart.h
        BSP/tests/test_esp8266.c
        BSP/esp8266/esp8266.c
        BSP/esp8266/esp8266.h
        BSP/uart/uart_dma.c
        BSP/uart/uart_dma.h
        BSP/uart/uart_polling.c
        BSP/uart/uart_polling.h

)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
        BSP/mt6835/mt6835.c
        BSP/motor/motor.c
        BSP/motor/pwm.c
        BSP/motor/tb6612_init.c
        BSP/pid/pid.c
        BSP/encoder/encoder.c
        BSP/encoder/lis3mdl.c
        BSP/timer/timer_it.c
        BSP/esp8266/esp8266.c
        BSP/tests/test_esp8266.c
        BSP/uart/uart.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
        BSP
        BSP/tests
        BSP/eeprom
        BSP/common
        BSP/mt6835
        BSP/motor
        BSP/timer
        BSP/pid
        BSP/encoder
        BSP/esp8266
        BSP/uart



)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    -u_printf_float
    # Add user defined libraries
)
