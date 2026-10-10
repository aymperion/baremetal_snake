add_library(test_gpio STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_gpio.c
)

target_include_directories(test_gpio PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(test_gpio
    PUBLIC
        Unity
        drivers
        mcuflags
    PRIVATE
        compile_warn
)
