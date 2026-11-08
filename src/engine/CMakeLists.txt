# plenio_engine: the real-time player (takes, stems, MIDI out). No VST3 types: the plug-ins adapt to it.

add_library(plenio_engine STATIC
    engine_placeholder.cpp
)
plenio_target_defaults(plenio_engine)
target_include_directories(plenio_engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(plenio_engine PUBLIC plenio_core)
set_target_properties(plenio_engine PROPERTIES FOLDER "Plenio")
