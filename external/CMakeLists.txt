add_library(Unity STATIC
    Unity/src/unity.c
)

target_include_directories(Unity PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Unity/src
)

target_link_libraries(Unity PUBLIC
    mcuflags
)