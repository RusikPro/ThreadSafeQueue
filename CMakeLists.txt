cmake_minimum_required(VERSION 2.8.7)

set(PROJECT_NAME shared_queue)

project( ${PROJECT_NAME}
    LANGUAGES
        CXX
)

set(HEADER
    src/IQueue.h
    src/QueueFactory.h
    src/impl/SharedQueue.h
    src/impl/QueueImpl.h
)

set(SOURCE
    src/impl/SharedQueue.cpp
    src/impl/QueueImpl.cpp
)


# Boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_DEBUG On)


# set(Boost_INCLUDE_DIR /compil/midas/external/)

# INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)



#-----------------------------------------------------------------------------#
# Tests                                                                       #
#-----------------------------------------------------------------------------#

enable_testing()

#-----------------------------------------------------------------------------#
# Project tests                                                              #
#-----------------------------------------------------------------------------#

project( ${PROJECT_NAME} )
add_subdirectory(test)