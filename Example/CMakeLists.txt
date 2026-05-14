add_executable(Example
    Example.cpp
)

target_link_libraries(Example
    PRIVATE GLib
)

add_custom_command(TARGET Example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:Example>/assets
)