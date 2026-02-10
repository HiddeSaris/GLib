add_library(GLib
    test.cpp
)

target_include_directories(GLib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/GLib/include
        ${PROJECT_SOURCE_DIR}/GLib/src
)