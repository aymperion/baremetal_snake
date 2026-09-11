option(COMPUTE_SIZE "Choose whether to compute bin size" ON)
option(OUTPUT_HEX "Choose whether to output .hex file" ON)
option(OUTPUT_BIN "Choose whether to output .bin file" ON)
cmake_minimum_required(VERSION 3.23)

project(Snake C ASM)

set(firmwareName "main")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${firmwareName}.elf)

target_include_directories(${firmwareName}.elf
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/include
)

target_sources(${firmwareName}.elf
    PRIVATE
        src/main.c
        src/startup_stm32f446zetx.s
        drivers/src/gpio_driver.c
        drivers/src/exti_helper.c
)

target_compile_options(${firmwareName}.elf 
    PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -Wall
        -Wextra
        -Werror
)

target_link_options(${firmwareName}.elf 
    PRIVATE
        -T${CMAKE_CURRENT_SOURCE_DIR}/STM32F446ZETX_FLASH.ld
        -nostartfiles
        -mcpu=cortex-m4
        -mthumb
)

target_link_libraries(${firmwareName}.elf 
    PRIVATE 
        c 
        rdimon
)


if (COMPUTE_SIZE)
    add_custom_command(
        TARGET ${firmwareName}.elf POST_BUILD
        COMMAND arm-none-eabi-size ${firmwareName}.elf
    )
endif()

if(OUTPUT_HEX)
    add_custom_command(
        TARGET ${firmwareName}.elf POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Generating ${firmwareName}.hex ..."
        COMMAND arm-none-eabi-objcopy -O ihex ${firmwareName}.elf ${firmwareName}.hex
    )
endif()

if(OUTPUT_BIN)
    add_custom_command(
        TARGET ${firmwareName}.elf POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Generating ${firmwareName}.bin ..."
        COMMAND arm-none-eabi-objcopy -O binary -S ${firmwareName}.elf ${firmwareName}.bin
    )
endif()