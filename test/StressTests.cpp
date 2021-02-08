#define BOOST_TEST_MODULE StressTests
#include <boost/test/unit_test.hpp>

#include "utilities.hpp"

#include "QueueFactory.h"

#include <algorithm>
#include <set>

/*----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
Test plan:

Done    1. More producers less consumers
Done    2. Less producers more consumers

------------------------------------------------------------------------------*/

constexpr int g_queueSize = 10000000;

/*----------------------------------------------------------------------------*/

BOOST_AUTO_TEST_SUITE( Test3 )

/*----------------------------------------------------------------------------*/

BOOST_AUTO_TEST_CASE( MoreProducersLessConsumers_1 )
{
    constexpr int producersCount = 100;
    constexpr int cunsomersCount = producersCount / 2;
    constexpr int elementsToPush = 10000;
    constexpr int elementsToPop = elementsToPush * 2;

    auto pQueue = QueueFactory::createStandardSharedQueue< int >( g_queueSize );

    std::vector< std::thread > pushers;
    std::vector< std::thread > poppers;
    std::set< int > expectedUniqueNumbers;
    std::mutex uniqueElementsMutex;
    std::mutex countMutex;

    std::vector< int > pNumbers;
    for ( int i = 0; i < producersCount * elementsToPush; ++i ) {
        pNumbers.emplace_back( i );
    }

    for ( int i = 0; i < producersCount; ++i )
    {
        pushers.push_back( std::thread (
            [ &pQueue, &pNumbers, &countMutex, i ] {
                for ( int j = 0; j < elementsToPush; ++j )
                {
                    pQueue->enqueue(
                        &pNumbers.at( j + ( i * elementsToPush ) )
                    );
                    std::lock_guard< std::mutex > lck( countMutex );
                    BOOST_CHECK( pQueue->count() <= g_queueSize );
                }
            } )
        );
    }

    for ( int i = 0; i < cunsomersCount; ++i )
    {
        poppers.push_back( std::thread (
            [
                    &pQueue
                ,   &pNumbers
                ,   &expectedUniqueNumbers
                ,   &uniqueElementsMutex
                ,   &countMutex
                ,   i
            ]
            {
                for ( int j = 0; j < elementsToPop; ++j )
                {
                    auto * pElem = pQueue->dequeue();
                    //thread_safe_cout( "Popped element:", *pElem );
                    {
                        std::lock_guard< std::mutex > lck( countMutex );
                        BOOST_CHECK( pQueue->count() >= 0 );
                    }
                    {
                        std::lock_guard< std::mutex > lck(
                            uniqueElementsMutex
                        );
                        expectedUniqueNumbers.insert( *pElem );
                    }
                }
            } )
        );
    }

    for ( auto & thread: poppers )
        thread.join();

    for ( auto & thread: pushers )
        thread.join();

    BOOST_CHECK_EQUAL(
            expectedUniqueNumbers.size()
        ,   elementsToPush * producersCount
    );
    BOOST_CHECK_EQUAL( pQueue->count(), 0 );

}

/*----------------------------------------------------------------------------*/

BOOST_AUTO_TEST_CASE( LessProducersMoreConsumers_2 )
{
    constexpr int producersCount = 50;
    constexpr int cunsomersCount = producersCount * 2;
    constexpr int elementsToPush = 2000;
    constexpr int elementsToPop = elementsToPush / 2;

    auto pQueue = QueueFactory::createStandardSharedQueue< int >( g_queueSize );

    std::vector< std::thread > pushers;
    std::vector< std::thread > poppers;
    std::vector< int > expectedUniqueNumbers;
    expectedUniqueNumbers.reserve( elementsToPush * producersCount );
    std::mutex uniqueElementsMutex;
    std::mutex countMutex;

    std::vector< int > pNumbers;
    for ( int i = 0; i < producersCount * elementsToPush; ++i ) {
        pNumbers.emplace_back( i );
    }

    for ( int i = 0; i < producersCount; ++i )
    {
        pushers.push_back( std::thread (
            [ &pQueue, &pNumbers, &countMutex, i ] {
                for ( int j = 0; j < elementsToPush; ++j )
                {
                    pQueue->enqueue(
                        &pNumbers.at( j + ( i * elementsToPush ) )
                    );
                    std::lock_guard< std::mutex > lck( countMutex );
                    BOOST_CHECK( pQueue->count() <= g_queueSize );
                }
            } )
        );
    }

    for ( int i = 0; i < cunsomersCount; ++i )
    {
        poppers.push_back( std::thread (
            [
                    &pQueue
                ,   &pNumbers
                ,   &expectedUniqueNumbers
                ,   &uniqueElementsMutex
                ,   &countMutex
                ,   i
            ]
            {
                for ( int j = 0; j < elementsToPop; ++j )
                {
                    auto * pElem = pQueue->dequeue();
                    {
                        std::lock_guard< std::mutex > lck( countMutex );
                        BOOST_CHECK( pQueue->count() >= 0 );
                    }
                    {
                        std::lock_guard< std::mutex > lck(
                            uniqueElementsMutex
                        );
                        expectedUniqueNumbers.push_back( *pElem );
                    }
                }
            } )
        );
    }

    for ( auto & thread: poppers )
        thread.join();

    for ( auto & thread: pushers )
        thread.join();

    BOOST_CHECK_EQUAL(
            expectedUniqueNumbers.size()
        ,   elementsToPush * producersCount
    );
    BOOST_CHECK(
        std::unique(
            expectedUniqueNumbers.begin(), expectedUniqueNumbers.end()
        ) == expectedUniqueNumbers.end()
    );
    BOOST_CHECK_EQUAL( pQueue->count(), 0 );

}

/*----------------------------------------------------------------------------*/

BOOST_AUTO_TEST_SUITE_END()