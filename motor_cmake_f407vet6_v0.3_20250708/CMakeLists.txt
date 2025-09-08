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
set(CMAKE_PROJECT_NAME motor_cmake_f407vet6_v0)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    BSP/mymain.c
    BSP/led/led.c
    BSP/key/key.c
    BSP/pwm/bit_stream.c
    BSP/pwm/pid_init.c
    BSP/pwm/tb6612.c
    BSP/pwm/qam_test.c
    BSP/timer/timer_it.c 
    BSP/usart/usart_mx.c
    BSP/pwm/motor.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    BSP
    BSP/led 
    BSP/key
    BSP/pwm
    BSP/timer 
    BSP/usart
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
    # DEBUG_PRINT
    # DEBUG_PLOT_PID
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    -u_printf_float
    # -fdiagnostics-color=always
    # Add user defined libraries
)
# 设置 C 编译器彩色输出
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color=always")
set(CMAKE_COLOR_DIAGNOSTICS ON)
# # 对所有目标生效
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=")
#================config options================
set(UPLOAD_TOOL "cmsis-dap") #options: jlink, cmsis-dap
set(SWD_JTAG_PORT "swd") #options: swd, jtag
set(TARGET_MCU "stm32f4x") #options: stm32f1x, stm32f4x, stm32f7x, stm32h7x, and so on
#==============================================

# 查找必要的工具
find_program(OPENOCD /usr/bin/openocd)
find_program(GDB arm-none-eabi-gdb)
find_program(KILL pkill)  # Linux/macOS
# 对于 Windows 使用 taskkill
if(WIN32)
  find_program(KILL taskkill)
endif()
# Set upload command based on the selected tool
if(UPLOAD_TOOL STREQUAL "jlink")
  set(INTERFACE_CFG "interface/jlink.cfg")
elseif(UPLOAD_TOOL STREQUAL "cmsis-dap")
  set(INTERFACE_CFG "interface/cmsis-dap.cfg")
endif()

add_custom_target(flash
COMMAND ${OPENOCD} 
  -f ${INTERFACE_CFG}
  -c "transport select ${SWD_JTAG_PORT}"
  -f target/${TARGET_MCU}.cfg
  -c "program ${CMAKE_PROJECT_NAME}.elf verify reset exit"
DEPENDS ${CMAKE_PROJECT_NAME}.elf  # 依赖目标文件，确保先编译
COMMENT "Flashing ${CMAKE_PROJECT_NAME}.elf with OpenOCD"
)
add_dependencies(flash ${CMAKE_PROJECT_NAME})
# 自定义目标：启动调试会话
add_custom_target(start_openocd
COMMAND ${OPENOCD} 
  -f ${INTERFACE_CFG}
  -c "transport select swd" 
  -f target/${TARGET_MCU}.cfg
  -c "gdb_port 3333" 
  -c "tcl_port 6667" 
  -c "telnet_port 4445"
  > openocd.log 2>&1 &
)
#================debug stm32 command================
add_custom_target(debug
COMMAND ${GDB} ${CMAKE_PROJECT_NAME}.elf
  -ex "target extended-remote :3333" 
  -ex "monitor reset halt" 
  -ex "load" 
  -ex "break main" 
  -ex "continue" 
  USES_TERMINAL
COMMAND ${KILL} -f openocd 2> /dev/null
COMMENT "Closing openocd server.."
)
add_dependencies(debug start_openocd)