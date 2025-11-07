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
set(CMAKE_PROJECT_NAME stm32h750vbt6_cmake)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
        BSP/led/key.c
        BSP/stm32_hal.c
        BSP/stm32_hal.h
        BSP/led/led.c
        BSP/led/led.h
        BSP/mymain.c
        BSP/mymain.h
        BSP/test/test_ads1256.c
        BSP/FOC/currentSense/ads1256.h
        BSP/FOC/currentSense/ads1256.c
        BSP/FOC/BLDCMotor.h
        BSP/FOC/BLDCMotor.c
        BSP/FOC/BLDCDriver/BLDCDriver.h
        BSP/FOC/BLDCDriver/BLDCDriver.c
        BSP/FOC/encoder/as5600.h
        BSP/FOC/encoder/as5600.c
        BSP/FOC/encoder/encoder.h
        BSP/FOC/encoder/encoder.c
        BSP/FOC/encoder/encoder_as5600.c
        BSP/FOC/pid/pid.h
        BSP/FOC/pid/pid.c
        BSP/FOC/timer/timer.h
        BSP/FOC/timer/timer.c
        BSP/FOC/foc_base.c
        BSP/FOC/foc_motor.c
        BSP/FOC/lowpass_filter.c
        BSP/FOC/pwm.h
        BSP/FOC/pwm.c
        BSP/test/test_as5600.c
        BSP/test/test_motor.c
        BSP/FOC/currentSense/currentSense.c
        BSP/FOC/currentSense/currentSense.h
        BSP/test/test_adc.c
        BSP/FOC/motor.c
        BSP/FOC/motor.h
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
    BSP/FOC/currentSense/ads1256.c
    BSP/FOC/timer/timer.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
        BSP
        BSP/FOC
        BSP/FOC/currentSense
        BSP/FOC/BLDCDriver
        BSP/FOC/encoder
        BSP/FOC/pid
        BSP/FOC/timer
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
