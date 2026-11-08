# plenio_core: everything that needs neither VST3 nor a UI. Testable on its own (tests/core).

add_library(plenio_core STATIC
    log.h
    log.cpp
    paths.h
    paths.cpp
    json.h
)
plenio_target_defaults(plenio_core)
target_include_directories(plenio_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/.." "${PLENIO_GENERATED}")
target_link_libraries(plenio_core PUBLIC plenio_json PRIVATE plenio_httplib plenio_drlibs plenio_r8brain)
if(WIN32)
    target_link_libraries(plenio_core PUBLIC shell32 ole32)
endif()
set_target_properties(plenio_core PROPERTIES FOLDER "Plenio")
