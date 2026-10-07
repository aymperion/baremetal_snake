add_library(startup STATIC
    startup_stm32f446zetx.s
)

target_link_libraries(startup
    PUBLIC
        mcuflags
    PRIVATE
        compile_warn
)
