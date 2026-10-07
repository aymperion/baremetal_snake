# Based on https://github.com/rpoisel/cmake-unity-tutorial/ :
cmake_minimum_required(VERSION 3.23)

project(Snake C ASM)

set(TARGET_GROUP production CACHE STRING "Group to build: production or test")

option(COMPUTE_SIZE "Choose whether to compute bin size" ON)
option(OUTPUT_HEX "Choose whether to output .hex file" ON)
option(OUTPUT_BIN "Choose whether to output .bin file" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ----------------------------------------------------
# Common INTERFACE targets
# ----------------------------------------------------
add_library(mcuflags INTERFACE)
target_compile_options(mcuflags INTERFACE
    -mcpu=cortex-m4
    -mthumb
)
target_link_options(mcuflags INTERFACE
    -mcpu=cortex-m4
    -mthumb
)

add_library(compile_warn INTERFACE)
target_compile_options(compile_warn INTERFACE
    -Wall
    -Wextra
    -Werror
)

# ----------------------------------------------------
# Helper function to generate output files
# ----------------------------------------------------
function(generate_firmware_outputs target_name)
    string(REGEX REPLACE "\\.elf$" "" base_name "${target_name}")

    if (COMPUTE_SIZE)
        add_custom_command(
            TARGET ${target_name} POST_BUILD
            COMMAND arm-none-eabi-size $<TARGET_FILE:${target_name}>
        )
    endif()

    if(OUTPUT_HEX)
        add_custom_command(
            TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Generating ${base_name}.hex ..."
            COMMAND arm-none-eabi-objcopy -O ihex $<TARGET_FILE:${target_name}> $<TARGET_FILE_DIR:${target_name}>/${base_name}.hex
        )
    endif()

    if(OUTPUT_BIN)
        add_custom_command(
            TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Generating ${base_name}.bin ..."
            COMMAND arm-none-eabi-objcopy -O binary -S $<TARGET_FILE:${target_name}> $<TARGET_FILE_DIR:${target_name}>/${base_name}.bin
        )
    endif()
endfunction()

# Shared code
add_subdirectory(drivers)
add_subdirectory(startup)

# "production" target
if(TARGET_GROUP STREQUAL production)
    set(firmwareName "main")
    

    add_executable(${firmwareName}.elf
        src/main.c
    )

    target_include_directories(${firmwareName}.elf
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_link_options(${firmwareName}.elf 
        PRIVATE
            -T${CMAKE_CURRENT_SOURCE_DIR}/STM32F446ZETX_FLASH.ld
            -nostartfiles
            --specs=nano.specs
            --specs=rdimon.specs
    )

    target_link_libraries(${firmwareName}.elf 
        PRIVATE 
            drivers
            startup
            compile_warn
            c 
            rdimon
    )

    generate_firmware_outputs(${firmwareName}.elf)
else()
    message(FATAL_ERROR "Given TARGET_GROUP '${TARGET_GROUP}' unknown. Expected 'production'.")
endif()