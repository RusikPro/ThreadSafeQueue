#include "impl/SharedQueue.h"

#include <chrono>
#include <iostream>

/*----------------------------------------------------------------------------*/

template < typename _T >
struct SharedQueue< _T >::Node
{
    _T * data;
    std::unique_ptr< Node > next;
};

/*----------------------------------------------------------------------------*/

template < typename _T >
SharedQueue< _T >::SharedQueue ( std::size_t _size )
    :   m_pHead( std::make_unique< Node >() )
    ,   m_pTail( m_pHead.get() )
    ,   m_currentQueueSizeLockable( 0 )
    ,   m_queueSize( _size )
{
}

/*----------------------------------------------------------------------------*/

template < typename _T >
SharedQueue< _T >::~SharedQueue ()
{
}

/*----------------------------------------------------------------------------*/

template < typename _T >
int SharedQueue< _T >::count () const noexcept
{
    return m_currentQueueSizeLockable.load();
}

/*----------------------------------------------------------------------------*/

template < typename _T >
void SharedQueue< _T >::enqueue ( _T * _pNewValue )
{
    std::unique_ptr< Node > pNewNode( std::make_unique< Node >() );
    {
        std::unique_lock< std::mutex > sizeLock( m_mutex );
        m_popDataCond.wait( sizeLock, [ this ] ()
        {
            return m_currentQueueSizeLockable < m_queueSize;
        } );
        ++m_currentQueueSizeLockable;


        m_pTail->data = _pNewValue;
        Node * const pNewTail = pNewNode.get();
        m_pTail->next = std::move( pNewNode );
        m_pTail = pNewTail;
    }

    m_pushDataCond.notify_one();
}

/*----------------------------------------------------------------------------*/

template < typename _T >
bool SharedQueue< _T >::enqueue ( _T * _pNewValue, int _millisecondsTimeout )
{
    using namespace std::chrono;

    std::unique_ptr< Node > pNewNode( std::make_unique< Node >() );
    {
        std::unique_lock< std::mutex > sizeLock( m_mutex );
        bool notFull = m_popDataCond.wait_for(
                sizeLock
            ,   milliseconds( _millisecondsTimeout )
            ,   [ this ] ()
                {
                    return m_currentQueueSizeLockable < m_queueSize;
                }
        );

        if ( !notFull )
        {
            return false;
        }

        ++m_currentQueueSizeLockable;

        // Update the queue's tail
        m_pTail->data = _pNewValue;
        Node *const pNewTail = pNewNode.get();
        m_pTail->next = std::move(pNewNode);
        m_pTail = pNewTail;
    }
    // Notify one consumer that new data is available
    m_pushDataCond.notify_one();
    return true;
}

/*----------------------------------------------------------------------------*/

template < typename _T >
_T * SharedQueue< _T >::dequeue ()
{
    std::unique_lock< std::mutex > queueLock( m_mutex );
    m_pushDataCond.wait( queueLock, [ this ] ()
    {
        return m_pHead.get() != getTail(); // Wait until queue is not empty
    } );

    --m_currentQueueSizeLockable;

    // Remove the head node
    auto pOldHead = std::move(m_pHead);
    m_pHead = std::move(pOldHead->next);

    m_popDataCond.notify_one();

    return pOldHead->data;
}

/*----------------------------------------------------------------------------*/

template < typename _T >
_T * SharedQueue< _T >::dequeue ( int _millisecondsTimeout )
{
    using namespace std::chrono;

    std::unique_lock<std::mutex> queueLock(m_mutex);
    bool dequeued = m_pushDataCond.wait_for(
            queueLock
        ,   milliseconds( _millisecondsTimeout )
        ,   [ this ] ()
            {
                // Wait until queue is not empty
                return m_pHead.get() != getTail();
            }
    );

    if ( !dequeued )
    {
        return nullptr;
    }

    --m_currentQueueSizeLockable;

    // Remove the head node
    auto pOldHead = std::move( m_pHead );
    m_pHead = std::move( pOldHead->next );

    m_popDataCond.notify_one();

    return pOldHead->data;
}

/*----------------------------------------------------------------------------*/

template < typename _T >
typename SharedQueue< _T >::Node * SharedQueue< _T >::getTail ()
{
    return m_pTail;
}

/*----------------------------------------------------------------------------*/
