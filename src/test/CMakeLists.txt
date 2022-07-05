if(NOT EXISTS ${CMAKE_SOURCE_DIR}/third_party/libporto)
    # libporto already brings along googletest. Do not add it twice.
    add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/googletest third_party/googletest)
endif()

# Macro to create a test executable given the test name and
# the list of test code sources.
macro(package_add_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})
    add_dependencies(build-tests ${TESTNAME})
    target_link_libraries(${TESTNAME} ${PROJECT_NAME} gtest gmock gtest_main)
    set_target_properties(${TESTNAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PPX_OUTPUT_DIR}/test"
    )
    gtest_discover_tests(${TESTNAME}
        # Set working directory to project output root.
        WORKING_DIRECTORY "${PPX_OUTPUT_DIR}"
    )
endmacro()

# The default `test` target automatically created by CMake runs the tests but
# does not build them.
# This custom `build-tests` target builds all the tests.
add_custom_target(build-tests)
# This custom `run-tests` target builds and runs all the tests.
add_custom_target(run-tests
    COMMAND ${CMAKE_CTEST_COMMAND}
    DEPENDS build-tests
)

file(GLOB TEST_SOURCES "*.cpp" "*.h")
package_add_test(ppx_tests ${TEST_SOURCES})
