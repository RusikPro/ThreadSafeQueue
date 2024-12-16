#ifndef __SHAREDQUEUE_SRC_IMPL_SHAREDQUEUEPIMPL_H__
#define __SHAREDQUEUE_SRC_IMPL_SHAREDQUEUEPIMPL_H__

/*----------------------------------------------------------------------------*/

#include "IQueue.h"
#include "QueueImpl.h"

#include <condition_variable>
#include <memory>
#include <mutex>

/*----------------------------------------------------------------------------*/

template < typename _DataT >
struct Data : IData
{
    explicit Data ( _DataT * _pValue )
        :   m_pValue( _pValue )
    {}

    void* get() override {
        return static_cast<void*>(m_pValue);  // Return as void*
    }

    _DataT * m_pValue;
};

/*----------------------------------------------------------------------------*/

template < typename _T >
class SharedQueuePImpl
    :   public IQueue < _T >
{
private:

    std::unique_ptr< SharedQueueImpl > pImpl;

public:

    explicit SharedQueuePImpl ( std::size_t _size )
        :   pImpl( std::make_unique< SharedQueueImpl >( _size ) )
    {
    }

    ~SharedQueuePImpl () = default;

    int count () const noexcept override
    {
        return pImpl->count();
    }

    void enqueue ( _T * _pNewValue ) override
    {
        pImpl->enqueue( new Data< _T >( _pNewValue ) );
    }

    bool enqueue ( _T * _pNewValue, int _millisecondsTimeout ) override
    {
        return pImpl->enqueue(
            new Data< _T >( _pNewValue ), _millisecondsTimeout
        );
    }

    _T * dequeue () override
    {
        auto * pData = pImpl->dequeue();
        auto * pElem = reinterpret_cast< _T * >( pData->get() );
        delete pData;
        return pElem;
    }

    _T * dequeue ( int _millisecondsTimeout ) override
    {
        auto * pData = pImpl->dequeue( _millisecondsTimeout );
        auto * pElem = reinterpret_cast< _T * >( pData->get() );
        delete pData;
        return pElem;
    }
};

/*----------------------------------------------------------------------------*/

#endif // __SHAREDQUEUE_SRC_IMPL_SHAREDQUEUEPIMPL_H__
