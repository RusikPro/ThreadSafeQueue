#define BOOST_TEST_MODULE CorrectnessTests
#include <boost/test/unit_test.hpp>

#include "utilities.hpp"

#include "QueueFactory.h"

/*----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
Test plan:

Not done    1. Data correctness
Done        2. Enqueue timeout
Done        3. Dequeue timeout

------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

BOOST_AUTO_TEST_SUITE( Test )

/*----------------------------------------------------------------------------*/

BOOST_AUTO_TEST_CASE( EnqueueTimeout_2 )
{
    auto elements = QueueFactory::createStandardSharedQueue< int >( 10 );

    std::thread t1Push( [ & ] {
        for ( int i = 0; i < 10; ++i ) {
            int * pElem = new int{ i };
            elements->enqueue( pElem );
            thread_safe_cout( "Pushed one element:", *pElem );
        }

        int * pNumber = new int{ 10 };

        thread_safe_cout( "Trying to add '10'" );
        auto added = elements->enqueue( pNumber, 1000 );
        thread_safe_cout( "Was added? -", std::boolalpha, added );

        thread_safe_cout( "Trying to add '10'" );
        added = elements->enqueue( pNumber, 1000 );
        BOOST_CHECK( added );
        thread_safe_cout( "Was added? -", std::boolalpha, added );
    } );

    std::thread t1Pop( [ &elements ] () {
        sleep_for( 1500 );
        auto * pElem = elements->dequeue();
        thread_safe_cout( "Popped one element:", *pElem );
        BOOST_CHECK_EQUAL( *pElem, 0 );
        sleep_for( 1000 );
        delete pElem;
    } );

    t1Pop.join();
    t1Push.join();

    //  TODO: split test into { initialization; workflow; checking } blocks
}

/*----------------------------------------------------------------------------*/

BOOST_AUTO_TEST_CASE( DequeueTimeout_3 )
{
    auto elements = QueueFactory::createStandardSharedQueue< int >( 10 );

    std::thread t1Pop( [ &elements ] () {
        thread_safe_cout( "Trying to pop" );
        auto * pElem = elements->dequeue( 2000 );
        thread_safe_cout( "Was popped? -", std::boolalpha, pElem != nullptr );

        thread_safe_cout( "Trying to pop" );
        pElem = elements->dequeue( 2000 );
        thread_safe_cout( "Was popped? -", std::boolalpha, pElem != nullptr );
        BOOST_CHECK( pElem != nullptr );

        thread_safe_cout( "Popped one element:", *pElem );
    } );

    std::thread t1Push( [ &elements ] {
        int * pNumber = new int{ 10 };

        sleep_for( 3000 );
        thread_safe_cout( "Adding '10'" );
        elements->enqueue( pNumber );
    } );


    t1Pop.join();
    t1Push.join();

    //  TODO: split test into { initialization; workflow; checking } blocks
}

/*----------------------------------------------------------------------------*/

BOOST_AUTO_TEST_SUITE_END()