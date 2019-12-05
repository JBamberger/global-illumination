
find_package(GTest CONFIG REQUIRED)

add_executable(unit_tests
    main.cpp
    entities-test.cpp
    bbox-test.cpp
    checkerboard-test.cpp
    uv-mapping-test.cpp
)

target_link_libraries(
	unit_tests
	PRIVATE GTest::gtest GTest::gtest_main
	global_illu_lib
)

add_test(NAME unit COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/unit_tests)