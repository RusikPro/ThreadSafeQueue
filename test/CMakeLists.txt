# Set C++ Standard globally for all test targets
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# Boost Setup
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)

# Function to add a test target
function(add_unit_test TEST_NAME TEST_SOURCES)
    # Define the executable for the test
    add_executable(${TEST_NAME} ${TEST_SOURCES})

    target_include_directories(${TEST_NAME}
        PRIVATE
        ${BOOST_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/impl
        ${PROJECT_SOURCE_DIR}/test
    )

    target_link_libraries(${TEST_NAME}
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        -lpthread
    )

    # Register the test
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

endfunction()


# List of Tests: Define test names and source files as separate lists
set(TEST_NAMES stress_tests correctness_tests performance_tests)
set(TEST_SOURCES
    "StressTests.cpp"
    "SharedQueue.cpp"
    "PerformanceTests.cpp"
)

# Add tests in a loop
list(LENGTH TEST_NAMES TEST_COUNT)
math(EXPR INDEX "${TEST_COUNT} - 1")

foreach(i RANGE ${INDEX})
    list(GET TEST_NAMES ${i} TEST_NAME)
    list(GET TEST_SOURCES ${i} TEST_SOURCE)

    add_unit_test(${TEST_NAME}
        "${PROJECT_SOURCE_DIR}/src/impl/QueueImpl.cpp;${TEST_SOURCE}"
    )
endforeach()
