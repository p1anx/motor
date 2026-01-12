cmake_minimum_required(VERSION 3.22)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user sources here
        ${CMAKE_CURRENT_SOURCE_DIR}/RTT/JS_RTT.c
        ${CMAKE_CURRENT_SOURCE_DIR}/RTT/SEGGER_RTT.c
        ${CMAKE_CURRENT_SOURCE_DIR}/RTT/SEGGER_RTT_printf.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user defined include paths
      ${CMAKE_CURRENT_SOURCE_DIR}/RTT
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user defined symbols

)
target_link_libraries(${CMAKE_PROJECT_NAME}
        # Add user defined libraries
)
