cmake_minimum_required(VERSION 3.22)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user sources here
#        ${CMAKE_CURRENT_SOURCE_DIR}/STM32/mymain.c
        ${CMAKE_CURRENT_SOURCE_DIR}/STM32/mymain.c
        ${CMAKE_CURRENT_SOURCE_DIR}/STM32/stm32_hal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/STM32/uart/uart.c
        ${CMAKE_CURRENT_SOURCE_DIR}/STM32/adc/adc_sampling.c
        ${CMAKE_CURRENT_SOURCE_DIR}/STM32/pwm/pwm.c
        ${CMAKE_CURRENT_SOURCE_DIR}/STM32/key/key.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Config/configSTM32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Config/config.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Debug/debug_main.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Sensor/MT6835/mt6835.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Sensor/sensor.c
        ${CMAKE_CURRENT_SOURCE_DIR}/FOC/foc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/CurrentSense/currentSense.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Driver/FOCDriver.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Motor/Motor.c
        ${CMAKE_CURRENT_SOURCE_DIR}/PID/pid.c
        ${CMAKE_CURRENT_SOURCE_DIR}/LowpassFilter/LowpassFilter.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Monitor/monitor.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Config/configODrive.c
        ${CMAKE_CURRENT_SOURCE_DIR}/DRV830X/DRV830X.c
        ${CMAKE_CURRENT_SOURCE_DIR}/QAM/QAM.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user defined include paths
        ${CMAKE_CURRENT_SOURCE_DIR}/STM32
        ${CMAKE_CURRENT_SOURCE_DIR}/Debug
        ${CMAKE_CURRENT_SOURCE_DIR}/STM32/uart
        ${CMAKE_CURRENT_SOURCE_DIR}/STM32/adc
        ${CMAKE_CURRENT_SOURCE_DIR}/STM32/pwm
        ${CMAKE_CURRENT_SOURCE_DIR}/STM32/key
        ${CMAKE_CURRENT_SOURCE_DIR}/Config
        ${CMAKE_CURRENT_SOURCE_DIR}/Sensor
        ${CMAKE_CURRENT_SOURCE_DIR}/Sensor/MT6835
        ${CMAKE_CURRENT_SOURCE_DIR}/Sensor/AS5600
        ${CMAKE_CURRENT_SOURCE_DIR}/Sensor/EncABZ
        ${CMAKE_CURRENT_SOURCE_DIR}/FOC
        ${CMAKE_CURRENT_SOURCE_DIR}/CurrentSense
        ${CMAKE_CURRENT_SOURCE_DIR}/Driver
        ${CMAKE_CURRENT_SOURCE_DIR}/Motor
        ${CMAKE_CURRENT_SOURCE_DIR}/PID
        ${CMAKE_CURRENT_SOURCE_DIR}/LowpassFilter
        ${CMAKE_CURRENT_SOURCE_DIR}/Monitor
        ${CMAKE_CURRENT_SOURCE_DIR}/DRV830X
        ${CMAKE_CURRENT_SOURCE_DIR}/QAM
#      ${CMAKE_CURRENT_SOURCE_DIR}/Config
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user defined symbols
        Debug
#        IS_CALCULATED_TIME
#        IS_FIXED_TIME
        IS_ODRIVE
)
target_link_libraries(${CMAKE_PROJECT_NAME}
#        stm32cubemx
        -u_printf_float

        # Add user defined libraries
)
