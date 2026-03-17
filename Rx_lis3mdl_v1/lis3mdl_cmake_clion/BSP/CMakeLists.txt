cmake_minimum_required(VERSION 3.22)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/uart/my_uart.c

)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/uart

)

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
        Debug

)

target_link_libraries(${CMAKE_PROJECT_NAME}
        -u_printf_float

)