# Plenio's own code: the core (no VST3), the engine, and the bundle with both plug-ins.

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/plugin/version.h.in" "${PLENIO_GENERATED}/plenio_version.h" @ONLY)

add_subdirectory(core)
add_subdirectory(engine)

set(PLENIO_PLUGIN_SOURCES
    plugin/entry.cpp
    plugin/ids.h
    plugin/instrument.h
    plugin/instrument.cpp
    plugin/ara_effect.h
    plugin/ara_effect.cpp
    plugin/state_stream.h
    plugin/state_stream.cpp
    plugin/song_controller.h
    plugin/song_controller.cpp
    platform/dialogs.h
    platform/dialogs.cpp
    platform/drag.h
    platform/drag.cpp
    platform/keys.h
    platform/keys.cpp
    plugin/reaper.h
    plugin/reaper.cpp
    ara/document_controller.h
    ara/document_controller.cpp
    ara/playback_renderer.h
    ara/playback_renderer.cpp
    ara/audio_source.h
    ara/audio_source.cpp
    ara/content_reader.h
    ara/factory.h
    ara/factory.cpp
    ui/backend.h
    ui/editor_view.h
    ui/editor_view.cpp
    ui/webview_panel.h
    ui/webview_panel.cpp
    ui/webui_resources.h
    "${vst3sdk_SOURCE_DIR}/public.sdk/source/vst/vstsinglecomponenteffect.cpp"
    "${PLENIO_WEBUI_CPP}"
)

# the bundle's folder icon: Plenio's own (the SDK's default is the VST logo, a Steinberg trademark)
set(SMTG_PACKAGE_ICON_PATH "${PROJECT_SOURCE_DIR}/resources/plenio.ico")
smtg_add_vst3plugin(Plenio ${PLENIO_PLUGIN_SOURCES})
plenio_target_defaults(Plenio)
target_include_directories(Plenio PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${PLENIO_GENERATED}")
target_link_libraries(Plenio PRIVATE
    sdk
    ARA_PlugIn_Library
    plenio_core
    plenio_engine
    plenio_choc
    plenio_reaper_sdk
)
add_dependencies(Plenio plenio_webui)
set_source_files_properties("${PLENIO_WEBUI_CPP}" PROPERTIES GENERATED TRUE)
set_target_properties(Plenio PROPERTIES FOLDER "Plenio")

if(WIN32)
    target_sources(Plenio PRIVATE "${PROJECT_SOURCE_DIR}/resources/plenio.rc")
    target_link_libraries(Plenio PRIVATE ole32 oleaut32 shell32 shlwapi user32 gdi32 version)
endif()

if(MSVC)
    # the generated web UI source is large: no warnings and the big-object format for it
    set_source_files_properties("${PLENIO_WEBUI_CPP}" PROPERTIES COMPILE_OPTIONS "/W0;/bigobj")
endif()
