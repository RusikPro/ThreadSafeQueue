#ifndef __SHAREDQUEUE_SRC_IMPL_SHAREDQUEUE_H__
#define __SHAREDQUEUE_SRC_IMPL_SHAREDQUEUE_H__

/*----------------------------------------------------------------------------*/

#include "IQueue.h"

#include <condition_variable>
#include <memory>
#include <mutex>

/*----------------------------------------------------------------------------*/

template < typename _T >
class SharedQueue
    :   public IQueue < _T >
{
public:

    explicit SharedQueue ( std::size_t _size );

    ~SharedQueue ();

    int count () const noexcept override;

    void enqueue ( _T * _pNewValue ) override;

    bool enqueue ( _T * _pNewValue, int _millisecondsTimeout ) override;

    _T * dequeue () override;

    _T * dequeue ( int _millisecondsTimeout ) override;

private:

    struct Node;

    Node * get_tail ();

private:

    std::mutex m_headMutex;
    std::unique_ptr< Node > m_pHead;
    std::mutex m_tailMutex;
    mutable std::mutex m_sizeMutex;
    Node * m_pTail;
    std::condition_variable m_pushDataCond;
    std::condition_variable m_popDataCond;

    mutable std::size_t m_currentQueueSizeLockable;
    const std::size_t m_queueSize;
};

/*----------------------------------------------------------------------------*/

#include "impl/SharedQueue.cpp"

/*----------------------------------------------------------------------------*/

#endif // __SHAREDQUEUE_SRC_IMPL_SHAREDQUEUE_H__
