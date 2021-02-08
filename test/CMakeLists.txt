set( PROJECT_UNIT_TESTS_NAME stress_tests )

set(HEADERS
)

set(SOURCE
    ../src/impl/QueueImpl.cpp
    StressTests.cpp
)

ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)

add_executable(${PROJECT_UNIT_TESTS_NAME} ${HEADERS} ${SOURCE})

#set( BOOST_TEST_LOG_LEVEL test_suite )
set( BOOST_TEST_BUILD_INFO yes )

target_include_directories(${PROJECT_UNIT_TESTS_NAME}
    PRIVATE
    ${BOOST_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/impl
    ${PROJECT_SOURCE_DIR}/test
)

target_link_libraries(${PROJECT_UNIT_TESTS_NAME}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    -lpthread
)

set_target_properties(${PROJECT_UNIT_TESTS_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

add_test(NAME stress COMMAND ${PROJECT_UNIT_TESTS_NAME})

################################################################################



set( PROJECT_UNIT_TESTS_NAME correctness_tests )

set(SOURCE
    ../src/impl/QueueImpl.cpp
    SharedQueue.cpp
)

add_executable(${PROJECT_UNIT_TESTS_NAME} ${HEADERS} ${SOURCE})

target_include_directories(${PROJECT_UNIT_TESTS_NAME}
    PRIVATE
    ${BOOST_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/impl
    ${PROJECT_SOURCE_DIR}/test
)

target_link_libraries(${PROJECT_UNIT_TESTS_NAME}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    -lpthread
)

set_target_properties(${PROJECT_UNIT_TESTS_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

add_test(NAME correctness COMMAND ${PROJECT_UNIT_TESTS_NAME})

################################################################################



set( PROJECT_UNIT_TESTS_NAME performance_tests )

set(SOURCE
    ../src/impl/QueueImpl.cpp
    PerformanceTests.cpp
)

add_executable(${PROJECT_UNIT_TESTS_NAME} ${HEADERS} ${SOURCE})

target_include_directories(${PROJECT_UNIT_TESTS_NAME}
    PRIVATE
    ${BOOST_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/impl
    ${PROJECT_SOURCE_DIR}/test
)

target_link_libraries(${PROJECT_UNIT_TESTS_NAME}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    -lpthread
)

set_target_properties(${PROJECT_UNIT_TESTS_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

add_test(NAME performance COMMAND ${PROJECT_UNIT_TESTS_NAME})

