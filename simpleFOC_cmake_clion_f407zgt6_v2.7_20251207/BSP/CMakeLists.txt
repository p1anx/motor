cmake_minimum_required(VERSION 3.22)

#add_executable(${CMAKE_PROJECT_NAME}
        #        BSP/STM32Monitor/Inc/acqTrigger.h
        #        BSP/STM32Monitor/Inc/dataAcq.h
        #        BSP/STM32Monitor/Src/acqTrigger.c
        #        BSP/STM32Monitor/Src/dataAcq.c

#)
# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user sources here

        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/key/key.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/stm32_hal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/led/led.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/mymain.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/currentSense/ads1256.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/BLDCMotor.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/BLDCDriver/BLDCDriver.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/encoder/as5600.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/encoder/encoder.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/encoder/encoder_as5600.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/pid/pid.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/timer/timer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/foc_base.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/foc_motor.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/lowpass_filter.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/pwm.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/currentSense/currentSense.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/currentSense/my_adc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/motor.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/encoder/mt6835.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/LoopController.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/uart/uart.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/MotorType/Motor_2804/motor_2804.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/uart/vofa.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/stm32h7_dma/stm32h7_dma.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/test/test_ads1256.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/test/test_3pwm.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/test/test_uart.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/test/test_mt6835.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/test/test_main.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/test/test_adc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/test/test_as5600.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/test/test_motor.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/test/test_2804.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user defined include paths
      ${CMAKE_CURRENT_SOURCE_DIR}/../BSP
      ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC
      ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/key
      ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/currentSense
      ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/BLDCDriver
      ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/encoder
      ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/pid
      ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/timer
      ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/uart
      ${CMAKE_CURRENT_SOURCE_DIR}/../BSP/FOC/MotorType/Motor_2804
        #        BSP/STM32Monitor/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user defined symbols
)
target_link_libraries(${CMAKE_PROJECT_NAME}
#        stm32cubemx
        -u_printf_float

        # Add user defined libraries
)
