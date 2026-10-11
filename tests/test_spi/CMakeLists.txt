add_library(test_spi STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_spi.c
)

target_include_directories(test_spi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(test_spi
    PUBLIC
        Unity
        drivers
        mcuflags
    PRIVATE
        compile_warn
)
