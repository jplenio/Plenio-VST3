# Unit tests of the core and the engine (doctest), run with ctest or tools/test.ps1.

add_executable(plenio_tests
    main.cpp
    core/test_paths.cpp
    core/test_midi_score.cpp
    core/test_timeline.cpp
    core/test_song.cpp
    engine/test_player.cpp
)
plenio_target_defaults(plenio_tests)
target_link_libraries(plenio_tests PRIVATE plenio_core plenio_engine plenio_doctest)
target_compile_definitions(plenio_tests PRIVATE PLENIO_TEST_DATA="${CMAKE_CURRENT_SOURCE_DIR}/data")
set_target_properties(plenio_tests PROPERTIES FOLDER "Tests")
add_test(NAME plenio_tests COMMAND plenio_tests)
