# ----------------------------------------------------
# GPIO Loopback Test Executable
# ----------------------------------------------------
set(testTarget "main")

add_executable(${testTarget}.elf
    src/test_main.c
    src/test_gpio_loopback.c
)

set_target_properties(${testTarget}.elf PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_include_directories(${testTarget}.elf
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_options(${testTarget}.elf
    PRIVATE
        -T${CMAKE_CURRENT_SOURCE_DIR}/../STM32F446ZETX_FLASH.ld
        -nostartfiles
        --specs=nano.specs
        --specs=rdimon.specs
)

target_link_libraries(${testTarget}.elf
    PRIVATE
        drivers
        startup
        Unity
        compile_warn
        c
        rdimon
)

add_test(NAME ${testTarget} COMMAND ${testTarget}.elf)

generate_firmware_outputs(${testTarget}.elf)