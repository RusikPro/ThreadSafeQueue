#define BOOST_TEST_MODULE PerformanceTests
#include <boost/test/unit_test.hpp>

#include "utilities.hpp"

#include "QueueFactory.h"

#include <algorithm>
#include <chrono>
#include <numeric>

/*----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
Test plan:

Done    1. Header implementation
Done        1.1. Multiple threads - one element
Done        1.2. One thread - multiple elements
Done            1.2.1. Queue size less than elements number
Done            1.2.2. Queue size equal to elements number
////////////////////////////////////////////////////////////////////////////////
This part is done mainly to reduce the compilation speed, though it can slow
down queue work a little bit:
Done    2. PImpl
Done        2.1. Multiple threads - one element
Done        2.2. One thread - multiple elements
Done            2.2.1. Queue size less than elements number
Done            2.2.2. Queue size equal to elements number

------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

using namespace std::chrono;

struct Fixture
{

    using TimeType = decltype( system_clock::now() );
    using BenchmarkType = std::pair< TimeType, TimeType >;
    using ElapsedTimeType = decltype( std::declval< microseconds >().count() );

    Fixture ()
        :   m_pElement{ new int{ 1 } }
        ,   m_pElements(
                QueueFactory::createStandardSharedQueue< int >( 100 )
            )
    {
    }

    ~Fixture ()
    {
        delete m_pElement;
    }

    void setQueue ( std::unique_ptr< IQueue< int > > _pQueue )
    {
        m_pElements = std::move( _pQueue );
    }

    void testOneElementPerThread ()
    {
        constexpr int measurementsCount = 10000;

        BenchmarkType benchmark;
        for ( int i = 0; i < measurementsCount; ++i )
        {
            std::thread tConsumer( [ this, &benchmark ] {

                m_pElements->dequeue();
                benchmark.first = system_clock::now();
            } );

            std::thread tPusher( [ this, &benchmark ] {

                benchmark.second = system_clock::now();
                m_pElements->enqueue( m_pElement );
            } );

            tConsumer.join();
            tPusher.join();

            m_benchmarks.push_back(
                duration_cast< microseconds >(
                    benchmark.first - benchmark.second
                ).count()
            );
        }

        BOOST_TEST_MESSAGE( "Measurements number: " << measurementsCount );
    }

    void testMultipleElementsPerThread (
            int _elementsToPush
    )
    {
        std::vector< TimeType > pushingTimes;
        std::vector< TimeType > poppingTimes;

        BenchmarkType benchmark;

        std::thread tConsumer( [ this, _elementsToPush, &poppingTimes ] {
            for ( int i = 0; i < _elementsToPush; ++i )
            {
                auto * pElem = m_pElements->dequeue();
                poppingTimes.push_back( system_clock::now() );
                BOOST_CHECK( pElem == m_pElement );
                BOOST_CHECK_EQUAL( *pElem, 1 );
            }
        } );

        std::thread tPusher( [ this, _elementsToPush, &pushingTimes ] {
            for ( int i = 0; i < _elementsToPush; ++i )
            {
                pushingTimes.push_back( system_clock::now() );
                m_pElements->enqueue( m_pElement );
            }
        } );

        tConsumer.join();
        tPusher.join();

        for ( int i = 0; i < _elementsToPush; ++i )
        {
            m_benchmarks.push_back(
                duration_cast< microseconds >(
                    poppingTimes.at( i ) - pushingTimes.at( i )
                ).count()
            );
        }
    }

    void printBenchmarks ()
    {
        auto minimumElapsed =
            std::min_element( m_benchmarks.begin(), m_benchmarks.end() );
        BOOST_TEST_MESSAGE(
            "Fastest passthrough: " << *minimumElapsed << " microseconds"
        );

        auto maximumElapsed =
            std::max_element( m_benchmarks.begin(), m_benchmarks.end() );
        BOOST_TEST_MESSAGE(
            "Slowest passthrough: " << *maximumElapsed << " microseconds"
        );

        auto average =
            std::accumulate(
                m_benchmarks.begin(), m_benchmarks.end(), 0
            ) / m_benchmarks.size()
        ;
        BOOST_TEST_MESSAGE(
            "Average passthrough: " << average << " microseconds"
        );
    }

    int * m_pElement;
    std::unique_ptr< IQueue< int > > m_pElements;
    std::vector< ElapsedTimeType > m_benchmarks;
};

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

BOOST_FIXTURE_TEST_SUITE( PerformanceTests, Fixture )

/*----------------------------------------------------------------------------*/

BOOST_AUTO_TEST_CASE( MultipleThreadsOneElement__1_1 )
{
    setQueue( QueueFactory::createStandardSharedQueue< int >( 100 ) );

    testOneElementPerThread();

    printBenchmarks();
}

/*----------------------------------------------------------------------------*/

BOOST_AUTO_TEST_CASE( OneThreadMultipleElements__QueueSizeLess__1_2_1 )
{
    constexpr int queueSize = 100;
    constexpr int elementsToPush = 10000;

    setQueue( QueueFactory::createStandardSharedQueue< int >( queueSize ) );

    testMultipleElementsPerThread( elementsToPush );

    BOOST_TEST_MESSAGE( "Queue size: " << queueSize );
    BOOST_TEST_MESSAGE( "Elements passed: " << elementsToPush );

    printBenchmarks();
}

/*----------------------------------------------------------------------------*/

BOOST_AUTO_TEST_CASE( OneThreadMultipleElements__QueueSizeEqual__1_2_2 )
{
    constexpr int queueSize = 10000;
    constexpr int elementsToPush = 10000;

    setQueue( QueueFactory::createStandardSharedQueue< int >( queueSize ) );
    testMultipleElementsPerThread( elementsToPush );

    BOOST_TEST_MESSAGE( "Queue size: " << queueSize );
    BOOST_TEST_MESSAGE( "Elements passed: " << elementsToPush );

    printBenchmarks();
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

BOOST_AUTO_TEST_CASE( PImpl__MultipleThreadsOneElement__2_1 )
{
    setQueue( QueueFactory::createSharedQueueWithPImpl< int >( 100 ) );

    testOneElementPerThread();

    printBenchmarks();
}

/*----------------------------------------------------------------------------*/

BOOST_AUTO_TEST_CASE( PImpl__OneThreadMultipleElements__QueueSizeLess__2_2_1 )
{
    constexpr int queueSize = 100;
    constexpr int elementsToPush = 10000;

    setQueue( QueueFactory::createSharedQueueWithPImpl< int >( queueSize ) );

    testMultipleElementsPerThread( elementsToPush );

    BOOST_TEST_MESSAGE( "Queue size: " << queueSize );
    BOOST_TEST_MESSAGE( "Elements passed: " << elementsToPush );

    printBenchmarks();
}

/*----------------------------------------------------------------------------*/

BOOST_AUTO_TEST_CASE( PImpl__OneThreadMultipleElements__QueueSizeEqual__2_2_2 )
{
    constexpr int queueSize = 10000;
    constexpr int elementsToPush = 10000;

    setQueue( QueueFactory::createSharedQueueWithPImpl< int >( queueSize ) );
    testMultipleElementsPerThread( elementsToPush );

    BOOST_TEST_MESSAGE( "Queue size: " << queueSize );
    BOOST_TEST_MESSAGE( "Elements passed: " << elementsToPush );

    printBenchmarks();
}

/*----------------------------------------------------------------------------*/

BOOST_AUTO_TEST_SUITE_END()

/*----------------------------------------------------------------------------*/