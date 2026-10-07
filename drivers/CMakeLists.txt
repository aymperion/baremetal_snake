add_library(drivers STATIC
    src/exti_helper.c
    src/gpio_driver.c
    src/rcc_helper.c
)

target_include_directories(drivers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(drivers
    PUBLIC
        mcuflags
    PRIVATE
        compile_warn
)
