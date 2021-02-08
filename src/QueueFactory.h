#ifndef __SHAREDQUEUE_SRC_QUEUEFACTORY_H___
#define __SHAREDQUEUE_SRC_QUEUEFACTORY_H___

/*----------------------------------------------------------------------------*/

#include "impl/SharedQueue.h"
#include "impl/SharedQueuePImpl.h"

/*----------------------------------------------------------------------------*/

#include <memory>

class QueueFactory
{
public:

    template < typename _T >
    static std::unique_ptr< IQueue< _T > >
    createStandardSharedQueue ( std::size_t _size )
    {
        return std::make_unique< SharedQueue< _T > >( _size );
    }

    template < typename _T >
    static std::unique_ptr< IQueue< _T > >
    createSharedQueueWithPImpl ( std::size_t _size )
    {
        return std::make_unique< SharedQueuePImpl< _T > >( _size );
    }
};

/*----------------------------------------------------------------------------*/

#endif // __SHAREDQUEUE_SRC_QUEUEFACTORY_H___